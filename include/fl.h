#ifndef FL_H_   // include guard
#define FL_H_

#include "structs.h"
#include "types.h"

#include "common/graphics.h"

#define LOG_BUFFER_SIZE 512

extern int DEMMA_DEBUG;
#define DEMMA_LOOPS 3600

#include "Game/color3rd.h"

s32 flLogOut(s8* format, ...);
s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...);
s32 flPrintColor(u32 col);

s32 flFlip(u32 flag);

// unimplemented
s32 flLockTexture(Rect* lprect, u32 th, plContext* lpcontext, u32 flag);
s32 flUnlockTexture(u32 th);
u32 flCreateTextureHandle(plContext* bits, u32 flag);
u32 flSetTextureHandle(plContext* bits, s32 id, u32 flag);
s32 flReleaseTextureHandle(u32 texture_handle);
u32 flCreatePaletteHandle(plContext* lpcontext, u32 flag);
s32 flReleasePaletteHandle(u32 palette_handle);
s32 flLockPalette(Rect* lprect, u32 th, plContext* lpcontext, u32 flag);
s32 flUnlockPalette(u32 th);
s32 flSetRenderState(enum _FLSETRENDERSTATE func, u32 value);

// memory management
void* flAllocMemory(s32 size);
s32 flGetFrame(FMS_FRAME* frame);
s32 flGetSpace();
void* flAllocMemoryS(s32 size);
s32 flInitialize(s32 /* unused */, s32 /* unused */);
s32 system_work_init();

#endif  // FL_H_