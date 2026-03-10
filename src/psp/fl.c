#include "fl.h"

#include <malloc.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <pspdebug.h>
#include <string.h>

#include "AcrSDK/common/fbms.h"
#include "AcrSDK/common/mlPAD.h"
#include "AcrSDK/common/prilay.h"
#include "AcrSDK/common/memfound.h"
#include "Game/color3rd.h"
#include "Game/AcrUtil.h"
#include "psp/PPGFile.h"
#include "psp/flps2etc.h"

#include "common.h"

#include "sdk/libgraph.h"

static const unsigned short testPalette[64] = {
0x0001,0x0015,0x002B,0x003F,
0x0281,0x0295,0x02AB,0x02BF,
0x0541,0x0555,0x056B,0x057F,
0x07C1,0x07D5,0x07EB,0x07FF,

0x5001,0x5015,0x502B,0x503F,
0x5281,0x5295,0x52AB,0x52BF,
0x5541,0x5555,0x556B,0x557F,
0x57C1,0x57D5,0x57EB,0x57FF,

0xA801,0xA815,0xA82B,0xA83F,
0xAA81,0xAA95,0xAAAB,0xAABF,
0xAD41,0xAD55,0xAD6B,0xAD7F,
0xAFC1,0xAFD5,0xAFEB,0xAFFF,

0xF801,0xF815,0xF82B,0xF83F,
0xFA81,0xFA95,0xFAAB,0xFABF,
0xFD41,0xFD55,0xFD6B,0xFD7F,
0xFFC1,0xFFD5,0xFFEB,0xFFFF
};

FLTexture flPalette[FL_PALETTE_MAX];
FLTexture flTexture[FL_TEXTURE_MAX];
FLPS2State flPs2State;

FL_FMS flFMS;

s32 flFrame;

int debug_mode = 0;

void enableDebug(){
    if(debug_mode == 0){
        pspDebugScreenInit();
        debug_mode = 1;
    }
}

s32 flLogOut(s8* format, ...){
    if(DEMMA_DEBUG){
        char buffer[LOG_BUFFER_SIZE];

        va_list args;
        va_start(args, format);

        vsnprintf(buffer, sizeof(buffer), format, args);

        va_end(args);

        enableDebug();
        pspDebugScreenPrintf("%s", buffer);

        flPrintColor(0xFFFFFFFF);
    }
}

s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...) {
    if(DEMMA_DEBUG){
        char buffer[LOG_BUFFER_SIZE];

        va_list args;
        va_start(args, format);

        vsnprintf(buffer, sizeof(buffer), format, args);

        va_end(args);

        enableDebug();
        pspDebugScreenSetXY(posi_x, posi_y);
        pspDebugScreenPrintf("%s", buffer);
    }
}

s32 flPrintColor(u32 col){
    if(DEMMA_DEBUG){
        enableDebug();
        pspDebugScreenSetTextColor(col);
    }
}

s32 flFlip(u32 flag) {
    flFrame++;

    //flLogOut("flFrame %d\n", flFrame);

    return 1;
}

u32 flPS2GetTextureSize(u32 format, s32 dw, s32 dh, s32 bnum);
s32 flPS2LockTexture(Rect* /* unused */, FLTexture* lpflTexture, plContext* lpcontext, u32 flag, s32 /* unused */);
s32 flPS2UnlockTexture(FLTexture*);

u32 flCreateTextureHandle(plContext* bits, u32 flag) {
    FLTexture* lpflTexture;
    u32 th = flPS2GetTextureHandle();

    if (th == 0) {
        return 0;
    }

    lpflTexture = &flTexture[LO_16_BITS(th) - 1];
    flPS2GetTextureInfoFromContext(bits, 1, th, flag);

    if (bits->ptr == NULL) {
        lpflTexture->mem_handle = flPS2GetSystemMemoryHandle(lpflTexture->size, 0);
    } else {
        flPS2ConvertTextureFromContext(bits, lpflTexture, 0);
        flPS2CreateTextureHandle(th, flag);
    }

    flLogOut("create tex texdata %x %d %x\n", lpflTexture->wkVram, th, lpflTexture);

    return th;
}

s32 flPS2GetTextureInfoFromContext(plContext* bits, s32 bnum, u32 th, u32 flag) {
    FLTexture* lpflTexture;
    s32 lp0;
    s32 dw;
    s32 dh;
    plContext* lpcon;

    lpflTexture = &flTexture[LO_16_BITS(th) - 1];

    if (bnum > 1) {
        if (bnum > 7) {
            flLogOut("Not supported mipmap texture @flPS2GetTextureInfoFromContext");
            assert(0);
            return 0;
        }

        lpcon = bits + 1;
        dw = bits->width;
        dh = bits->height;

        for (lp0 = 1; lp0 < bnum; lp0++) {
            dw >>= 1;
            dh >>= 1;

            if ((lpcon->width != dw) || (lpcon->height != dh)) {
                flLogOut("Not supported mipmap texture @flPS2GetTextureInfoFromContext");
                assert(0);
                return 0;
            }

            lpcon += 1;
        }
    }

    lpflTexture->be_flag = 1;
    lpflTexture->flag = flag;
    lpflTexture->desc = bits->desc;
    lpflTexture->width = bits->width;
    lpflTexture->height = bits->height;
    lpflTexture->mem_handle = 0;
    lpflTexture->lock_ptr = 0;
    lpflTexture->lock_flag = 0;
    lpflTexture->tex_num = bnum;

    switch (bits->bitdepth) {
    default:
        flLogOut("Not supported texture bit depth @flPS2GetTextureInfoFromContext");
        assert(0);
        return 0;

    case 0:
        lpflTexture->format = GU_PSM_T4;
        lpflTexture->bitdepth = 0;
        break;

    case 1:
        lpflTexture->format = GU_PSM_T8;
        lpflTexture->bitdepth = 1;
        break;

    case 2:
        lpflTexture->format = GU_PSM_5551;
        lpflTexture->bitdepth = 2;
        break;

    case 3:
        lpflTexture->format = GU_PSM_4444;
        lpflTexture->bitdepth = 2;
        break;

    case 4:
        lpflTexture->format = GU_PSM_8888;
        lpflTexture->bitdepth = 4;
        break;
    }

    switch (bits->width) {
    case 1024:
    case 512:
    case 256:
    case 128:
    case 64:
    case 32:
        break;

    default:
        flLogOut("Not supported width...%d @flPS2GetTextureInfoFromContext", bits->width);
        return 0;
    }

    switch (bits->height) {
    case 1024:
    case 512:
    case 256:
    case 128:
    case 64:
    case 32:
        break;

    default:
        flLogOut("Not supported height...%d @flPS2GetTextureInfoFromContext", bits->height);
        return 0;
    }

    lpflTexture->size =
        flPS2GetTextureSize(lpflTexture->format, lpflTexture->width, lpflTexture->height, lpflTexture->tex_num);
    return 1;
}

s32 flPS2CreateTextureHandle(u32 th, u32 flag) {
    
    return 1;
}

u32 flPS2GetTextureHandle() {
    s32 i;

    for (i = 0; i < FL_TEXTURE_MAX; i++) {
        if (!flTexture[i].be_flag) {
            break;
        }
    }

    if (i == FL_TEXTURE_MAX) {
        flPrintColor(0xFFFF0000);
        flLogOut("ERROR flPS2GetTextureHandle flps2vram.c\n");
    }

    flLogOut("flPS2GetTextureHandle %d\n", i);

    return i + 1;
}

u32 flCreatePaletteHandle(plContext* lpcontext, u32 flag) {
    FLTexture* lpflPalette;
    u32 ph = flPS2GetPaletteHandle();

    flLogOut("flCreatePaletteHandle start\n");

    if (ph == 0) {
        return 0;
    }

    lpflPalette = &flPalette[HI_16_BITS(ph) - 1];
    flPS2GetPaletteInfoFromContext(lpcontext, ph, flag);

    flLogOut("flCreatePaletteHandle 0\n");
    lpflPalette->mem_handle = flPS2GetSystemMemoryHandle(lpflPalette->size, 2);
    //lpflPalette->wkVram = memalign(16, lpflPalette->size);
    flLogOut("flCreatePaletteHandle 1\n");

    if (lpcontext->ptr != NULL) {
        u16* src = (u16*) lpcontext->ptr;
        u16* dest = (u16*) flPS2GetSystemBuffAdrs(lpflPalette->mem_handle);
        //u16* dest = (u16*) lpflPalette->wkVram;
        
        for(int i = 0; i < lpflPalette->size; i++){
                dest[i] = src[i] & 0x83D0;
                dest[i] |= (src[i] & 0x001F) << 10;
                dest[i] |= (src[i] & 0x7B00) >> 10;
        }

        flPS2CreatePaletteHandle(ph, flag);
    }

    flLogOut("flCreatePaletteHandle end %d\n", ph >> 16);

    return ph >> 16;
}

s32 flPS2GetPaletteInfoFromContext(plContext* bits, u32 ph, u32 flag) {
    FLTexture* lpflPalette = &flPalette[((ph & 0xFFFF0000) >> 0x10) - 1];

    if (bits->height != 1) {
        flLogOut("Supported only 1 palette. Unallocatable. @flCreatePaletteHandle");
        return 0;
    }

    switch (bits->bitdepth) {
    default:
        flLogOut("Not supported texture bit depth @flCreatePaletteHandle");
        return 0;

    case 2:
        lpflPalette->format = 2;
        lpflPalette->bitdepth = 2;
        break;

    case 3:
        lpflPalette->format = 1;
        lpflPalette->bitdepth = 3;
        break;

    case 4:
        lpflPalette->format = 0;
        lpflPalette->bitdepth = 4;
        break;
    }

    if (bits->width == 256) {
        lpflPalette->width = 16;
        lpflPalette->height = 16;
    } else {
        lpflPalette->width = 8;
        lpflPalette->height = 2;
    }

    lpflPalette->desc = bits->desc;
    lpflPalette->flag = flag;
    lpflPalette->be_flag = 1;
    lpflPalette->mem_handle = 0;
    lpflPalette->lock_ptr = 0;
    lpflPalette->lock_flag = 0;
    lpflPalette->tex_num = 1;
    lpflPalette->size = flPS2GetTextureSize(lpflPalette->format, lpflPalette->width, lpflPalette->height, lpflPalette->tex_num);
    return 1;
}

s32 flPS2CreatePaletteHandle(u32 ph, u32 flag) {
    return 1;
}

u32 flPS2GetPaletteHandle() {
    s32 i;

    for (i = 0; i < FL_PALETTE_MAX; i++) {
        if (!flPalette[i].be_flag) {
            break;
        }
    }

    if (i == FL_PALETTE_MAX) {
        flPrintColor(0xFFFF0000);
        flLogOut("ERROR flPS2GetPaletteHandle flps2vram.c\n");
    }

    return (i + 1) << 16;
}

s32 flReleaseTextureHandle(u32 texture_handle) {
    FLTexture* lpflTexture = &flTexture[texture_handle - 1];

    flLogOut("flReleaseTextureHandle\n");

    if ((texture_handle == 0) || (texture_handle > FL_TEXTURE_MAX) || (lpflTexture->be_flag == 0)) {
        flPrintColor(0xFFFF0000);
        flLogOut("ERROR flReleaseTextureHandle flps2vram.c\n");
    }

    if (lpflTexture->wkVram != NULL) {
        free(lpflTexture->wkVram);
        lpflTexture->wkVram = NULL;
    }

    if(lpflTexture->mem_handle != 0){
        flPS2ReleaseSystemMemory(lpflTexture->mem_handle);
        lpflTexture->mem_handle = 0;
    }

    flMemset(lpflTexture, 0, sizeof(FLTexture));
    return 1;
}

s32 flReleasePaletteHandle(u32 palette_handle) {
    FLTexture* lpflPalette = &flPalette[palette_handle - 1];

    if ((palette_handle == 0) || (palette_handle > FL_PALETTE_MAX) || (lpflPalette->be_flag == 0)) {
        flPrintColor(0xFFFF0000);
        flLogOut("ERROR flReleasePaletteHandle flps2vram.c\n");
    }

    if (lpflPalette->wkVram != NULL) {
        free(lpflPalette->wkVram);
        lpflPalette->wkVram = NULL;
    }

    if (lpflPalette->mem_handle != 0) {
        flPS2ReleaseSystemMemory(lpflPalette->mem_handle);
    }

    flMemset(lpflPalette, 0, sizeof(FLTexture));
    return 1;
}

s32 flLockTexture(Rect* lprect, u32 th, plContext* lpcontext, u32 flag) {
    FLTexture* lpflTexture = &flTexture[th - 1];

    if (th > FL_TEXTURE_MAX) {
        return 0;
    }

    if (!lpflTexture->be_flag) {
        return 0;
    }

    return flPS2LockTexture(lprect, lpflTexture, lpcontext, flag, 0);
}

s32 flLockPalette(Rect* lprect, u32 th, plContext* lpcontext, u32 flag) {
    FLTexture* lpflPalette = &flPalette[th - 1];

    if (th > FL_PALETTE_MAX) {
        return 0;
    }

    if (!lpflPalette->be_flag) {
        return 0;
    }

    if (flPS2LockTexture(lprect, lpflPalette, lpcontext, flag, 1) == 0) {
        return 0;
    }

    if ((lpflPalette->width == 16) && (lpflPalette->height == 16)) {
        lpcontext->width = 256;
        lpcontext->height = 1;
    } else {
        lpcontext->width = 16;
        lpcontext->height = 1;
    }

    lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
    return 1;
}

s32 flPS2LockTexture(Rect* /* unused */, FLTexture* lpflTexture, plContext* lpcontext, u32 flag, s32 /* unused */) {
    u8* buff_ptr;
    u8* buff_ptr1;
    plContext src;

    lpflTexture->lock_flag = flag;
    lpcontext->desc = lpflTexture->desc;
    lpcontext->width = lpflTexture->width;
    lpcontext->height = lpflTexture->height;

    switch (flag & 3) {
    case 0:
        if (lpflTexture->mem_handle == 0) {
            buff_ptr1 = mflTemporaryUse(lpflTexture->size * 2);
            buff_ptr = buff_ptr1 + lpflTexture->size;
            // Loading an image from VRAM used to be here
        } else {
            buff_ptr = mflTemporaryUse(lpflTexture->size);
            buff_ptr1 = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
        }

        lpflTexture->lock_ptr = (uintptr_t)buff_ptr;
        lpcontext->ptr = buff_ptr;
        src.desc = lpcontext->desc;
        src.width = lpcontext->width;
        src.height = lpcontext->height;
        src.ptr = buff_ptr1;

        switch (lpflTexture->format) {
        case 20:
            lpcontext->bitdepth = 0;
            lpcontext->pitch = lpcontext->width / 2;
            flMemcpy(buff_ptr, buff_ptr1, lpflTexture->size);
            break;

        case 19:
            lpcontext->bitdepth = 1;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            flMemcpy(buff_ptr, buff_ptr1, lpflTexture->size);
            break;

        case 2:
            lpcontext->bitdepth = 2;
            lpcontext->pixelformat.rl = 5;
            lpcontext->pixelformat.rs = 0xA;
            lpcontext->pixelformat.rm = 0x1F;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gs = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pixelformat.bl = 5;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0x1F;
            lpcontext->pixelformat.al = 1;
            lpcontext->pixelformat.as = 0xF;
            lpcontext->pixelformat.am = 1;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            src.bitdepth = 2;
            src.pixelformat.rl = 5;
            src.pixelformat.rs = 0xA;
            src.pixelformat.rm = 0x1F;
            src.pixelformat.gl = 5;
            src.pixelformat.gs = 5;
            src.pixelformat.gm = 0x1F;
            src.pixelformat.bl = 5;
            src.pixelformat.bs = 0;
            src.pixelformat.bm = 0x1F;
            src.pixelformat.al = 1;
            src.pixelformat.as = 0xF;
            src.pixelformat.am = 1;
            src.pixelformat.rs = 0;
            src.pixelformat.bs = 0xA;
            src.pixelformat.gl = 5;
            src.pixelformat.gm = 0x1F;
            src.pitch = src.width * src.bitdepth;
            plConvertContext(lpcontext, &src);
            break;

        case 1:
            lpcontext->bitdepth = 3;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 0;
            lpcontext->pixelformat.as = 0;
            lpcontext->pixelformat.am = 0;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            src.bitdepth = 3;
            src.pixelformat.rl = 8;
            src.pixelformat.rs = 0x10;
            src.pixelformat.rm = 0xFF;
            src.pixelformat.gl = 8;
            src.pixelformat.gs = 8;
            src.pixelformat.gm = 0xFF;
            src.pixelformat.bl = 8;
            src.pixelformat.bs = 0;
            src.pixelformat.bm = 0xFF;
            src.pixelformat.al = 0;
            src.pixelformat.as = 0;
            src.pixelformat.am = 0;
            src.pixelformat.rs = 0;
            src.pixelformat.bs = 0x10;
            src.pitch = src.width * src.bitdepth;
            plConvertContext(lpcontext, &src);
            break;

        case 0:
            lpcontext->bitdepth = 4;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 8;
            lpcontext->pixelformat.as = 0x18;
            lpcontext->pixelformat.am = 0xFF;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            src.bitdepth = 4;
            src.pixelformat.rl = 8;
            src.pixelformat.rs = 0x10;
            src.pixelformat.rm = 0xFF;
            src.pixelformat.gl = 8;
            src.pixelformat.gs = 8;
            src.pixelformat.gm = 0xFF;
            src.pixelformat.bl = 8;
            src.pixelformat.bs = 0;
            src.pixelformat.bm = 0xFF;
            src.pixelformat.al = 8;
            src.pixelformat.as = 0x18;
            src.pixelformat.am = 0xFF;
            src.pixelformat.rs = 0;
            src.pixelformat.bs = 0x10;
            src.pitch = src.width * src.bitdepth;
            plConvertContext(lpcontext, &src);
            break;
        }

        break;

    case 1:
        buff_ptr = mflTemporaryUse(lpflTexture->size);

        if (lpflTexture->mem_handle == 0) {
            // Loading an image from VRAM used to be here
        } else {
            buff_ptr1 = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
            flMemcpy(buff_ptr, buff_ptr1, lpflTexture->size);
        }

        lpflTexture->lock_ptr = (uintptr_t)buff_ptr;
        lpcontext->ptr = buff_ptr;

        switch (lpflTexture->format) {
        case 20:
            lpcontext->bitdepth = 0;
            lpcontext->pitch = lpcontext->width / 2;
            break;

        case 19:
            lpcontext->bitdepth = 1;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 2:
            lpcontext->bitdepth = 2;
            lpcontext->pixelformat.rl = 5;
            lpcontext->pixelformat.rs = 0xA;
            lpcontext->pixelformat.rm = 0x1F;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gs = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pixelformat.bl = 5;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0x1F;
            lpcontext->pixelformat.al = 1;
            lpcontext->pixelformat.as = 0xF;
            lpcontext->pixelformat.am = 1;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0xA;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 1:
            lpcontext->bitdepth = 3;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 0;
            lpcontext->pixelformat.as = 0;
            lpcontext->pixelformat.am = 0;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0x10;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 0:
            lpcontext->bitdepth = 4;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 8;
            lpcontext->pixelformat.as = 0x18;
            lpcontext->pixelformat.am = 0xFF;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0x10;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;
        }

        break;

    case 2:
        if (lpflTexture->mem_handle == 0) {
            buff_ptr = mflTemporaryUse(lpflTexture->size);
        } else {
            buff_ptr = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
        }

        lpflTexture->lock_ptr = (uintptr_t)buff_ptr;
        lpcontext->ptr = buff_ptr;

        switch (lpflTexture->format) {
        case 20:
            lpcontext->bitdepth = 0;
            lpcontext->pitch = lpcontext->width / 2;
            break;

        case 19:
            lpcontext->bitdepth = 1;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 2:
            lpcontext->bitdepth = 2;
            lpcontext->pixelformat.rl = 5;
            lpcontext->pixelformat.rs = 0xA;
            lpcontext->pixelformat.rm = 0x1F;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gs = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pixelformat.bl = 5;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0x1F;
            lpcontext->pixelformat.al = 1;
            lpcontext->pixelformat.as = 0xF;
            lpcontext->pixelformat.am = 1;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0xA;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 1:
            lpcontext->bitdepth = 3;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 0;
            lpcontext->pixelformat.as = 0;
            lpcontext->pixelformat.am = 0;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0x10;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 0:
            lpcontext->bitdepth = 4;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 8;
            lpcontext->pixelformat.as = 0x18;
            lpcontext->pixelformat.am = 0xFF;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0x10;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;
        }

        break;

    case 3:
        if (lpflTexture->mem_handle == 0) {
            buff_ptr = mflTemporaryUse(lpflTexture->size);
        } else {
            buff_ptr = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
        }

        lpflTexture->lock_ptr = (uintptr_t)buff_ptr;
        lpcontext->ptr = buff_ptr;

        switch (lpflTexture->format) {
        case 20:
            lpcontext->bitdepth = 0;
            lpcontext->pitch = lpcontext->width / 2;
            break;

        case 19:
            lpcontext->bitdepth = 1;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 2:
            lpcontext->bitdepth = 2;
            lpcontext->pixelformat.rl = 5;
            lpcontext->pixelformat.rs = 0xA;
            lpcontext->pixelformat.rm = 0x1F;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gs = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pixelformat.bl = 5;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0x1F;
            lpcontext->pixelformat.al = 1;
            lpcontext->pixelformat.as = 0xF;
            lpcontext->pixelformat.am = 1;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0xA;
            lpcontext->pixelformat.gl = 5;
            lpcontext->pixelformat.gm = 0x1F;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 1:
            lpcontext->bitdepth = 3;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 0;
            lpcontext->pixelformat.as = 0;
            lpcontext->pixelformat.am = 0;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0x10;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;

        case 0:
            lpcontext->bitdepth = 4;
            lpcontext->pixelformat.rl = 8;
            lpcontext->pixelformat.rs = 0x10;
            lpcontext->pixelformat.rm = 0xFF;
            lpcontext->pixelformat.gl = 8;
            lpcontext->pixelformat.gs = 8;
            lpcontext->pixelformat.gm = 0xFF;
            lpcontext->pixelformat.bl = 8;
            lpcontext->pixelformat.bs = 0;
            lpcontext->pixelformat.bm = 0xFF;
            lpcontext->pixelformat.al = 8;
            lpcontext->pixelformat.as = 0x18;
            lpcontext->pixelformat.am = 0xFF;
            lpcontext->pixelformat.rs = 0;
            lpcontext->pixelformat.bs = 0x10;
            lpcontext->pitch = lpcontext->width * lpcontext->bitdepth;
            break;
        }

        break;
    }

    lpcontext->desc = lpcontext->desc | 2;
    return 1;
}

s32 flUnlockTexture(u32 th) {
    FLTexture* lpflTexture = &flTexture[th - 1];

    if (th > FL_TEXTURE_MAX) {
        return 0;
    }

    if (!lpflTexture->be_flag) {
        return 0;
    }

    return flPS2UnlockTexture(lpflTexture);
}

s32 flUnlockPalette(u32 th) {
    FLTexture* lpflPalette = &flPalette[th - 1];

    if (th > FL_PALETTE_MAX) {
        return 0;
    }

    if (!lpflPalette->be_flag) {
        return 0;
    }

    return flPS2UnlockTexture(lpflPalette);
}

s32 flPS2UnlockTexture(FLTexture* lpflTexture) {
    u8* buff_ptr;
    u8* buff_ptr1;
    plContext src;
    plContext dst;

    switch (lpflTexture->lock_flag & 3) {
    case 0:
        if (lpflTexture->mem_handle != 0) {
            buff_ptr = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
            buff_ptr1 = (u8*)lpflTexture->lock_ptr;
        } else {
            buff_ptr = mflTemporaryUse(lpflTexture->size);
            buff_ptr1 = (u8*)lpflTexture->lock_ptr;
        }

        src.desc = lpflTexture->desc;
        src.width = lpflTexture->width;
        src.height = lpflTexture->height;
        src.ptr = buff_ptr1;
        dst.desc = lpflTexture->desc;
        dst.width = lpflTexture->width;
        dst.height = lpflTexture->height;
        dst.ptr = buff_ptr;

        switch (lpflTexture->format) {
        case 20:
        case 19:
            flMemcpy(buff_ptr, buff_ptr1, lpflTexture->size);
            break;

        case 2:
            src.bitdepth = 2;
            src.pixelformat.rl = 5;
            src.pixelformat.rs = 0xA;
            src.pixelformat.rm = 0x1F;
            src.pixelformat.gl = 5;
            src.pixelformat.gs = 5;
            src.pixelformat.gm = 0x1F;
            src.pixelformat.bl = 5;
            src.pixelformat.bs = 0;
            src.pixelformat.bm = 0x1F;
            src.pixelformat.al = 1;
            src.pixelformat.as = 0xF;
            src.pixelformat.am = 1;
            src.pitch = src.width * src.bitdepth;
            dst.bitdepth = 2;
            dst.pixelformat.rl = 5;
            dst.pixelformat.rs = 0xA;
            dst.pixelformat.rm = 0x1F;
            dst.pixelformat.gl = 5;
            dst.pixelformat.gs = 5;
            dst.pixelformat.gm = 0x1F;
            dst.pixelformat.bl = 5;
            dst.pixelformat.bs = 0;
            dst.pixelformat.bm = 0x1F;
            dst.pixelformat.al = 1;
            dst.pixelformat.as = 0xF;
            dst.pixelformat.am = 1;
            dst.pixelformat.rs = 0;
            dst.pixelformat.bs = 0xA;
            dst.pixelformat.gl = 5;
            dst.pixelformat.gm = 0x1F;
            dst.pitch = dst.width * dst.bitdepth;
            plConvertContext(&dst, &src);
            break;

        case 1:
            src.bitdepth = 3;
            src.pixelformat.rl = 8;
            src.pixelformat.rs = 0x10;
            src.pixelformat.rm = 0xFF;
            src.pixelformat.gl = 8;
            src.pixelformat.gs = 8;
            src.pixelformat.gm = 0xFF;
            src.pixelformat.bl = 8;
            src.pixelformat.bs = 0;
            src.pixelformat.bm = 0xFF;
            src.pixelformat.al = 0;
            src.pixelformat.as = 0;
            src.pixelformat.am = 0;
            src.pitch = src.width * src.bitdepth;
            dst.bitdepth = 3;
            dst.pixelformat.rl = 8;
            dst.pixelformat.rs = 0x10;
            dst.pixelformat.rm = 0xFF;
            dst.pixelformat.gl = 8;
            dst.pixelformat.gs = 8;
            dst.pixelformat.gm = 0xFF;
            dst.pixelformat.bl = 8;
            dst.pixelformat.bs = 0;
            dst.pixelformat.bm = 0xFF;
            dst.pixelformat.al = 0;
            dst.pixelformat.as = 0;
            dst.pixelformat.am = 0;
            dst.pixelformat.rs = 0;
            dst.pixelformat.bs = 0x10;
            dst.pitch = dst.width * dst.bitdepth;
            plConvertContext(&dst, &src);
            break;

        case 0:
            src.bitdepth = 4;
            src.pixelformat.rl = 8;
            src.pixelformat.rs = 0x10;
            src.pixelformat.rm = 0xFF;
            src.pixelformat.gl = 8;
            src.pixelformat.gs = 8;
            src.pixelformat.gm = 0xFF;
            src.pixelformat.bl = 8;
            src.pixelformat.bs = 0;
            src.pixelformat.bm = 0xFF;
            src.pixelformat.al = 8;
            src.pixelformat.as = 0x18;
            src.pixelformat.am = 0xFF;
            src.pitch = src.width * src.bitdepth;
            dst.bitdepth = 4;
            dst.pixelformat.rl = 8;
            dst.pixelformat.rs = 0x10;
            dst.pixelformat.rm = 0xFF;
            dst.pixelformat.gl = 8;
            dst.pixelformat.gs = 8;
            dst.pixelformat.gm = 0xFF;
            dst.pixelformat.bl = 8;
            dst.pixelformat.bs = 0;
            dst.pixelformat.bm = 0xFF;
            dst.pixelformat.al = 8;
            dst.pixelformat.as = 0x18;
            dst.pixelformat.am = 0xFF;
            dst.pixelformat.rs = 0;
            dst.pixelformat.bs = 0x10;
            dst.pitch = dst.width * dst.bitdepth;
            plConvertContext(&dst, &src);
            break;
        }

        break;

    case 1:
        if (lpflTexture->mem_handle != 0) {
            buff_ptr = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
            buff_ptr1 = (u8*)lpflTexture->lock_ptr;
            flMemcpy(buff_ptr, buff_ptr1, lpflTexture->size);
        } else {
            buff_ptr = (u8*)lpflTexture->lock_ptr;
        }

        break;

    case 2:
    case 3:
        break;
    }

    lpflTexture->desc &= ~2;

    return 1;
}

u32 flPS2GetTextureSize(u32 format, s32 dw, s32 dh, s32 bnum) {
    u32 tex_size;
    s32 lp0;

    tex_size = 0;

    for (lp0 = 0; lp0 < bnum; lp0++) {
        switch (format) {
        case 0:
        case 1:
            tex_size += dw * dh * 4;
            break;

        case 2:
        case 10:
            tex_size += dw * dh * 2;
            break;

        case 19:
            tex_size += dw * dh;
            break;

        case 20:
            tex_size += (dw * dh) >> 1;
            break;
        }

        dw >>= 1;
        dh >>= 1;
    }

    return tex_size;
}


s32 flInitialize(s32 /* unused */, s32 /* unused */){
    if (system_work_init() == 0) {
        return 0;
    }

    flPS2SystemTmpBuffInit();
    flPADInitialize();

    return 1;
}

void flSetTexture(int th){
    int texture_handle = LO_16_BITS(th) - 1;
    FLTexture *flTex = &flTexture[texture_handle];
    int palette_handle = HI_16_BITS(th) - 1;
    FLTexture *flPal = &flPalette[palette_handle];
    u16 *pal = flPal->wkVram;

    void *texData = flTex->wkVram;
    
    if(texData == NULL)
        texData = flPS2GetSystemBuffAdrs(flTex->mem_handle);

    if(pal == NULL)
        pal = flPS2GetSystemBuffAdrs(flPal->mem_handle);

    
    sceGuClutMode(GU_PSM_5551, 0, 0xFF, 0);
    sceGuClutLoad(flPal->size / 8, pal);

    if(currentTexture != texData){
        sceGuTexMode(flTex->format, 0, 0, GU_FALSE);
        sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
        void *p = (void*) 0x4000;
        sceGuTexImage(0, flTex->width, flTex->height, flTex->width, texData);
        //sceGuTexImage(0, flTex->width, flTex->height, flTex->width, p);
        currentTexture = texData;
    }
}

s32 flSetRenderState(enum _FLSETRENDERSTATE func, u32 value) {
    switch (func) {
    case FLRENDER_TEXSTAGE0:
    case FLRENDER_TEXSTAGE1:
    case FLRENDER_TEXSTAGE2:
    case FLRENDER_TEXSTAGE3:

        if (func == FLRENDER_TEXSTAGE0) {
            flSetTexture(value);
        }

        break;

    case FLRENDER_BACKCOLOR:
        setBackGroundColor(value | 0xFF000000);
        break;

    default:
        break;
    }

    return 1;
}

static s32 system_work_init() {
    void* temp;

    flMemset(&flPs2State, 0, sizeof(FLPS2State));
    int temp_size = 0x01C00000;
    //int temp_size = 0x01C00000;
    temp = malloc(temp_size);

    if (temp == NULL) {
        flLogOut("system_work_init malloc failed\n");
        while(1);
        return 0;
    }

    fmsInitialize(&flFMS, temp, temp_size, 0x16);
    //const int system_memory_size = 0xA00000;
    const int system_memory_size = 0x1000000;
    temp = flAllocMemoryS(system_memory_size);
    mflInit(temp, system_memory_size, 0x16);

    return 1;
}

s32 flPS2ConvertTextureFromContext(plContext* lpcontext, FLTexture* lpflTexture, u32 type) {
    //lpflTexture->mem_handle = flPS2GetSystemMemoryHandle(lpflTexture->size, 2);
    lpflTexture->mem_handle = (u32)lpcontext->ptr;
    lpflTexture->wkVram = NULL;

    //memcpy(lpflTexture->wkVram, lpcontext->ptr, lpflTexture->size);

    u8 *dst_ptr = flPS2GetSystemBuffAdrs(lpflTexture->mem_handle);
    s32 tex_size;
    s32 dw = lpflTexture->width;
    s32 dh = lpflTexture->height;
    s32 lp0;

    u16 color;
    u16 *p_color_16;

    for (lp0 = 0; lp0 < lpflTexture->tex_num; lp0++) {

            switch (lpflTexture->format) {
            default:
                flLogOut("Not supported texture bit depth @flPS2ConvertTextureFromContext");
                break;

            case GU_PSM_T4:
                tex_size = (dw * dh) >> 1;
                break;
            case GU_PSM_T8:
                tex_size = dw * dh;
                break;

            case GU_PSM_5551:
                tex_size = dw * dh * 2;
                p_color_16 = (u16*) dst_ptr;
                for(int i = 0; i < dw * dh; i++){
                    color = p_color_16[i] & 0x8000;
                    color |= p_color_16[i] & 0x03E0;
                    color |= (p_color_16[i] & 0x7C00) >> 10;
                    color |= (p_color_16[i] & 0x001F) << 10;
                    p_color_16[i] = color;
                }
                break;

            case GU_PSM_4444:
                tex_size = dw * dh * 2;
                p_color_16 = (u16*) dst_ptr;
                for(int i = 0; i < dw * dh; i++){
                    color = p_color_16[i] & 0x8000;
                    color |= p_color_16[i] & 0x03E0;
                    color |= (p_color_16[i] & 0x7C00) >> 10;
                    color |= (p_color_16[i] & 0x001F) << 10;
                    p_color_16[i] = color;
                }
                break;

            case GU_PSM_8888:
                tex_size = dw * dh * 4;
                break;
            }

            dst_ptr = &dst_ptr[tex_size];
            dw >>= 1;
            dh >>= 1;
            lpcontext++;
        }

    return 1;
}