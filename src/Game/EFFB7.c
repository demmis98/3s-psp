#include "Game/EFFB7.h"
#include "bin2obj/char_table.h"
#include "common.h"
#include "Game/CHARSET.h"
#include "Game/EFFECT.h"
#include "Game/aboutspr.h"
#include "Game/bg.h"
#include "Game/n_input.h"
#include "Game/ta_sub.h"

void effect_B7_move(WORK* wkp, s32 /*unused*/) {
    WORK_Other* ewk = (WORK_Other*) wkp;
#if defined(TARGET_PS2)
    void set_char_move_init2(WORK * wk, s32 koc, s32 index, s32 ip, s32 scf);
#endif

    NAME_WK* np = (NAME_WK*)ewk->my_master;

    switch (ewk->wu.routine_no[0]) {
    case 0:
        ewk->wu.routine_no[0]++;
        ewk->wu.disp_flag = 1;
        set_char_move_init2(&ewk->wu, 0, 7, ewk->wu.old_rno[0] + 1, 0);
        break;

    case 1:
        disp_pos_trans_entry(ewk);
        break;

    case 2:
        ewk->wu.routine_no[0]++;
        break;

    default:
        all_cgps_put_back(&ewk->wu);
        push_effect_work(&ewk->wu);
        break;
    }
}
