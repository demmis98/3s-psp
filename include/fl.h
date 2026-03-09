#ifndef FL_H_   // include guard
#define FL_H_

#include "structs.h"
#include "types.h"

#include "common/graphics.h"
#include "psp/flps2etc.h"

#define LOG_BUFFER_SIZE 512

extern int DEMMA_DEBUG;
#define DEMMA_LOOPS 3600

#include "Game/color3rd.h"
#include "psp/PPGFile.h"

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
s32 flInitialize(s32 /* unused */, s32 /* unused */);
static s32 system_work_init();

// from modern port

#define FL_PALETTE_MAX 1088
#define FL_TEXTURE_MAX MAX_TEXTURES

extern u32 flDebugStrCtr;
extern u32 flDebugStrCol;
extern u32 flDebugStrHan;
extern s32 flVramStaticNum;
extern FL_FMS flFMS;
extern u32 flSystemRenderOperation;
extern s32 flHeight;
extern s32 flWidth;
extern FLTexture flPalette[FL_PALETTE_MAX];
extern FLTexture flTexture[FL_TEXTURE_MAX];
extern FLPS2State flPs2State;

u32 flPS2GetPaletteHandle();
s32 flPS2CreatePaletteHandle(u32 ph, u32 flag);
s32 flPS2GetPaletteInfoFromContext(plContext* bits, u32 ph, u32 flag);
u32 flPS2GetTextureHandle();
s32 flPS2CreateTextureHandle(u32 th, u32 flag);
s32 flPS2GetTextureInfoFromContext(plContext* bits, s32 bnum, u32 th, u32 flag);
s32 flPS2ConvertTextureFromContext(plContext* lpcontext, FLTexture* lpflTexture, u32 type);

#endif  // FL_H_