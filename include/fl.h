#ifndef FL_H_   // include guard
#define FL_H_

#include "types.h"

s32 flLogOut(s8* format, ...);
s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...);
s32 flPrintColor(u32 col);

#endif  // FL_H_