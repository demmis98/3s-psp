/**
 * @file memfound.h
 * @brief System memory facade — thin wrappers over the plmem manager.
 *
 * Provides the global system memory manager instance and convenience
 * functions (mflInit, mflRegister, mflRetrieve, etc.) that delegate
 * to the underlying plmem* handle-based memory manager.
 *
 * Part of the AcrSDK common module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef MEMFOUND_H
#define MEMFOUND_H

#include "sf33rd/AcrSDK/common/memmgr.h"
#include "structs.h"
#include "types.h"

extern MEM_BLOCK sysmemblock[4096];

void mflInit(void* mem_ptr, s32 memsize, s32 memalign);
u32 mflGetSpace();
size_t mflGetFreeSpace();
u32 mflRegisterS(s32 len);
u32 mflRegister(s32 len);
void* mflTemporaryUse(s32 len);
void* mflRetrieve(u32 handle);
s32 mflRelease(u32 handle);
void* mflCompact();

#endif
