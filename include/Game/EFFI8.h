#ifndef EFFI8_H
#define EFFI8_H

#include "structs.h"
#include "types.h"

void effect_I8_move(WORK* wkp, s32 /*unused*/);
s32 check_ball_mizushibuki(s16 xx, s16 yy);
void setup_effI8(PLW* wk, const BBBSTable* dadr);

#endif
