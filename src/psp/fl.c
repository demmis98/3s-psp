#include "fl.h"

#include <stdarg.h>
#include <stdio.h>

#include <pspdebug.h>

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