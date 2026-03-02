#ifndef EFF82_H
#define EFF82_H

#include "structs.h"
#include "types.h"

void effect_82_move(WORK* wkp, s32 /*unused*/);
#if defined(TARGET_PS2)
s32 effect_82_init(WORK* wk, s32 /* unused */);
#else
s32 effect_82_init(WORK* wk, s32 /*unused*/);
#endif

#endif
