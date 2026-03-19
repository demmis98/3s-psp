/**
 * @file flps2etc.h
 * @brief Memory wrappers, system memory handles, and temporary buffer management.
 *
 * Provides convenience wrappers around memset/memcpy, frame memory
 * stack allocation, system memory handle registration/retrieval, and
 * a double-buffered temporary buffer pool used by VRAM/texture code.
 *
 * Part of the AcrSDK ps2 module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef FLPS2ETC_H
#define FLPS2ETC_H

#include "structs.h"
#include "types.h"

void flMemset(void* dst, u32 pat, s32 size);
void flMemcpy(void* dst, void* src, s32 size);
void* flAllocMemory(s32 size);
s32 flGetFrame(FMS_FRAME* frame);
s32 flGetSpace();
void* flAllocMemoryS(s32 size);
u32 flPS2GetSystemMemoryHandle(s32 len, s32 type);
void flPS2ReleaseSystemMemory(u32 handle);
void* flPS2GetSystemBuffAdrs(u32 handle);
void flPS2SystemTmpBuffInit();
void flPS2SystemTmpBuffFlush();
uintptr_t flPS2GetSystemTmpBuff(s32 len, s32 align);

#endif // FLPS2ETC_H
