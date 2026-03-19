/**
 * @file ps2PAD.h
 * @brief Platform-specific pad driver — PS2/SDL gamepad reading and vibration.
 *
 * Implements the target-specific portion of the pad subsystem: reading
 * raw button/stick data from PS2 DualShock controllers (or SDL gamepads),
 * computing analog-to-digital lever conversion, and managing vibration
 * motor control.
 *
 * Part of the AcrSDK ps2 module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef PS2PAD_H
#define PS2PAD_H

#include "sf33rd/AcrSDK/common/pad.h"
#include "types.h"

typedef struct {
    u8 state;
    u8 anstate;
    u16 kind;
    PAD_CONN conn;
    u32 sw;
    PAD_ANSHOT anshot;
    PAD_STICK stick[2];
} TARPAD;

typedef struct {
    u8 state;
    u8 phase;
    u8 port;
    u8 slot;
    u8 kind;
    u8 vib;
    u8 socket_id;
    u8 pad_id;
    u32 bprofile;
    u32 vprofile;
    u32 vib_timer;
} PS2Slot;

extern TARPAD tarpad_root[2];
extern PS2Slot ps2slot[2];

s32 tarPADInit();
void tarPADDestroy();
void flPADConfigSetACRtoXX(s32 padnum, s16 a, s16 b, s16 c);
void tarPADRead();

#endif
