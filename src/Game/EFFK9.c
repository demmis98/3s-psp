#include "Game/EFFK9.h"
#include "bin2obj/char_table.h"
#include "common.h"
#include "Game/CHARSET.h"
#include "Game/EFFECT.h"
#include "Game/SLOWF.h"
#include "Game/aboutspr.h"
#include "Game/bg_data.h"
#include "Game/color3rd.h"
#include "Game/workuser.h"

void effect_K9_move(WORK* wkp, s32 /*unused*/) {
    WORK_Other* ewk = (WORK_Other*) wkp;
    WORK* mwk = (WORK*)ewk->my_master;

    switch (ewk->wu.routine_no[0]) {
    case 0:
        ewk->wu.routine_no[0]++;
        ewk->wu.disp_flag = 1;
        push_color_trans_req(0x36, 10);
        ewk->wu.my_col_code = 10;
        ewk->wu.my_priority = ewk->wu.position_z = 32;
        ewk->wu.my_clear_level = 0xA0;
        ewk->wu.my_mr_flag = 1;
        ewk->wu.my_mr.size.x = 127;
        ewk->wu.my_mr.size.y = 112;
        set_char_move_init(&ewk->wu, 0, 0x90);
        /* fallthrough */

    case 1:
        if (ewk->wu.dead_f == 0) {
            if (EXE_flag != 0 || Game_pause != 0 ||
                (ewk->wu.dir_old == mwk->now_koc && ewk->wu.dir_step == mwk->char_index &&
                 (char_move(&ewk->wu), ewk->wu.cg_type != 0xFF))) {
                sort_push_request(&ewk->wu);
                return;
            }
        }

        ewk->wu.disp_flag = 0;
        ewk->wu.routine_no[0] = 2;
        return;

    default:
    case 2:
        push_effect_work(&ewk->wu);
        return;
    }
}

s32 effect_K9_init(WORK* wk, s32 d) {
    u8 data = (u8) d;
    WORK_Other* ewk;
    s16 ix;

    if ((ix = pull_effect_work(5)) == -1) {
        return -1;
    }

    ewk = (WORK_Other*)frw[ix];
    ewk->my_master = (u32*)wk;
    ewk->wu.be_flag = 1;
    ewk->wu.id = 209;
    ewk->wu.type = data;
    ewk->wu.work_id = 16;
    ewk->wu.my_mts = 6;
    ewk->wu.my_family = 8;
    ewk->wu.position_x = 192;
    ewk->wu.position_y = 112 - base_y_pos;
    ewk->wu.dir_old = wk->now_koc;
    ewk->wu.dir_step = wk->char_index;
    *ewk->wu.char_table = _plef_char_table;
    return 0;
}
