#ifndef EFFXX_H
#define EFFXX_H

#include "types.h"

extern const void (*effmovejptbl[])(WORK *);
extern const s32 (*effinitjptbl[])(WORK* wk, u16 code);

#endif
