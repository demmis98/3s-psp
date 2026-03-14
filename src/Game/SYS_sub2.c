#include "Game/SYS_sub2.h"
#include "common.h"
#include "Game/WORK_SYS.h"

u8 dspwhPack(u8 xdsp, u8 ydsp) {
    u8 rnum = 100 - ydsp;
    rnum |= (100 - xdsp) * 16;
    return rnum;
}

void dspwhUnpack(u8 src, u8* xdsp, u8* ydsp) {
    *xdsp = 100 - ((src >> 4) & 0xF);
    *ydsp = 100 - (src & 0xF);
}

const s16 Adjust_XY_Data[4][2] = { { 18, 12 }, { 12, 8 }, { 6, 4 }, { 0, 0 } };

void Setup_Disp_Size(s16 /* unused */) {
}

void setup_pos_remake_key(s16 /* unused */) {
    // Do nothing
}
