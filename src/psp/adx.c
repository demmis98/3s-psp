/**
 * CRI ADX decoder for PSP
 * Based on working feat/fullscreen-scaling implementation
 * Uses sceAudioChReserve + sceAudioOutputBlocking (proven working)
 */
#include "psp/adx.h"
#include "psp/afs.h"

#include <pspaudio.h>
#include <pspaudiolib.h>
#include <pspthreadman.h>
#include <string.h>
#include <malloc.h>
#include <math.h>

#include "Game/main.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PSP_SAMPLES 1024
#define MAX_CH 2

typedef struct { s32 p1, p2; } ChSt;

static struct {
    volatile AdxStat stat;
    volatile s32 volume;
    volatile s32 running;

    u8 *buf;
    s32 buf_size;
    s32 data_offset;
    s32 channels;
    s32 sample_rate;
    s32 block_size;
    s32 spb;
    s32 frame_size;
    s32 coeff1, coeff2;
    ChSt ch[MAX_CH];
    s32 pos;

    s32 loop_enabled;
    s32 loop_start_byte;
    s32 loop_end_byte;
} P;

#define FREE_QUEUE_SIZE 3
static u8* free_queue[FREE_QUEUE_SIZE];

volatile u16 Sound_Mono;
volatile u16 Sound_Muffled;

static u16 rb16(const u8 *p) { return (p[0]<<8)|p[1]; }
static u32 rb32(const u8 *p) { return (p[0]<<24)|(p[1]<<16)|(p[2]<<8)|p[3]; }

static inline s32 clamp16(s32 x) {
    if ((u32)(x + 32768) > 65535)
        x = (x < 0) ? -32768 : 32767;
    return x;
}

static const s8 nibble_lut[16] = {
     0, 1, 2, 3, 4, 5, 6, 7,
    -8,-7,-6,-5,-4,-3,-2,-1
};

static void dec_block(const u8 *blk, ChSt *ch, s16 *out, s32 stride, s32 spb, s32 c1, s32 c2) {
    s32 scale = rb16(blk);
    const u8 *d = blk + 2;
    s32 p1 = ch->p1, p2 = ch->p2;
    s32 i;
    s32 spb_2 = spb >> 1;
    for (i = 0; i < spb_2; i++) {
        s32 b = d[i];
        s32 n1 = (b >> 4);
        n1 = (n1 ^ 8) - 8;
        s32 s1 = n1 * scale + ((c1 * p1 + c2 * p2) >> 12);
        s1 = clamp16(s1);
        *out = (s16)s1; out += stride; p2 = p1; p1 = s1;

        s32 n2 = b & 0xF;
        n2 = (n2 ^ 8) - 8;
        s32 s2 = n2 * scale + ((c1 * p1 + c2 * p2) >> 12);
        s2 = clamp16(s2);
        *out = (s16)s2; out += stride; p2 = p1; p1 = s2;
    }
    ch->p1 = p1; ch->p2 = p2;
}

static s32 parse_header(const u8 *h, s32 size) {
    if (size < 20 || h[0] != 0x80) return -1;
    P.data_offset = (s32)rb16(h + 2) + 4;
    P.channels = h[7];
    if (P.channels < 1 || P.channels > MAX_CH) return -1;
    P.sample_rate = (s32)rb32(h + 8);
    P.block_size = h[5];
    if (P.block_size < 3) return -1;
    P.spb = (P.block_size - 2) * 2;
    P.frame_size = P.block_size * P.channels;

    double w = 2.0 * M_PI * 500.0 / (double)P.sample_rate;
    double x = sqrt(2.0) - cos(w);
    double y = sqrt(2.0) - 1.0;
    double z = (x - sqrt((x+y)*(x-y))) / y;
    P.coeff1 = (s32)(z * 8192.0);
    P.coeff2 = (s32)(z * z * -4096.0);
    memset(P.ch, 0, sizeof(P.ch));

    P.loop_enabled = 0;
    u8 ver = h[0x12];
    if (ver == 3 && size > 0x2C) {
        /* ADX v3 loop structure:
           0x18: loop count (u32 BE) — non-zero = has loops
           0x1C: loop start SAMPLE (u32 BE)
           0x20: loop start BYTE offset from file start (u32 BE)
           0x24: loop end SAMPLE (u32 BE)
           0x28: loop end BYTE offset from file start (u32 BE) */
        if (rb32(h + 0x18) != 0) {
            P.loop_enabled = 1;
            P.loop_start_byte = (s32)rb32(h + 0x20) - P.data_offset;
            P.loop_end_byte = (s32)rb32(h + 0x28) - P.data_offset;
            if (P.loop_start_byte < 0) P.loop_start_byte = 0;
            if (P.loop_end_byte < 0) P.loop_end_byte = 0;
        }
    }
    return 0;
}

// Decode one source sample (called by resampler)
static s16 blk_buf[MAX_CH][64];
static s32 blk_pos = 0, blk_avail = 0;
static s16 last_l = 0, last_r = 0;
static u32 resample_frac = 0;
static volatile s32 adx_paused = 0;

/* Async ADX loading state */
static volatile s32 adx_loading = 0;  /* 1 = async read in progress */
static u8 *adx_load_buf = NULL;
static u32 adx_load_size = 0;

/* Deferred free — old buffer freed after callback can't reference it */
static u8 *adx_free_pending = NULL;
static s32 adx_buf_owned = 0;  /* 1 = P.buf was malloc'd, 0 = static */

/* Seamless preload — next segment ready for instant callback swap */
static u8 *adx_next_buf = NULL;
static u32 adx_next_size = 0;
static s32 adx_next_data_offset = 0;
static s32 adx_next_channels = 0;
static s32 adx_next_sample_rate = 0;
static s32 adx_next_block_size = 0;
static s32 adx_next_spb = 0;
static s32 adx_next_frame_size = 0;
static s32 adx_next_coeff1 = 0, adx_next_coeff2 = 0;
static s32 adx_next_loop_enabled = 0;
static s32 adx_next_loop_start = 0, adx_next_loop_end = 0;
static volatile s32 adx_next_ready = 0;  /* 1 = callback can swap to this */
static volatile s32 adx_next_consumed = 0; /* 1 = callback took the preload */
static volatile s32 audio_size = 0;
static volatile s32 vol_32 = 0;

static void decode_next_sample(void) {
    if (P.stat != ADX_STAT_PLAYING || P.buf == NULL) {
        last_l = last_r = 0;
        return;
    }

    if (blk_pos >= blk_avail) {
        audio_size = P.buf_size - P.data_offset;
        if (P.pos + P.frame_size > audio_size) {
            if (P.loop_enabled && P.loop_start_byte >= 0) {
                P.pos = P.loop_start_byte;
                memset(P.ch, 0, sizeof(P.ch));
            } else if (adx_next_ready && adx_next_buf) {
                /* Seamless swap inside callback — zero gap */
                adx_free_pending = P.buf;
                P.buf = adx_next_buf;
                P.buf_size = adx_next_size;
                P.data_offset = adx_next_data_offset;
                P.channels = adx_next_channels;
                P.sample_rate = adx_next_sample_rate;
                P.block_size = adx_next_block_size;
                P.spb = adx_next_spb;
                P.frame_size = adx_next_frame_size;
                P.coeff1 = adx_next_coeff1;
                P.coeff2 = adx_next_coeff2;
                P.loop_enabled = adx_next_loop_enabled;
                P.loop_start_byte = adx_next_loop_start;
                P.loop_end_byte = adx_next_loop_end;
                P.pos = 0;
                adx_next_buf = NULL;
                adx_next_ready = 0;
                adx_next_consumed = 1;
                /* Don't reset P.ch — keep ADPCM history continuous */
            } else {
                P.stat = ADX_STAT_PLAYEND;
                last_l = last_r = 0;
                return;
            }
        }
        if (P.loop_enabled && P.loop_end_byte > 0 && P.pos >= P.loop_end_byte) {
            P.pos = P.loop_start_byte;
            memset(P.ch, 0, sizeof(P.ch));
        }

        const u8 *src = P.buf + P.data_offset + P.pos;
        s32 c;
        for (c = 0; c < P.channels; c++)
            dec_block(src + c * P.block_size, &P.ch[c], blk_buf[c], 1, P.spb, P.coeff1, P.coeff2);
        P.pos += P.frame_size;
        blk_pos = 0;
        blk_avail = P.spb;
    }

    vol_32 = (P.volume + (P.volume << 1)) >> 2;  /* ~75% — BGM sits behind SFX */
    last_l = (s16)((blk_buf[0][blk_pos] * vol_32) >> 7);
    last_r = (P.channels == 2) ? (s16)((blk_buf[1][blk_pos] * vol_32) >> 7) : last_l;
    blk_pos++;
}

// pspaudiolib callback on channel 1 — resamples from source rate to 44100Hz
#define PSP_OUTPUT_RATE 44100

static void adx_psp_callback(void *buf, unsigned int reqn, void *userdata) {
    s16 *out = (s16 *)buf;

    if (P.stat != ADX_STAT_PLAYING || P.buf == NULL || adx_paused) {
        memset(buf, 0, reqn * 4);
        return;
    }

    u32 step = ((u32)P.sample_rate << 16) / PSP_OUTPUT_RATE;
    u16 last_m = 0;

    if(Sound_Mono){
        if(Sound_Muffled){
            for (u32 i = 0; i < reqn; i++) {
                resample_frac += step;
                while (resample_frac >= 0x10000) {
                    decode_next_sample();
                    resample_frac -= 0x10000;
                }
                if(!(i & 0x3))
                    last_m = (last_l + last_r) >> 1;
                out[i * 2]     = last_m;
                out[i * 2 + 1] = last_m;
            }
        }
        else{
            for (u32 i = 0; i < reqn; i++) {
                resample_frac += step;
                while (resample_frac >= 0x10000) {
                    decode_next_sample();
                    resample_frac -= 0x10000;
                }
                last_m = (last_l + last_r) >> 1;
                out[i * 2]     = last_m;
                out[i * 2 + 1] = last_m;
            }
        }
    }
    else{
        if(Sound_Muffled){
            u16 l_temp = 0, r_temp = 0;
            for (u32 i = 0; i < reqn; i++) {
                resample_frac += step;
                while (resample_frac >= 0x10000) {
                    decode_next_sample();
                    resample_frac -= 0x10000;
                }
                if(!(i & 0x3)){
                    l_temp = last_l;
                    r_temp = last_r;
                }
                out[i * 2]     = l_temp;
                out[i * 2 + 1] = r_temp;
            }
        }
        else{
            for (u32 i = 0; i < reqn; i++) {
                resample_frac += step;
                while (resample_frac >= 0x10000) {
                    decode_next_sample();
                    resample_frac -= 0x10000;
                }
                out[i * 2]     = last_l;
                out[i * 2 + 1] = last_r;
            }
        }
    }
}

void adxInit(void) {
    memset(&P, 0, sizeof(P));
    P.stat = ADX_STAT_STOP;
    P.volume = 127;
    // Use pspaudiolib callback on channel 1 (channel 0 = SPU/SFX)
    pspAudioSetChannelCallback(1, adx_psp_callback, NULL);
}

void adxShutdown(void) {
    pspAudioSetChannelCallback(1, NULL, NULL);
    if (P.buf) free(P.buf);
}

static void adxPlayInternal(u16 fnum, s32 sync) {
    #ifdef PSP_FAT
        return;
    #endif
    u32 file_size = afsGetFileSize(fnum);
    if (file_size == 0) return;

    u8 *buf = (u8 *)memalign(16, file_size);
    if (!buf) return;

    /* Cancel any pending async load and stale preload */
    if (adx_load_buf) { free(adx_load_buf); adx_load_buf = NULL; }
    adx_loading = 0;
    if (adx_next_buf) { free(adx_next_buf); adx_next_buf = NULL; }
    adx_next_ready = 0;
    adx_next_consumed = 0;

    if (sync) {
        if (!afsReadSync(fnum, buf, file_size)) {
            free(buf);
            return;
        }
        /* Stop before parse — parse_header modifies P fields that the
           callback reads. Without this, the callback decodes old audio
           data with new coefficients/offsets → static blast. */
        P.stat = ADX_STAT_STOP;
        if (parse_header(buf, file_size) < 0) {
            free(buf);
            return;
        }
        u8 *old = P.buf;
        s32 old_owned = adx_buf_owned;
        P.buf = buf;
        P.buf_size = file_size;
        P.pos = 0;
        blk_pos = blk_avail = 0;
        resample_frac = 0;
        adx_buf_owned = 1;
        P.stat = ADX_STAT_PLAYING;
        if (old && old_owned) adx_free_pending = old;
    } else {
        P.stat = ADX_STAT_STOP;
        if (P.buf && adx_buf_owned) { adx_free_pending = P.buf; }
        P.buf = NULL;
        P.pos = 0;
        adx_load_buf = buf;
        adx_load_size = file_size;
        adx_loading = 1;
        afsReadAsync(fnum, buf, file_size);
    }
}

void adxPlay(u16 fnum) {
    adxPlayInternal(fnum, 1);  /* PSP: always sync — async races with stop calls */
}

void adxPlaySync(u16 fnum) {
    adxPlayInternal(fnum, 1);  /* sync for seamless */
}

void adxPlayLoop(u16 fnum, u32 ls, u32 le) { adxPlay(fnum); }

void adxStop(void) {
    P.stat = ADX_STAT_STOP;
    if (P.buf && adx_buf_owned) { adx_free_pending = P.buf; }
    P.buf = NULL;
    adx_buf_owned = 0;
    if (adx_load_buf) { free(adx_load_buf); adx_load_buf = NULL; }
    if (adx_next_buf) { free(adx_next_buf); adx_next_buf = NULL; }
    adx_loading = 0;
    adx_next_ready = 0;
    adx_next_consumed = 0;
    P.pos = 0;
}

void adxSetVolume(s32 vol) {
    if (vol < 0) vol = 0;
    if (vol > 127) vol = 127;
    P.volume = vol;
}

AdxStat adxGetStat(void) { return P.stat; }

void adxUpdate(void) {
    if (free_queue[0]) free(free_queue[0]);

    for (int i = 1; i < FREE_QUEUE_SIZE; i++)
        free_queue[i - 1] = free_queue[i];

    free_queue[FREE_QUEUE_SIZE - 1] = adx_free_pending;
    adx_free_pending = NULL;

    /* Check if async ADX load completed */
    if (adx_loading && afsCheckRead()) {
        adx_loading = 0;
        u8 *buf = adx_load_buf;
        u32 size = adx_load_size;
        adx_load_buf = NULL;

        if (parse_header(buf, size) < 0) {
            free(buf);
            return;
        }
        P.buf = buf;
        P.buf_size = size;
        P.pos = 0;
        blk_pos = blk_avail = 0;
        resample_frac = 0;
        P.stat = ADX_STAT_PLAYING;
    }
}

// ── Compatibility wrappers for the new sound system ──────────────
// Bridge the branch's ADX_* API to our existing adx* functions.
// Supports seamless multi-track, memory playback, and AFS streaming.

static s32 adx_entry_queue[16];
static s32 adx_entry_count = 0;
static void adx_preload_next_segment(void);

void ADX_Init(void) {
    adxInit();
    adx_paused = 0;
    adx_entry_count = 0;
}

void ADX_Exit(void) {
    adxShutdown();
}

void ADX_Stop(void) {
    adxStop();
}

void ADX_StartAfs(s32 fnum) {
    adx_paused = 0;
    adxPlay((u16)fnum);
}

void ADX_EntryAfs(s32 fnum) {
    if (adx_entry_count < 16) {
        adx_entry_queue[adx_entry_count++] = fnum;
        /* Preload immediately if a track is playing and no preload pending */
        if (P.stat == ADX_STAT_PLAYING && !adx_next_ready) {
            adx_preload_next_segment();
        }
    }
}

/* Parse ADX header directly into adx_next_* fields without touching P */
static s32 parse_header_into_next(const u8 *h, s32 size) {
    if (size < 20 || h[0] != 0x80) return -1;
    adx_next_data_offset = (s32)rb16(h + 2) + 4;
    adx_next_channels = h[7];
    if (adx_next_channels < 1 || adx_next_channels > MAX_CH) return -1;
    adx_next_sample_rate = (s32)rb32(h + 8);
    adx_next_block_size = h[5];
    if (adx_next_block_size < 3) return -1;
    adx_next_spb = (adx_next_block_size - 2) * 2;
    adx_next_frame_size = adx_next_block_size * adx_next_channels;

    double w = 2.0 * M_PI * 500.0 / (double)adx_next_sample_rate;
    double x = sqrt(2.0) - cos(w);
    double y = sqrt(2.0) - 1.0;
    double z = (x - sqrt((x+y)*(x-y))) / y;
    adx_next_coeff1 = (s32)(z * 8192.0);
    adx_next_coeff2 = (s32)(z * z * -4096.0);

    adx_next_loop_enabled = 0;
    u8 ver = h[0x12];
    if (ver == 3 && size > 0x28) {
        if (rb16(h + 0x16) == 1) {
            adx_next_loop_enabled = 1;
            adx_next_loop_start = (s32)rb32(h + 0x1C) - adx_next_data_offset;
            adx_next_loop_end = (s32)rb32(h + 0x24) - adx_next_data_offset;
            if (adx_next_loop_start < 0) adx_next_loop_start = 0;
            if (adx_next_loop_end < 0) adx_next_loop_end = 0;
        }
    }
    return 0;
}

/* Preload next queued segment so the callback can swap with zero gap */
#define ADX_PRELOAD_SIZE (256 * 1024)

static void adx_preload_next_segment(void) {
    if (adx_next_ready || adx_entry_count <= 0) return;

    u16 fnum = (u16)adx_entry_queue[0];
    u32 file_size = afsGetFileSize(fnum);
    if (file_size == 0) return;

    u32 preload_size = file_size;
    if (preload_size > ADX_PRELOAD_SIZE)
        preload_size = ADX_PRELOAD_SIZE;

    /* IMPORTANT: allocate full file size */
    u8 *buf = (u8 *)memalign(16, file_size);
    if (!buf) return;

    /* only read first chunk */
    if (!afsReadSync(fnum, buf, preload_size)) {
        free(buf);
        return;
    }

    if (parse_header_into_next(buf, preload_size) < 0) {
        free(buf);
        return;
    }

    adx_next_buf = buf;

    /* IMPORTANT: total file size */
    adx_next_size = file_size;

    adx_next_ready = 1;
}

void ADX_StartSeamless(void) {
    adx_paused = 0;

    if (adx_entry_count > 0) {
        adxPlaySync((u16)adx_entry_queue[0]);
        for (s32 i = 1; i < adx_entry_count; i++) {
            adx_entry_queue[i - 1] = adx_entry_queue[i];
        }
        adx_entry_count--;

        /* Preload the NEXT segment so callback can swap instantly */
        adx_preload_next_segment();
    }
}

void ADX_StartMem(void* data, u32 size) {
    if (!data || size == 0) return;

    P.stat = ADX_STAT_STOP;
    if (P.buf && adx_buf_owned) { adx_free_pending = P.buf; }
    P.buf = NULL;
    P.pos = 0;

    /* Clear stale seamless preload from previous track */
    if (adx_next_buf) { free(adx_next_buf); adx_next_buf = NULL; }
    adx_next_ready = 0;
    adx_next_consumed = 0;
    if (adx_load_buf) { free(adx_load_buf); adx_load_buf = NULL; }
    adx_loading = 0;

    if (parse_header((u8*)data, size) < 0) return;

    P.buf = (u8*)data;
    P.buf_size = size;
    P.pos = 0;
    blk_pos = blk_avail = 0;
    resample_frac = 0;
    adx_buf_owned = 0;  /* static buffer, don't free */
    P.stat = ADX_STAT_PLAYING;
    adx_paused = 0;
}

s32 ADX_GetState(void) {
    return (s32)adxGetStat();
}

void ADX_SetOutVol(s32 vol) {
    /* The volume table returns values in a -999..0 range.
       Map to our 0..127 range: treat 0 as max, -999 as silent. */
    s32 mapped;
    if (vol <= -999) {
        mapped = 0;
    } else if (vol >= 0) {
        mapped = 127;
    } else {
        mapped = (999 + vol) * 127 / 999;
    }
    adxSetVolume(mapped);
}

void ADX_SetMuffle(s32 muffle) {
    Sound_Muffled = muffle;
}

void ADX_SetMono(s32 mono) {
    Sound_Mono = mono;
}

s32 ADX_GetNumFiles(void) {
    return adx_entry_count;
}

void ADX_ResetEntry(void) {
    adx_entry_count = 0;
}

void ADX_Pause(s32 pause) {
    adx_paused = pause;
}

/* Suspend/resume for PSP sleep — called from power callback thread */
static volatile s32 adx_was_playing = 0;

void adxSuspend(void) {
    adx_was_playing = (P.stat == ADX_STAT_PLAYING && !adx_paused);
    adx_paused = 1;  /* silence callback immediately */
    //pspAudioSetChannelCallback(1, NULL, NULL);
}

void adxResume(void) {
    if (adx_was_playing) {
        adx_paused = 0;  /* restore playback */
    }
    pspAudioSetChannelCallback(1, adx_psp_callback, NULL);
}

s32 ADX_IsPaused(void) {
    return adx_paused;
}

void ADX_ProcessTracks(void) {
    adxUpdate();

    /* Callback consumed the preload — advance queue and preload next */
    if (adx_next_consumed) {
        adx_next_consumed = 0;
        if (adx_entry_count > 0) {
            for (s32 i = 1; i < adx_entry_count; i++) {
                adx_entry_queue[i - 1] = adx_entry_queue[i];
            }
            adx_entry_count--;
        }
        adx_preload_next_segment();
    }

    /* Fallback: if no preload was ready and track ended, use sync swap */
    if (adx_entry_count > 0 && adxGetStat() == ADX_STAT_PLAYEND) {
        ADX_StartSeamless();
    }
}