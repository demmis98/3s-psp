#ifndef EMLSHIM_H_
#define EMLSHIM_H_

#include "structs.h"
#include "types.h"

typedef struct {
    u32 cmd;
    u32 guid;
    u8 mono;
} CSE_SYS_PARAM_MONO;

typedef struct {
    u32 cmd;
    u32 guid;
    u8 bank;
    u8 vol;
} CSE_SYS_PARAM_BANKVOL;

typedef struct {
    u32 cmd;
    u32 guid;
    CSE_REQP reqp;
    u16 pmd_speed;
    u16 pmd_depth;
    u16 amd_speed;
    u16 amd_depth;
} CSE_SYS_PARAM_LFO;

typedef struct {
    u32 cmd;
    u32 guid;
    CSE_REQP reqp;
    CSE_PHDP phdp;
} CSE_SYS_PARAM_SNDSTART;

typedef struct {
    u32 cmd;
    u32 guid;
    CSE_REQP reqp;
} CSE_SYS_PARAM_SECHANGE;

struct VWork;
struct VId;

void emlShimInit();
void emlShimSysSetVolume(CSE_SYS_PARAM_BANKVOL* param);
void emlShimSysSetMono(CSE_SYS_PARAM_MONO* param);
void emlShimStartSound(CSE_SYS_PARAM_SNDSTART* param);
void emlShimSeKeyOff(CSE_REQP* pReqp);
void emlShimSeStop(CSE_REQP* pReqp);
void emlShimSeSetLfo(CSE_SYS_PARAM_LFO* param);
void emlShimSeStopAll();
void emlShimWorkTick();

#endif // EMLSHIM_H_
