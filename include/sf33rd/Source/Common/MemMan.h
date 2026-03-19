/**
 * @file MemMan.h
 * @brief Doubly-linked-list heap memory manager.
 *
 * Provides a best-fit allocator that supports both forward and reverse
 * allocation within a pre-allocated memory region. Used by the PPG
 * subsystem and the zlib decompression wrapper.
 *
 * Part of the Common module.
 * Originally from the PS2 memory management module.
 */
#ifndef MEMMAN_H
#define MEMMAN_H

#include "structs.h"
#include "types.h"

void mmSystemInitialize();
void mmHeapInitialize(_MEMMAN_OBJ* mmobj, u8* adrs, s32 size, s32 unit, s8* format);
uintptr_t mmRoundUp(s32 unit, uintptr_t num);
uintptr_t mmRoundOff(s32 unit, uintptr_t num);
void mmDebWriteTag(s8* /* unused */);
ssize_t mmGetRemainder(_MEMMAN_OBJ* mmobj);
ssize_t mmGetRemainderMin(_MEMMAN_OBJ* mmobj);
u8* mmAlloc(_MEMMAN_OBJ* mmobj, ssize_t size, s32 flag);
struct _MEMMAN_CELL* mmAllocSub(_MEMMAN_OBJ* mmobj, ssize_t size, s32 flag);
void mmFree(_MEMMAN_OBJ* mmobj, u8* adrs);

#endif
