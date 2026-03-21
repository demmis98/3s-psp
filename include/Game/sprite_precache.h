#ifndef SPRITE_PRECACHE_H
#define SPRITE_PRECACHE_H

#include "structs.h"
#include "types.h"

// Pre-decode all tiles for a given sprite pattern number into the MTS cache.
// This avoids runtime LZ77 decompression + GPU upload on first display.
void sprite_precache_pattern(s32 mts_id, u16 cg_number);

#endif
