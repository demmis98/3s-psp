#include <pspctrl.h>
#include <string.h>
#include "psp/pspPAD.h"

u8 playAsP2 = false;
extern volatile int g_request_pause;

TARPAD tarpad_root[2];

s32 tarPADInit() {

    memset(tarpad_root, 0, sizeof(tarpad_root));

    for (int i = 0; i < 2; i++) {
        tarpad_root[i].kind = 1 - i;    // so it detects pad 0 as connected and pad 1 as disconnected
        tarpad_root[i].anstate = 0x60;
        tarpad_root[i].state = i;
        tarpad_root[i].conn.port = i;
        tarpad_root[i].conn.slot = 0;
    }

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    return 1;
}

void tarPADDestroy() {}

/*

#include <pspiofilemgr.h>
#include <stdio.h>
#define FL_TEXTURE_MAX 256
extern u32 fltex_c[FL_TEXTURE_MAX];
extern FLTexture flTexture[FL_TEXTURE_MAX];
void dump_fltex_to_file() {
    SceUID file = sceIoOpen("ms0:/fltex_dump.txt",
                           PSP_O_WRONLY | PSP_O_CREAT | PSP_O_TRUNC,
                           0777);

    if (file < 0) {
        return; // failed to open file
    }

    char buffer[64];

    for (int i = 0; i < FL_TEXTURE_MAX; i++) {
        int len = snprintf(buffer, sizeof(buffer),
                           "ID %d: %lu %d %d %d\n", i, fltex_c[i], flTexture[i].width, flTexture[i].height, flTexture[i].format);

        sceIoWrite(file, buffer, len);
    }

    sceIoClose(file);
}
*/

void tarPADRead() {

    SceCtrlData pad;
    sceCtrlReadBufferPositive(&pad, 1);

    //TARPAD* tp = &tarpad_root[0];
    TARPAD* tp;

    if(playAsP2){
        tp = &tarpad_root[1];
        tarpad_root[0].kind = 0;
        tarpad_root[1].kind = 1;
    }
    else {
        tp = &tarpad_root[0];
        tarpad_root[0].kind = 1;
        tarpad_root[1].kind = 0;
    }

    u32 sw = 0;

    if (pad.Buttons & PSP_CTRL_HOLD){
        tp->sw = 0;
        return;
    }

    if(pad.Buttons & PSP_CTRL_HOME)
        g_request_pause = 1;

    if (pad.Buttons & PSP_CTRL_UP || pad.Ly < 0x40) sw |= 0x0001;
    if (pad.Buttons & PSP_CTRL_RIGHT || pad.Lx > 0xC0) sw |= 0x0008;
    if (pad.Buttons & PSP_CTRL_DOWN || pad.Ly > 0xC0) sw |= 0x0002;
    if (pad.Buttons & PSP_CTRL_LEFT || pad.Lx < 0x40) sw |= 0x0004;

    if (pad.Buttons & PSP_CTRL_TRIANGLE) sw |= 0x0200;
    if (pad.Buttons & PSP_CTRL_CIRCLE) sw |= 0x0020;
    if (pad.Buttons & PSP_CTRL_CROSS) sw |= 0x0010;
    if (pad.Buttons & PSP_CTRL_SQUARE) sw |= 0x0100;

    if (pad.Buttons & (PSP_CTRL_LTRIGGER)) sw |= 0x0400;
    if (pad.Buttons & (PSP_CTRL_RTRIGGER)) sw |= 0x0040;
    //if (pad.Buttons & PSP_CTRL_L1) sw |= 0x0800;
    //if (pad.Buttons & PSP_CTRL_L2) sw |= 0x0080;

    if (pad.Buttons & PSP_CTRL_START) sw |= 0x8000;
    if (pad.Buttons & PSP_CTRL_SELECT){
        sw |= 0x4000;
        //dump_fltex_to_file();
    }

    tp->sw = sw;

    //tp->stick[0].x = pad.Lx - 128;
    //tp->stick[0].y = pad.Ly - 128;

    tp->stick[1].x = 0;
    tp->stick[1].y = 0;
}