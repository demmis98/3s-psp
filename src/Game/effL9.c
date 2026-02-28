#include "Game/effL9.h"
#include "bin2obj/char_table.h"
#include "common.h"
#include "Game/CHARSET.h"
#include "Game/EFFECT.h"
#include "Game/PLCNT.h"
#include "Game/aboutspr.h"
#include "Game/bg.h"
#include "Game/texcash.h"
#include "Game/workuser.h"

void effect_L9_move(WORK* wkp, s32 /*unused*/) {
    WORK_Other* ewk = (WORK_Other*) wkp;
#if defined(TARGET_PS2)
    void set_char_move_init(WORK * wk, s16 koc, s32 index);
#endif

    WORK_Other* oya = (WORK_Other*)ewk->my_master;

    switch (ewk->wu.routine_no[1]) {
    case 0:
        ewk->wu.routine_no[1] += 1;
        ewk->wu.disp_flag = 1;
        set_char_move_init(&ewk->wu, 0, ewk->wu.char_index);
        break;

    case 1:
        char_move(&ewk->wu);

        if (oya->wu.routine_no[0] >= 3) {
            ewk->wu.routine_no[1] += 1;
            ewk->wu.disp_flag = 0;
        }

        ewk->wu.position_x = ewk->wu.xyz[0].disp.pos & 0xFFFF;
        ewk->wu.position_y = ewk->wu.xyz[1].disp.pos & 0xFFFF;
        sort_push_request4(&ewk->wu);
        break;

    case 2:
        ewk->wu.routine_no[1] += 1;
        break;

    default:
        all_cgps_put_back(&ewk->wu);
        push_effect_work(&ewk->wu);
        break;
    }
}

s32 effect_L9_init(WORK* wkp, s32 d) {
    WORK_Other* oya = (WORK_Other*) wkp;
    u8 ten_type = (u8) d;
#if defined(TARGET_PS2)
    s16 get_my_trans_mode(s32 curr);
#endif

    WORK_Other* ewk;
    s16 ix;

    if ((ix = pull_effect_work(4)) == -1) {
        return -1;
    }

    ewk = (WORK_Other*)frw[ix];
    ewk->my_master = (u32*)oya;
    ewk->master_id = oya->wu.id;
    ewk->wu.type = ten_type;
    ewk->wu.be_flag = 1;
    ewk->wu.id = 219;
    ewk->wu.work_id = 16;
    ewk->wu.my_priority = 64;
    ewk->wu.cgromtype = 1;
    ewk->wu.rl_flag = 0;
    ewk->wu.my_col_mode = 0x4200;
    ewk->wu.char_table[0] = _etc2_char_table;
    ewk->wu.my_family = 8;
    ewk->wu.my_col_code = 56;
    ewk->wu.position_x = ewk->wu.xyz[0].disp.pos & 0xFFFF;
    ewk->wu.position_y = ewk->wu.xyz[1].disp.pos & 0xFFFF;
    ewk->wu.xyz[0].disp.pos = bg_w.bgw[1].wxy[0].disp.pos;

    if (ewk->wu.type) {
        ewk->wu.char_index = 53;

        if (plw[Winner_id].wu.rl_flag) {
            ewk->wu.xyz[0].disp.pos = 191;
        } else {
            ewk->wu.xyz[0].disp.pos = 192;
        }

        ewk->wu.my_priority = ewk->wu.position_z = 9;
        ewk->wu.xyz[1].disp.pos = 24;
    } else {
        ewk->wu.char_index = 48;
        ewk->wu.my_priority = ewk->wu.position_z = 17;
        ewk->wu.xyz[0].disp.pos = 192;
        ewk->wu.xyz[1].disp.pos = 8;
    }

    ewk->wu.my_mts = 14;
    ewk->wu.my_trans_mode = get_my_trans_mode(ewk->wu.my_mts);
    return 0;
}
