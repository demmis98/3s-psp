#ifndef FL_H_   // include guard
#define FL_H_

#include "types.h"

#include "common/graphics.h"

#define LOG_BUFFER_SIZE 512

#define DEMMA_DEBUG 0

s32 flLogOut(s8* format, ...);
s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...);
s32 flPrintColor(u32 col);

s32 flFlip(u32 flag);

#endif  // FL_H_