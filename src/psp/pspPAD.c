#include <pspctrl.h>
#include <string.h>
#include "psp/pspPAD.h"

TARPAD tarpad_root[2];

s32 tarPADInit() {

    memset(tarpad_root, 0, sizeof(tarpad_root));

    for (int i = 0; i < 2; i++) {
        tarpad_root[i].kind = 1;
        tarpad_root[i].anstate = 0x60;
        tarpad_root[i].state = 1;
        tarpad_root[i].conn.port = i;
        tarpad_root[i].conn.slot = 0;
    }

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    return 1;
}

void tarPADDestroy() {}

void tarPADRead() {

    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);

    TARPAD* tp = &tarpad_root[0];

    u32 sw = 0;

    if (pad.Buttons & PSP_CTRL_UP || pad.Ly < 0x40) sw |= 0x0001;
    if (pad.Buttons & PSP_CTRL_RIGHT || pad.Lx > 0xC0) sw |= 0x0008;
    if (pad.Buttons & PSP_CTRL_DOWN || pad.Ly > 0xC0) sw |= 0x0002;
    if (pad.Buttons & PSP_CTRL_LEFT || pad.Lx < 0x40) sw |= 0x0004;

    if (pad.Buttons & PSP_CTRL_TRIANGLE) sw |= 0x0200;
    if (pad.Buttons & PSP_CTRL_CIRCLE) sw |= 0x0020;
    if (pad.Buttons & PSP_CTRL_CROSS) sw |= 0x0010;
    if (pad.Buttons & PSP_CTRL_SQUARE) sw |= 0x0100;

    if (pad.Buttons & PSP_CTRL_LTRIGGER) sw |= 0x0400;
    if (pad.Buttons & PSP_CTRL_RTRIGGER) sw |= 0x0040;

    if (pad.Buttons & PSP_CTRL_START) sw |= 0x8000;
    if (pad.Buttons & PSP_CTRL_SELECT) sw |= 0x4000;

    tp->sw = sw;

    //tp->stick[0].x = pad.Lx - 128;
    //tp->stick[0].y = pad.Ly - 128;

    tp->stick[1].x = 0;
    tp->stick[1].y = 0;
}