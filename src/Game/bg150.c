#include "Game/bg150.h"
#include "common.h"
#include "Game/EFF25.h"
#include "Game/EFF44.h"
#include "Game/EFF85.h"
#include "Game/EFFI4.h"
#include "Game/PLCNT.h"
#include "Game/WORK_SYS.h"
#include "Game/bg.h"
#include "Game/bg_data.h"
#include "Game/bg_sub.h"
#include "Game/eff05.h"
#include "Game/eff06.h"
#include "Game/eff12.h"
#include "Game/eff94.h"
#include "Game/ta_sub.h"

void BG150() {
    bgw_ptr = &bg_w.bgw[1];
    bg1502();
    bgw_ptr = &bg_w.bgw[0];
    bg1501();
    bgw_ptr = &bg_w.bgw[2];
    bg1502_sync_common();
    zoom_ud_check();
    bg_pos_hosei2();
    Bg_Family_Set();
}

void bg1501() {
    void (*bg1501_jmp[2])() = { bg1501_init00, bg_move_common };
    bg1501_jmp[bgw_ptr->r_no_0]();
}

void bg1501_init00() {
    bgw_ptr->r_no_0++;
    bgw_ptr->old_pos_x = bgw_ptr->xy[0].disp.pos = bgw_ptr->pos_x_work = 0x200;
    bgw_ptr->hos_xy[0].cal = bgw_ptr->wxy[0].cal = bgw_ptr->xy[0].cal;
    bgw_ptr->zuubun = 0;
}

void bg1502() {
    void (*bg1502_jmp[2])() = { bg1502_init00, bg_base_move_common };
    bg1502_jmp[bgw_ptr->r_no_0]();
}

void bg1502_init00() {
    bgw_ptr->r_no_0++;
    bgw_ptr->old_pos_x = bgw_ptr->xy[0].disp.pos = bgw_ptr->pos_x_work = 0x200;
    bgw_ptr->hos_xy[0].cal = bgw_ptr->wxy[0].cal = bgw_ptr->xy[0].cal;
    bgw_ptr->zuubun = 0;
    effect_05_init(NULL, 0);
    effect_12_init(5);
    effect_06_init(NULL, 0);
    effect_44_init(8);
    effect_25_init(0);
    effect_94_init(0);
    effect_94_init(1);
    effect_I4_init(NULL, 0);
    effect_85_init(NULL, 0);
}

void bg1502_sync_common() {
    switch (bgw_ptr->r_no_0) {
    case 0:
        bgw_ptr->r_no_0++;
        bgw_ptr->fam_no = 2;
        bgw_ptr->old_pos_x = bgw_ptr->xy[0].disp.pos = bgw_ptr->pos_x_work = 0x200;
        bgw_ptr->hos_xy[0].cal = bgw_ptr->wxy[0].cal = bgw_ptr->xy[0].cal;
        bgw_ptr->zuubun = 0;
        bgw_ptr->y_limit = bgw_ptr->y_limit2 = 0xF0;
        bgw_ptr->pos_y_work = 0;
        bgw_ptr->xy[1].disp.pos = 0;
        bgw_ptr->speed_x = 0xF000;
        bgw_ptr->speed_y = 0xF000;
        sync_fam_set3(bgw_ptr->fam_no);
        break;

    case 1:
        bg_x_move_check();
        bg_y_move_check();
        sync_fam_set3(bgw_ptr->fam_no);
        break;
    }
}
