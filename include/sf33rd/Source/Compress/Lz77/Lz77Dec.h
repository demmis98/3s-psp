/**
 * @file Lz77Dec.h
 * @brief LZ77 variant decompressor.
 *
 * Decodes data compressed with the game's custom LZ77 variant which
 * supports multiple encoding modes: dictionary copies, literal runs,
 * fill runs, and incrementing-fill runs with 8-bit and 16-bit lengths.
 *
 * Part of the Compress module.
 * Originally from the PS2 compression module.
 */
#ifndef LZ77DEC_H
#define LZ77DEC_H

#include "types.h"

s32 decLZ77withSizeCheck(u8* src, u8* dst, s32 size);

#endif
