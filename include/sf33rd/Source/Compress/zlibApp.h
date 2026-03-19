/**
 * @file zlibApp.h
 * @brief zlib inflate wrapper with custom memory allocator.
 *
 * Provides a thin wrapper around zlib's inflate API using a dedicated
 * heap (MemMan) for zlib's internal allocations, avoiding use of the
 * system malloc.
 *
 * Part of the Compress module.
 * Originally from the PS2 compression module.
 */
#ifndef ZLIBAPP_H
#define ZLIBAPP_H

#include "types.h"

void zlib_Initialize(void* tempAdrs, s32 tempSize);
ssize_t zlib_Decompress(void* srcBuff, s32 srcSize, void* dstBuff, s32 dstSize);

#endif
