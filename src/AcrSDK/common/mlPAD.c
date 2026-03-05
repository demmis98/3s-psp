#include "AcrSDK/common/mlPAD.h"
#include "common.h"
//#include "sf33rd/AcrSDK/ps2/flPADUSR.h"
//#include "sf33rd/AcrSDK/ps2/ps2PAD.h"
#include "structs.h"

#include <pspctrl.h>

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

FLPAD* flpad_adr[2];
FLPAD_CONFIG flpad_config[2];
u8 NumOfValidPads;

SceCtrlData flpad_root[2];
FLPAD flpad_conf[2];

void flPADConfigSetACRtoXX(s32 padnum, s16 a, s16 b, s16 c) {}

void flpad_ram_clear(u32* adrs_int, s32 xx) {
}

s32 flPADInitialize() {
    s32 i;

    // Initialize the PSP pad sampling mode
    sceCtrlSetSamplingCycle(0);  // Sampling cycle (0: automatic, 1: manual)
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);  // Set to analog mode

    return 1;  // Always returns 1 as success in PSP version
}

void flPADDestroy() {
    //tarPADDestroy();
}

void flPADWorkClear() {
    
}

void flPADConfigSet(const FLPAD_CONFIG* adrs, s32 padnum) {
    
}

void flPADGetALL() {
    s16 i = 0;

    SceCtrlData pad;

    // Read the current state of the pads (maximum 2 pads on PSP)
    sceCtrlReadBufferPositive(&pad, 1);  // Pass the buffer to store the data, reading 2 pads

    NumOfValidPads = 0;

    flpad_adr[0][i].state = 0;
    //flpad_adr[0][i].state = pad.Buttons;
    if(pad.Buttons & PSP_CTRL_UP){
        flpad_adr[0][i].state = 0xFF;
        flpad_adr[0][i].sw = 0xFF;
        flpad_adr[0][i].sw_new = 0xFF;
        flpad_adr[0][i].sw_chg = 0xFF;
    }

    flPADACRConf();  // Same function for configuration as in the PS2 code
}

void flPADACRConf() {
    
}

void padconf_setup_depth(u8* deps, u8 num, u32 iodat) {
    s32 i;

    /*
    for (i = 0; i < 0x10; i++) {
        if (iodat & flpad_io_map[i]) {
            if (deps[i] < num) {
                deps[i] = num;
            }

            if ((iodat ^= flpad_io_map[i]) == 0) {
                break;
            }
        }
    }
    */
}

f64 atan2(f64, f64);

void flupdate_pad_stick_dir(PAD_STICK* st) {
}

void flupdate_pad_button_data(FLPAD* pad, u32 data) {
    pad->sw_old = pad->sw;
    pad->sw = data;
    pad->sw_new = pad->sw & (pad->sw_old ^ pad->sw);
    pad->sw_off = pad->sw_old & (pad->sw_old ^ pad->sw);
    pad->sw_chg = pad->sw_new | pad->sw_off;
}

void flupdate_pad_on_cnt(FLPAD* pad) {
}

void flPADSetRepeatSw(FLPAD* pad, u32 IOdata, u8 ctr, u8 times) {
    s32 i;
    u8 cmpctr;

    /*
    for (i = 0; i < 0x18; i++) {
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
    */
}
