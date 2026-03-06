#include "fl.h"

#include <stdarg.h>
#include <stdio.h>
#include <assert.h>

#include <pspdebug.h>

#include "Game/color3rd.h"
#include "psp/PPGFile.h"

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
        flPrintColor(0xFFFFFFFF);
        pspDebugScreenPrintf("%s", buffer);
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

    if(DEMMA_DEBUG)
        flLogOut("flFrame %d\n", flFrame);

    return 1;
}

s32 flLockTexture(Rect* lprect, u32 th, plContext* lpcontext, u32 flag) {}
s32 flUnlockTexture(u32 th){}

u32 flCreateTextureHandle(plContext* bits, u32 flag) {
    int id;

    for(id = 0; (texturesPSPUsed[id] && (id < MAX_TEXTURES)); id++);

    if (id == MAX_TEXTURES)
        return -1;

    return flSetTextureHandle(bits, id, flag);
}

u32 flSetTextureHandle(plContext* bits, s32 id, u32 flag) {
    if(texturesPSPUsed[id])
        free(texturesPSP[id].data);

    texturesPSP[id].data = bits->ptr;

    texturesPSP[id].width  = bits->width;
    texturesPSP[id].height = bits->height;
    texturesPSP[id].wRender = bits->width;
    texturesPSP[id].hRender = bits->height;

    
    switch(bits->bitdepth) {
        case 0:  texturesPSP[id].mode = GU_PSM_T4;    break;
        case 1:  texturesPSP[id].mode = GU_PSM_T8;    break;
        case 2:  texturesPSP[id].mode = GU_PSM_5551;  break;
        case 3:
        case 4:  texturesPSP[id].mode = GU_PSM_8888;  break;
        default:
            texturesPSPUsed[id] = false;
            return 0;
    }
    
    texturesPSPUsed[id] = true;
    
    return id;
}

s32 flReleaseTextureHandle(u32 texture_handle){
    free(texturesPSP[texture_handle].data);
    texturesPSP[texture_handle].data = NULL;
    texturesPSPUsed[texture_handle] = false;
    return 1;
}

u32 flCreatePaletteHandle(plContext* ctx, u32 flag) {
    int id;
    u16* src = ctx->ptr;

    for(id = 0; ColorRAMUsed[id] && id < MAX_PALETTES; id++);

    if (id == MAX_PALETTES)
        return -1;



    //memcpy(&ColorRAM[id], ctx->ptr, ctx->width * ctx->bitdepth);

    for(int i = 0; i < 64; i++){
        ColorRAM[id][i] = src[i] & 0x83D0;
        ColorRAM[id][i] += (src[i] & 0x001F) << 10;
        ColorRAM[id][i] += (src[i] & 0x7B00) >> 10;
    }

    ColorRAMUsed[id] = true;

    return id;
}

s32 flReleasePaletteHandle(u32 palette_handle){
    ColorRAMUsed[palette_handle] = false;
    return 1;
}

s32 flLockPalette(Rect* lprect, u32 th, plContext* lpcontext, u32 flag){}
s32 flUnlockPalette(u32 th){}
s32 flSetRenderState(enum _FLSETRENDERSTATE func, u32 value){}