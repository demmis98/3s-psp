#include "Game/IOConv.h"
#include "common.h"
#include "AcrSDK/common/mlPAD.h"
#include "Game/WORK_SYS.h"
#include "Game/debug/Debug.h"
#include "Game/main.h"
#include "Game/workuser.h"
#include "structs.h"
#include <pspctrl.h>

#define BTN_UP       0x0001
#define BTN_DOWN     0x0002
#define BTN_LEFT     0x0004
#define BTN_RIGHT    0x0008
#define BTN_TRIANGLE 0x0010
#define BTN_CIRCLE   0x0020
#define BTN_CROSS    0x0040
#define BTN_SQUARE   0x0080
#define BTN_L1       0x0100
#define BTN_R1       0x0200
#define BTN_L2       0x0400
#define BTN_R2       0x0800
#define BTN_START    0x4000
#define BTN_SELECT   0x8000

IO io_w;

u32 ioconv_table[24][2] = { { 0x1, 0x1 },      { 0x2, 0x2 },      { 0x4, 0x4 },       { 0x8, 0x8 },
                            { 0x100, 0x10 },   { 0x200, 0x20 },   { 0x400, 0x40 },    { 0x800, 0x80 },
                            { 0x10, 0x100 },   { 0x20, 0x200 },   { 0x40, 0x400 },    { 0x80, 0x800 },
                            { 0x0, 0x1000 },   { 0x0, 0x2000 },   { 0x8000, 0x4000 }, { 0x4000, 0x8000 },
                            { 0x0, 0x10000 },  { 0x0, 0x20000 },  { 0x0, 0x40000 },   { 0x0, 0x80000 },
                            { 0x0, 0x100000 }, { 0x0, 0x200000 }, { 0x0, 0x400000 },  { 0x0, 0x800000 } };

void keyConvert() {
    for (int i = 0; i < 2; i++) {
        u32 ps2Buttons = 0;
        u32 pspButtons = io_w.data[i].sw; // already read from PSP

        if (pspButtons & PSP_CTRL_UP)       ps2Buttons |= BTN_UP;
        if (pspButtons & PSP_CTRL_DOWN)     ps2Buttons |= BTN_DOWN;
        if (pspButtons & PSP_CTRL_LEFT)     ps2Buttons |= BTN_LEFT;
        if (pspButtons & PSP_CTRL_RIGHT)    ps2Buttons |= BTN_RIGHT;
        if (pspButtons & PSP_CTRL_TRIANGLE) ps2Buttons |= BTN_TRIANGLE;
        if (pspButtons & PSP_CTRL_CIRCLE)   ps2Buttons |= BTN_CIRCLE;
        if (pspButtons & PSP_CTRL_CROSS)    ps2Buttons |= BTN_CROSS;
        if (pspButtons & PSP_CTRL_SQUARE)   ps2Buttons |= BTN_SQUARE;
        if (pspButtons & PSP_CTRL_LTRIGGER) ps2Buttons |= BTN_L1;
        if (pspButtons & PSP_CTRL_RTRIGGER) ps2Buttons |= BTN_R1;
        if (pspButtons & PSP_CTRL_SELECT)   ps2Buttons |= BTN_SELECT;
        if (pspButtons & PSP_CTRL_START)    ps2Buttons |= BTN_START;

        io_w.sw[i] = ps2Buttons; // store for your game logic
    }
    
    p1sw_buff = io_w.sw[0];
    p2sw_buff = io_w.sw[1];
}
