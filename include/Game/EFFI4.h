#ifndef EFFI4_H
#define EFFI4_H

#include "structs.h"
#include "types.h"

void effect_I4_move(WORK* wkp, s32 /*unused*/);
void effect_i4_hit_sub(WORK_Other* ewk);
void effi4_down_to_up(WORK_Other* ewk);
void effi4_up_to_down(WORK_Other* ewk);
s32 effect_I4_init(WORK* /*unused*/, s32 /*unused*/);

#endif
