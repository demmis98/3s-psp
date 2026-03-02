#ifndef EFFD6_H
#define EFFD6_H

#include "structs.h"
#include "types.h"

void effect_D6_move(WORK* wkp, s32 /*unused*/);
s32 effect_D6_init(WORK* wk, s32 d);
void setup_hana_extra(WORK* wk, s16 num, s16 acc);

#endif
