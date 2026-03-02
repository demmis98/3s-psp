#ifndef EFFK2_H
#define EFFK2_H

#include "structs.h"
#include "types.h"

void effect_K2_move(WORK* wkp, s32 /*unused*/);
void setup_effK2(WORK* wk);
void setup_effK2_sync_bomb(WORK* wk);
void illegal_setup_effK2(WORK* wk, s16 ix);

#endif
