#ifndef EFFB8_H
#define EFFB8_H

#include "structs.h"
#include "types.h"

extern s16 old_mes_no2;
extern s16 old_mes_no3;
extern s16 old_mes_no_pl;

void effect_B8_move(WORK* wkp, s32 /*unused*/);
s32 effect_B8_init(s8 WIN_PL_NO, s16 timer);

#endif
