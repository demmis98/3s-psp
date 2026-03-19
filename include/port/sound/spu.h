#ifndef SPU_H_
#define SPU_H_

#include "common.h"

#include <pspkernel.h>

struct SPUVConf {
    u32 pitch;
    u32 voll, volr;
    u16 adsr1, adsr2;
    u16 pmon;
};

extern SceUID soundLock;

void SPU_Lock();
void SPU_Unlock();

void SPU_Init(void (*cb)());
void SPU_Upload(u32 dst, void* src, u32 size);
void SPU_Tick(s16* output);
void SPU_VoiceStart(int vnum, u32 start_addr);
void SPU_VoiceGetConf(int vnum, struct SPUVConf* conf);
void SPU_VoiceSetConf(int vnum, struct SPUVConf* conf);
bool SPU_VoiceIsFinished(int vnum);
void SPU_VoiceKeyOff(int vnum);
void SPU_VoiceStop(int vnum);

#endif // SPU_H_
