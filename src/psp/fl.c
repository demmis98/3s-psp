#include "fl.h"

#include <stdarg.h>

#include <pspdebug.h>

int debug_mode = 0;

void enableDebug(){
    if(debug_mode == 0){
        pspDebugScreenInit();
        debug_mode = 1;
    }
}

s32 flLogOut(s8* format, ...){
    va_list args;
    va_start(args, format);

    enableDebug();
    pspDebugScreenPrintf(format, args);
}

s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...) {
    va_list args;
    va_start(args, format);

    enableDebug();
    pspDebugScreenSetXY(posi_x, posi_y);
    pspDebugScreenPrintf(format, args);
}

s32 flPrintColor(u32 col){
    enableDebug();
    pspDebugScreenSetTextColor(col);
}
