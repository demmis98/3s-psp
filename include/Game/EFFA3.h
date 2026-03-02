#ifndef EFFA3_H
#define EFFA3_H

#include "structs.h"
#include "types.h"

void effect_A3_move(WORK* wkp, s32 /*unused*/);
s32 effect_A3_init(s16 id, u8 Type, u8 Type_in_Type, u8 dir_step, u8 Death_Type, s16 pos_x, s16 pos_y, s16 Buff);

#endif
