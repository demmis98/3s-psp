#include "psp/afs.h"
#include <string.h>
#include <pspthreadman.h>

#define AFS_MAGIC 0x00534641  /* "AFS\0" little-endian */
#define SECTOR_SIZE 2048

static AFS afs;
static s32 afs_ready = 0;
static char afs_path[256];

/* Background I/O thread for async reads */
static SceUID io_thread_id = -1;
static SceUID io_sema = -1;
static volatile u16 io_fnum;
static volatile void* io_buf;
static volatile u32 io_size;
static volatile s32 io_shutdown = 0;
static volatile s32 afs_suspending = 0;  /* set during PSP sleep transition */

/* Forward declarations for self-healing fd reopen */
static void afs_reopen_sync_fd(void);
static void afs_reopen_async_fd(void);

static int afs_io_thread(SceSize args, void* argp) {
    (void)args; (void)argp;

    while (!io_shutdown) {
        /* Wait for work */
        sceKernelWaitSema(io_sema, 1, NULL);
        if (io_shutdown) break;

        u16 fnum = io_fnum;
        void* buf = (void*)io_buf;
        u32 size = io_size;

        if (fnum >= afs.entry_count || afs.async_fd < 0 || !buf) {
            afs.async_result = -1;
            afs.async_busy = 0;
            continue;
        }

        u32 read_size = size;
        if (read_size > afs.entries[fnum].size) {
            read_size = afs.entries[fnum].size;
        }

        sceIoLseek32(afs.async_fd, afs.entries[fnum].offset, PSP_SEEK_SET);
        s32 result = sceIoRead(afs.async_fd, buf, read_size);

        /* Self-heal after PSP sleep — async fd may be stale.
           Don't reopen if we're in the suspend transition — afsReopen()
           handles it on RESUME_COMPLETE. */
        if (result < 0 && !afs_suspending) {
            afs_reopen_async_fd();
            if (afs.async_fd >= 0) {
                sceIoLseek32(afs.async_fd, afs.entries[fnum].offset, PSP_SEEK_SET);
                result = sceIoRead(afs.async_fd, buf, read_size);
            }
        }

        afs.async_result = result;
        afs.async_busy = 0;
    }

    return 0;
}

s32 afsInit(const char* path) {
    u32 magic;

    memset(&afs, 0, sizeof(afs));
    afs.fd = -1;
    afs.async_fd = -1;
    strncpy(afs_path, path, sizeof(afs_path) - 1);

    /* Primary fd for sync reads */
    afs.fd = sceIoOpen(path, PSP_O_RDONLY, 0);
    if (afs.fd < 0) {
        return 0;
    }

    sceIoRead(afs.fd, &magic, 4);
    if (magic != AFS_MAGIC) {
        sceIoClose(afs.fd);
        afs.fd = -1;
        return 0;
    }

    sceIoRead(afs.fd, &afs.entry_count, 4);
    if (afs.entry_count > AFS_MAX_ENTRIES) {
        afs.entry_count = AFS_MAX_ENTRIES;
    }

    sceIoRead(afs.fd, afs.entries, afs.entry_count * sizeof(AFSEntry));

    /* Second fd for async reads (separate seek position) */
    afs.async_fd = sceIoOpen(path, PSP_O_RDONLY, 0);

    /* Create I/O thread and semaphore */
    io_shutdown = 0;
    io_sema = sceKernelCreateSema("afsIO", 0, 0, 1, NULL);
    io_thread_id = sceKernelCreateThread("afsIO", afs_io_thread, 0x18, 0x4000, 0, NULL);
    if (io_thread_id >= 0) {
        sceKernelStartThread(io_thread_id, 0, NULL);
    }

    afs_ready = 1;
    return 1;
}

s32 afsIsReady(void) {
    return afs_ready;
}

void afsClose(void) {
    /* Shut down I/O thread */
    if (io_thread_id >= 0) {
        io_shutdown = 1;
        sceKernelSignalSema(io_sema, 1);
        sceKernelWaitThreadEnd(io_thread_id, NULL);
        sceKernelDeleteThread(io_thread_id);
        io_thread_id = -1;
    }
    if (io_sema >= 0) {
        sceKernelDeleteSema(io_sema);
        io_sema = -1;
    }

    if (afs.fd >= 0) {
        sceIoClose(afs.fd);
        afs.fd = -1;
    }
    if (afs.async_fd >= 0) {
        sceIoClose(afs.async_fd);
        afs.async_fd = -1;
    }
    afs_ready = 0;
}

void afsSuspend(void) {
    /* Called from power callback on SUSPENDING.
       Close fds to unblock any hung sceIoRead in the I/O thread.
       Reads will fail with EBADF instead of hanging forever. */
    afs_suspending = 1;

    if (afs.fd >= 0) { sceIoClose(afs.fd); afs.fd = -1; }
    if (afs.async_fd >= 0) { sceIoClose(afs.async_fd); afs.async_fd = -1; }
}

void afsReopen(void) {
    /* Re-open file descriptors after PSP sleep/resume. */
    if (afs_path[0] == 0) return;

    /* Close any leftover stale fds */
    if (afs.fd >= 0) sceIoClose(afs.fd);
    if (afs.async_fd >= 0) sceIoClose(afs.async_fd);

    afs.fd = sceIoOpen(afs_path, PSP_O_RDONLY, 0);
    afs.async_fd = sceIoOpen(afs_path, PSP_O_RDONLY, 0);
    /* Force-clear async state — I/O thread may have been stuck */
    afs.async_busy = 0;
    afs.async_result = -1;
    afs_suspending = 0;
}

u32 afsGetFileSize(u16 fnum) {
    if (fnum >= afs.entry_count) return 0;
    return afs.entries[fnum].size;
}

u32 afsGetSectorCount(u16 fnum) {
    u32 size = afsGetFileSize(fnum);
    return (size + SECTOR_SIZE - 1) / SECTOR_SIZE;
}

/* Try to reopen just the sync fd (thread-safe — only touches afs.fd) */
static void afs_reopen_sync_fd(void) {
    if (afs_path[0] == 0) return;
    if (afs.fd >= 0) sceIoClose(afs.fd);
    afs.fd = sceIoOpen(afs_path, PSP_O_RDONLY, 0);
}

/* Try to reopen just the async fd (called from I/O thread only) */
static void afs_reopen_async_fd(void) {
    if (afs_path[0] == 0) return;
    if (afs.async_fd >= 0) sceIoClose(afs.async_fd);
    afs.async_fd = sceIoOpen(afs_path, PSP_O_RDONLY, 0);
}

s32 afsReadSync(u16 fnum, void* buf, u32 size) {
    if (fnum >= afs.entry_count || !afs_ready) return 0;
    if (afs.entries[fnum].size == 0) return 0;

    u32 read_size = size;
    if (read_size > afs.entries[fnum].size) {
        read_size = afs.entries[fnum].size;
    }

    if (afs.fd < 0) afs_reopen_sync_fd();
    if (afs.fd < 0) return 0;

    sceIoLseek32(afs.fd, afs.entries[fnum].offset, PSP_SEEK_SET);
    s32 read = sceIoRead(afs.fd, buf, read_size);

    /* Self-heal after PSP sleep — fd may be stale */
    if (read < 0) {
        afs_reopen_sync_fd();
        if (afs.fd < 0) return 0;
        sceIoLseek32(afs.fd, afs.entries[fnum].offset, PSP_SEEK_SET);
        read = sceIoRead(afs.fd, buf, read_size);
    }

    return (read >= 0) ? 1 : 0;
}

s32 afsReadAsync(u16 fnum, void* buf, u32 size) {
    /* Fall back to sync — async I/O thread can deadlock on PSP sleep/resume */
    return afsReadSync(fnum, buf, size);
}

s32 afsCheckRead(void) {
    if (afs.async_busy) {
        return 0;  /* Still reading */
    }
    return 1;  /* Done */
}
