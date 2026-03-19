/**
 * @file prilay.h
 * @brief Priority layer utilities — memory, pixel drawing, color conversion.
 *
 * Provides low-level utility functions: custom memset/memmove, pixel
 * address calculation, pixel read/write in multiple bit depths and
 * pixel formats, and full-context color conversion between surfaces.
 *
 * Part of the AcrSDK common module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef PRILAY_H
#define PRILAY_H

#include "plcommon.h"
#include "structs.h"
#include "types.h"

s32 plReport(s8* format, ...);
void plMemset(void* dst, u32 pat, s32 size);
void plMemmove(void* dst, void* src, s32 size);
void* plCalcAddress(s32 x, s32 y, plContext* lpcontext);
s32 plDrawPixel(plContext* dst, Pixel* ptr);
s32 plDrawPixel_3(plContext* dst, s32 x, s32 y, u32 color);
u32 plGetColor(s32 x, s32 y, plContext* lpcontext);
s32 plConvertContext(plContext* dst, plContext* src);

#endif
