#ifndef SAVESUB_H_    // include guard
#define SAVESUB_H_

#include "types.h"

void SaveInit(s32 file_type, s32 save_mode);
s32 SaveMove();
void SaveMsg(u16 x, u16 y);

#endif  //  SAVESUB_H_