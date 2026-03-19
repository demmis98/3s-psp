/**
 * @file se.c
 * @brief Sound effect and BGM request handlers.
 *
 * Provides the per-character SE dispatch (Se_Shock, Se_Myself, Se_Let, etc.),
 * stage BGM selection, position-based stereo panning, and the debug sound code
 * overlay. All SE functions ultimately call SsRequestPan() in sound3rd.c.
 *
 * Part of the sound module.
 * Originally from the PS2 game module.
 */

#include "Game/sound/se.h"
#include "common.h"
#include "Game/appear.h"
#include "Game/debug/Debug.h"
#include "Game/plcnt.h"
#include "Game/workuser.h"
#include "Game/sound/se_data.h"
#include "Game/sound/sound3rd.h"
#include "Game/bg_sub.h"
#include "Game/work_sys.h"
#include "structs.h"

#define SDEB_SIZE 8
#define BGM_STAGE_DATA_SIZE 22
#define SE_PLAYER_OFFSET 0x300 /* Per-player SE code stride (P1 base, P1 + 0x300 = P2) */

u8 gSeqStatus[1];

s16 bgm_selector_arranged[8] = { 0, 1, 2, 1, 2, 1, 2, 1 };
s16 bgm_selector_arcade[8] = { 0, 1, 0, 1, 0, 1, 0, 1 };
s16* bgm_selector[2] = { bgm_selector_arranged, bgm_selector_arcade };

const u16 BGM_Stage_Data[22] = { 46, 1, 13, 34, 31, 4, 7, 16, 25, 28, 34, 1, 28, 43, 22, 10, 19, 40, 4, 37, 61, 62 };
const s16 SE_Shock_Data[7] = { 285, 286, 287, 288, 289, 305, 306 };
const s16 Finish_SE_Data[2][7] = { { 305, 306, 285, 286, 287, 288, 272 }, { 292, 293, 290, 291, 287, 288, 272 } };

/** @brief Select and play BGM for the given stage and round. */
void Stage_BGM(u16 Stage_Number, u16 Round_Number) {
    u16 code;

    if (Stage_Number >= BGM_STAGE_DATA_SIZE) {
        return;
    }

    if (Mode_Type == MODE_ARCADE && Play_Type == 0 && My_char[COM_id] == 17 && Bonus_Game_Flag == 0) {
        code = BGM_Stage_Data[17] + bgm_selector[sys_w.bgm_type][Round_Number & 7];
    } else {
        code = BGM_Stage_Data[Stage_Number] + bgm_selector[sys_w.bgm_type][Round_Number & 7];
    }

    *gSeqStatus = 0;

    if (code == 0x2E && gill_appear_check() == 0) {
        Go_BGM();
        return;
    }

    SsRequest(code);
}

/** @brief Play a sound effect by code (no panning). */
void Sound_SE(s16 Code) {
    SsRequest(Code);
}

/** @brief Request BGM playback by code (no panning). */
void BGM_Request(s16 Code) {
    SsRequest(Code);
}

/** @brief Request BGM playback with current-code collision check. */
void BGM_Request_Code_Check(u16 Code) {
    SsRequest_CC(Code);
}

/** @brief Stop background music. */
void BGM_Stop() {
    SsBgmOff();
}

/** @brief Kill all active sound effects. */
void SE_All_Off() {
    spu_all_off();
}

void Se_Dummy(WORK_Other* ewk, u16 Code) {}

/** @brief Shock/hit SE — switches to KO variant if target is dead. */
void Se_Shock(WORK_Other* ewk, u16 Code) {
    PLW* em;
    s16 xx;
    s16 zz;
    s16 uid;

    s16 assign1;


    if (ewk->wu.work_id == 1) {
        em = (PLW*)ewk->wu.target_adrs;
        uid = ewk->wu.id;
    } else {
        em = (PLW*)((PLW*)ewk->my_master)->wu.target_adrs;
        uid = ewk->master_id;
    }

    if (em->wu.work_id == 1 && em->wu.vital_new < 0) {
        for (xx = 0, assign1 = zz = 0x27; xx < 7; xx++) {
            if (Code == SE_Shock_Data[xx]) {
                zz = 0;
                break;
            }
        }
        Code += zz;
    }

    if (Code) {
        Code += uid * SE_PLAYER_OFFSET;
    }

    xx = Get_Position((PLW*)ewk);
    SsRequestPan(Code, xx, xx, 0, 2);
}

/** @brief Play SE from the caller's own player channel. */
void Se_Myself(WORK_Other* ewk, u16 Code) {
    s16 xx;
    s16 uid = ewk->wu.id;

    if ((ewk->wu.work_id == 1) || (uid = (ewk->master_id), uid < 2)) {
        if (Code) {
            Code += uid * SE_PLAYER_OFFSET;
        }

        xx = Get_Position((PLW*)ewk);
        SsRequestPan(Code, xx, xx, 0, 2);
    }
}

/** @brief Like Se_Myself but only plays when the character is alive. */
void Se_Myself_Die(WORK_Other* ewk, u16 Code) {
    s16 xx;

    if ((ewk->wu.work_id == 1) && (ewk->wu.vital_new >= 0)) {
        if (Code) {
            Code += ewk->wu.id * SE_PLAYER_OFFSET;
        }
        xx = Get_Position((PLW*)ewk);
        SsRequestPan(Code, xx, xx, 0, 2);
    }
}

/** @brief Play SE on the target's channel (hit reaction). */
void Se_Let(WORK_Other* ewk, u16 Code) {
    s16 xx;
    s16 uid;


    if (ewk->wu.work_id == 1) {
        uid = ewk->wu.id;
    } else {
        uid = ewk->master_id;
    }

    if (Code) {
        Code += uid * SE_PLAYER_OFFSET;
    }

    xx = Get_Position((PLW*)ewk);
    SsRequestPan(Code, xx, xx, 0, 2);
}

/** @brief Like Se_Let with special override codes for KO hits. */
void Se_Let_SP(WORK_Other* ewk, u16 Code) {
    PLW* em;
    s16 xx;
    s16 uid;

    if (ewk->wu.work_id == 1) {
        uid = ewk->wu.id;
    } else {
        uid = ewk->master_id;
    }

    em = (PLW*)ewk->wu.target_adrs;

    if ((em->wu.work_id == 1) && (em->wu.vital_new < 0)) {
        if (Code == 0x14B) {
            Code = 0x158;
        }
        if (Code == 0x13A) {
            Code = 0x15A;
        }
    }

    if (Code) {
        Code += uid * SE_PLAYER_OFFSET;
    }

    xx = Get_Position((PLW*)ewk);
    SsRequestPan(Code, xx, xx, 0, 2);
}

/** @brief Generic SE call — plays at the caller's screen position. */
void Call_Se(WORK_Other* ewk, u16 Code) {
    s16 xx;

    xx = Get_Position((PLW*)ewk);
    SsRequestPan(Code, xx, xx, 0, 2);
}

/** @brief Termination SE — only plays if character is airborne and alive. */
void Se_Term(WORK_Other* ewk, u16 Code) {
    s16 xx;

    if (ewk->wu.work_id != 1) {
        return;
    }

    if ((ewk->wu.mvxy.a[1].sp < 0) && (ewk->wu.xyz[1].disp.pos <= 64)) {
        return;
    }

    if (Code) {
        Code += ewk->wu.id * SE_PLAYER_OFFSET;
    }

    xx = Get_Position((PLW*)ewk);
    SsRequestPan(Code, xx, xx, 0, 2);
}

/** @brief Play the round-finish sound effect based on the last hit. */
void Finish_SE() {
    PLW* wk;
    s16 xx;
    s16 Code;

    Code = Check_Finish_SE();

    if (Code == -1) {
        return;
    }

    wk = &plw[Winner_id];

    if (Code) {
        Code += (wk->wu.id * SE_PLAYER_OFFSET);
    }

    xx = Get_Position(wk);
    SsRequestPan(Code, xx, xx, 0, 2);
}

/** @brief Look up the finish SE code from the last-called SE. Returns -1 if none. */
s32 Check_Finish_SE() {
    s16 xx;

    for (xx = 0; xx < 7; xx++) {
        if (Last_Called_SE == Finish_SE_Data[0][xx]) {
            return Finish_SE_Data[1][xx];
        }
    }

    return -1;
}

/** @brief Calculate stereo pan position (0x10–0x70) for a player on screen. */
u16 Get_Position(PLW* wk) {
    u16 xx;
    u16 yy;

    xx = get_center_position();
    xx -= 0xC0;
    xx = wk->wu.position_x - xx;
    xx /= 4;
    xx += 16;

    if (xx < 0x70) {
        return xx;
    }

    yy = get_center_position();

    if (yy > wk->wu.position_x) {
        return 0x10;
    }

    return 0x70;
}



void Store_Sound_Code(u16 code, void* rmc_void) {}
void Disp_Sound_Code() {}
