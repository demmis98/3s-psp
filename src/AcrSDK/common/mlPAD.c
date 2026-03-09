#include "AcrSDK/common/mlPAD.h"
#include "common.h"
//#include "sf33rd/AcrSDK/ps2/flPADUSR.h"
#include "psp/pspPAD.h"
#include "Game/IOConv.h"
#include "structs.h"

#include <pspctrl.h>

#include <math.h>
#include <string.h>

const u8 fllever_flip_data[4][16] = {
    { 0x00, 0x01, 0x02, 0x00, 0x04, 0x05, 0x06, 0x00, 0x08, 0x09, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x01, 0x02, 0x00, 0x08, 0x09, 0x0A, 0x00, 0x04, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x02, 0x01, 0x00, 0x04, 0x06, 0x05, 0x00, 0x08, 0x0A, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x02, 0x01, 0x00, 0x08, 0x0A, 0x09, 0x00, 0x04, 0x06, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00 },
};

const u8 fllever_depth_flip_data[4][4] = {
    { 0x00, 0x01, 0x02, 0x03 },
    { 0x00, 0x01, 0x03, 0x02 },
    { 0x01, 0x00, 0x02, 0x03 },
    { 0x01, 0x00, 0x03, 0x02 },
};
const FLPAD_CONFIG fltpad_config_basic = {
    .conf_sw = { 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,  10, 11, 12, 13, 14, 15,
                 16, 17, 18, 19, 20, 21, 22, 23, 24, 24, 24, 24, 24, 24, 24, 24 },
    .flip_lever = 0,
    .flip_ast1 = 0,
    .flip_ast2 = 0,
    .free = 0,
    .abut_on = 8,
    .ast1_on = 48,
    .ast2_on = 48,
    .free2 = 0
};

const u32 flpad_io_map[25] = { 0x1,     0x2,     0x4,      0x8,      0x10,     0x20,     0x40,   0x80,    0x100,
                               0x200,   0x400,   0x800,    0x1000,   0x2000,   0x4000,   0x8000, 0x10000, 0x20000,
                               0x40000, 0x80000, 0x100000, 0x200000, 0x400000, 0x800000, 0x0 };

FLPAD* flpad_adr[2];
FLPAD_CONFIG flpad_config[2];
u8 NumOfValidPads;

FLPAD flpad_root[2];
FLPAD flpad_conf[2];

s32 flPADInitialize() {
    s32 i;
    s32 flag = tarPADInit();

    flPADWorkClear();

    flpad_adr[0] = flpad_root;
    flpad_adr[1] = flpad_conf;

    for (i = 0; i < 2; i++) {
        flPADConfigSet(&fltpad_config_basic, i);
    }

    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);

    return flag;
}

void flPADDestroy() {
    tarPADDestroy();
}

void flPADWorkClear() {
    memset(flpad_root, 0, sizeof(flpad_root));
    memset(flpad_conf, 0, sizeof(flpad_conf));
}

void flPADConfigSet(const FLPAD_CONFIG* adrs, s32 padnum) {
    flpad_config[padnum] = *adrs;

    //flPADConfigSetACRtoXX(padnum, flpad_config[padnum].abut_on, flpad_config[padnum].ast1_on, flpad_config[padnum].ast2_on);
}

void flPADGetALL() {
    s16 i;

    tarPADRead();
    NumOfValidPads = 0;

    for (i = 0; i < 2; i++) {
        flpad_adr[0][i].state = tarpad_root[i].state;
        flpad_adr[0][i].anstate = tarpad_root[i].anstate;
        flpad_adr[0][i].kind = tarpad_root[i].kind;
        flpad_adr[0][i].conn = tarpad_root[i].conn;

        if ((flpad_adr[0][i].kind != 0) && (flpad_adr[0][i].kind != 0x8000)) {
            NumOfValidPads += 1;
        }

        flpad_adr[0][i].stick[0] = tarpad_root[i].stick[0];
        flpad_adr[0][i].stick[1] = tarpad_root[i].stick[1];
        flpad_adr[0][i].anshot = tarpad_root[i].anshot;

        flupdate_pad_button_data(&flpad_adr[0][i], tarpad_root[i].sw);
        flupdate_pad_on_cnt(&flpad_adr[0][i]);
        flpad_adr[0][i].sw_repeat = flpad_adr[0][i].sw_new;
    }

    flPADACRConf();
}

void flPADACRConf() {
    u8* csh;
    u32 conf_data;
    u32 conf_data2;
    u32 st0;
    u32 ast1;
    u32 ast2;
    s16 i;
    s16 j;
    u8 depthflip[32];

    for (i = 0; i < 2; i++) {
        flpad_adr[1][i].state = flpad_adr[0][i].state;
        flpad_adr[1][i].anstate = flpad_adr[0][i].anstate;
        flpad_adr[1][i].kind = flpad_adr[0][i].kind;
        flpad_adr[1][i].conn = flpad_adr[0][i].conn;

        st0 = flpad_adr[0][i].sw & 0xF;
        ast1 = flpad_adr[0][i].sw >> 16 & 0xF;
        ast2 = flpad_adr[0][i].sw >> 20 & 0xF;
        conf_data = flpad_adr[0][i].sw & 0xFFF0;

        conf_data |= fllever_flip_data[flpad_config[i].flip_lever][st0];
        conf_data |= fllever_flip_data[flpad_config[i].flip_ast1][ast1] << 0x10;
        conf_data |= fllever_flip_data[flpad_config[i].flip_ast2][ast2] << 0x14;

        csh = flpad_config[i].conf_sw;

        for (j = 0; j < 4; j++) {
            depthflip[j] = flpad_adr[0][i].anshot.pow[fllever_depth_flip_data[flpad_config[i].flip_lever][j]];
        }

        for (j = 4; j < 0x10; j++) {
            depthflip[j] = flpad_adr[0][i].anshot.pow[j];
        }

        for (j = 0; j < 0x10; j++) {
            flpad_adr[1][i].anshot.pow[j] = 0;
        }

        conf_data2 = 0;

        for (j = 0; j < 0x18; j++) {
            if (conf_data & flpad_io_map[j]) {
                conf_data2 |= flpad_io_map[csh[j]];
            }

            if (csh[j] < 0x10) {
                if (flpad_adr[1][i].anshot.pow[csh[j]] < depthflip[j]) {
                    flpad_adr[1][i].anshot.pow[csh[j]] = depthflip[j];
                }
            } else if (csh[j] > 0x18) {
                padconf_setup_depth(flpad_adr[1][i].anshot.pow, depthflip[j], flpad_io_map[csh[j]]);
            }
        }

        flupdate_pad_button_data(&flpad_adr[1][i], conf_data2);
        flupdate_pad_on_cnt(&flpad_adr[1][i]);

        flpad_adr[1][i].sw_repeat = flpad_adr[1][i].sw_new;
        flpad_adr[1][i].stick[0] = flpad_adr[0][i].stick[0];

        switch (flpad_config[i].flip_ast1) {
        case 1:
            flpad_adr[1][i].stick[0].x = flpad_adr[1][i].stick[0].x * -1;
            flpad_adr[1][i].stick[0].ang = 540 - flpad_adr[1][i].stick[0].ang;
            break;

        case 2:
            flpad_adr[1][i].stick[0].y = flpad_adr[1][i].stick[0].y * -1;
            flpad_adr[1][i].stick[0].ang = 360 - flpad_adr[1][i].stick[0].ang;
            break;

        case 3:
            flpad_adr[1][i].stick[0].x = flpad_adr[1][i].stick[0].x * -1;
            flpad_adr[1][i].stick[0].y = flpad_adr[1][i].stick[0].y * -1;
            flpad_adr[1][i].stick[0].ang = flpad_adr[1][i].stick[0].ang + 180;
            break;
        }

        flpad_adr[1][i].stick[1] = flpad_adr[0][i].stick[1];

        switch (flpad_config[i].flip_ast2) {
        case 1:
            flpad_adr[1][i].stick[1].x = flpad_adr[1][i].stick[1].x * -1;
            flpad_adr[1][i].stick[1].ang = 540 - flpad_adr[1][i].stick[1].ang;
            break;

        case 2:
            flpad_adr[1][i].stick[1].y = flpad_adr[1][i].stick[1].y * -1;
            flpad_adr[1][i].stick[1].ang = 360 - flpad_adr[1][i].stick[1].ang;
            break;

        case 3:
            flpad_adr[1][i].stick[1].x = flpad_adr[1][i].stick[1].x * -1;
            flpad_adr[1][i].stick[1].y = flpad_adr[1][i].stick[1].y * -1;
            flpad_adr[1][i].stick[1].ang = flpad_adr[1][i].stick[1].ang + 180;
            break;
        }

        flpad_adr[1][i].stick[0].ang = flpad_adr[1][i].stick[0].ang % 360;
        flpad_adr[1][i].stick[1].ang = flpad_adr[1][i].stick[1].ang % 360;

        flupdate_pad_stick_dir(&flpad_adr[1][i].stick[0]);
        flupdate_pad_stick_dir(&flpad_adr[1][i].stick[1]);
    }
}

void padconf_setup_depth(u8* deps, u8 num, u32 iodat) {
    s32 i;

    for (i = 0; i < 16; i++) {
        if (iodat & flpad_io_map[i]) {
            if (deps[i] < num) {
                deps[i] = num;
            }

            if ((iodat ^= flpad_io_map[i]) == 0) {
                break;
            }
        }
    }
}

void flupdate_pad_stick_dir(PAD_STICK* st) {
    f32 radian;

    if ((st->y | st->x) == 0) {
        radian = 0.0f;
    } else {
        radian = atan2(-st->y, st->x);

        if (radian < 0.0f) {
            radian += 6.2831855f; // Pi * 2
        }
    }

    st->rad = radian;
}

void flupdate_pad_button_data(FLPAD* pad, u32 data) {
    pad->sw_old = pad->sw;
    pad->sw = data;
    pad->sw_new = pad->sw & (pad->sw_old ^ pad->sw);
    pad->sw_off = pad->sw_old & (pad->sw_old ^ pad->sw);
    pad->sw_chg = pad->sw_new | pad->sw_off;
}

void flupdate_pad_on_cnt(FLPAD* pad) {
    s16 i;

    for (i = 0; i < 24; i++) {
        if (pad->sw & flpad_io_map[i]) {
            if (pad->rpsw[i].ctr.press != 0xFF) {
                pad->rpsw[i].ctr.press += 1;
            }
        } else {
            pad->rpsw[i].work = 0;
        }
    }
}

void flPADSetRepeatSw(FLPAD* pad, u32 IOdata, u8 ctr, u8 times) {
    s32 i;
    u8 cmpctr;

    for (i = 0; i < 24; i++) {
        if (IOdata & flpad_io_map[i]) {
            if (pad->rpsw[i].ctr.sw_up >= times) {
                pad->rpsw[i].ctr.sw_up = times - 1;
            }

            cmpctr = ctr - pad->rpsw[i].ctr.sw_up * (ctr / times);

            if (pad->rpsw[i].ctr.press >= cmpctr) {
                pad->rpsw[i].ctr.press = 0;
                pad->rpsw[i].ctr.sw_up += 1;
                pad->sw_repeat |= flpad_io_map[i];
            }
        }
    }
}
