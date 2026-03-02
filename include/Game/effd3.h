#ifndef EFFD3_H
#define EFFD3_H

#include "structs.h"
#include "types.h"

void effect_D3_move(WORK* wkp, s32 /*unused*/);
s32 effect_D3_init(WORK* /*unused*/, s32 d);
void akebono_finish(WORK_Other* ewk);
void syungoku_finish(WORK_Other* ewk);

#endif
