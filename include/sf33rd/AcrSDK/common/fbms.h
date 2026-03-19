/**
 * @file fbms.h
 * @brief Frame memory stack — dual-ended linear allocator.
 *
 * Provides a simple stack-based allocator that can allocate from
 * the bottom (heap 0) or top (heap 1) of a pre-allocated memory
 * region. Used for frame-temporary allocations in the AcrSDK layer.
 *
 * Part of the AcrSDK common module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef FBMS_H
#define FBMS_H

#include "structs.h"
#include "types.h"
#include <stddef.h>

ptrdiff_t fmsCalcSpace(FL_FMS* lp);
s32 fmsInitialize(FL_FMS* lp, void* memory_ptr, s32 memsize, s32 memalign);
void* fmsAllocMemory(FL_FMS* lp, s32 bytes, s32 heapnum);
s32 fmsGetFrame(FL_FMS* lp, s32 heapnum, FMS_FRAME* frame);

#endif
