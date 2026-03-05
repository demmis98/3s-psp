#include "fl.h"

#include <stdarg.h>
#include <stdio.h>

#include <pspdebug.h>

#include "Game/color3rd.h"

s32 flFrame;

int debug_mode = 0;

void enableDebug(){
    if(debug_mode == 0){
        pspDebugScreenInit();
        debug_mode = 1;
    }
}

s32 flLogOut(s8* format, ...){
    char buffer[LOG_BUFFER_SIZE];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    enableDebug();
    flPrintColor(0xFFFFFFFF);
    pspDebugScreenPrintf("%s", buffer);
}

s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...) {
    char buffer[LOG_BUFFER_SIZE];

    va_list args;
    va_start(args, format);

    vsnprintf(buffer, sizeof(buffer), format, args);

    va_end(args);

    enableDebug();
    pspDebugScreenSetXY(posi_x, posi_y);
    pspDebugScreenPrintf("%s", buffer);

}

s32 flPrintColor(u32 col){
    enableDebug();
    pspDebugScreenSetTextColor(col);
}

s32 flFlip(u32 flag) {
    flFrame++;

    if(DEMMA_DEBUG)
        flLogOut("flFrame %d\n", flFrame);

    return 1;
}

s32 flLockTexture(Rect* lprect, u32 th, plContext* lpcontext, u32 flag) {}
s32 flUnlockTexture(u32 th){}
u32 flCreateTextureHandle(plContext* bits, u32 flag){}
s32 flReleaseTextureHandle(u32 texture_handle){}

u32 flCreatePaletteHandle(plContext* ctx, u32 flag) {
    u16 *src = (u16*)ctx->ptr;
    for(int i = 0; i < 64; i++){
        ColorRAM[0][currentPaletteIndex] = src[i];
    }
}

s32 flReleasePaletteHandle(u32 palette_handle){}

s32 flLockPalette(Rect* lprect, u32 th, plContext* lpcontext, u32 flag){}
s32 flUnlockPalette(u32 th){}
s32 flSetRenderState(enum _FLSETRENDERSTATE func, u32 value){}