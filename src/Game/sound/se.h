/**
 * @file se.h
 * @brief Public API for sound effect and BGM request handling.
 *
 * Declares per-character SE dispatch functions (Se_Shock, Se_Myself, Se_Let, etc.),
 * stage BGM selection, position-based panning, and the sound debug display.
 *
 * Part of the sound module.
 * Originally from the PS2 game module.
 */

#ifndef SE_H
#define SE_H

#include "structs.h"
#include "types.h"

extern u8 gSeqStatus[1];

void Stage_BGM(u16 Stage_Number, u16 Round_Number);
void Sound_SE(s16 Code);
void BGM_Request(s16 Code);
void BGM_Request_Code_Check(u16 Code);
void BGM_Stop();
void SE_All_Off();

void Finish_SE();
s32 Check_Finish_SE();
u16 Get_Position(PLW* wk);

void Store_Sound_Code(u16 code, void* rmc);
void Disp_Sound_Code();

#endif
