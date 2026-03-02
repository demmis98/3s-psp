#include "Game/bg040.h"
#include "common.h"
#include "Game/EFF44.h"
#include "Game/EFF53.h"
#include "Game/PLCNT.h"
#include "Game/WORK_SYS.h"
#include "Game/bg.h"
#include "Game/bg_data.h"
#include "Game/bg_sub.h"
#include "Game/eff06.h"
#include "Game/eff12.h"
#include "Game/ta_sub.h"

void BG040() {
    bgw_ptr = &bg_w.bgw[1];
    bg0402();
    bgw_ptr = &bg_w.bgw[0];
    bg0401();
    zoom_ud_check();
    bg_pos_hosei2();
    Bg_Family_Set_2();
}

void bg0401() {
    void (*bg0401_jmp[3])() = { bg0401_init00, bg0401_init01, bg_move_common };
    bg0401_jmp[bgw_ptr->r_no_0]();
}

void bg0401_init00() {
    bgw_ptr->r_no_0++;
    bgw_ptr->old_pos_x = bgw_ptr->xy[0].disp.pos = bgw_ptr->pos_x_work = 0x200;
    bgw_ptr->hos_xy[0].cal = bgw_ptr->wxy[0].cal = bgw_ptr->xy[0].cal;
    bgw_ptr->zuubun = 0;
    effect_12_init(1);
    effect_06_init(NULL, 0);
    effect_44_init(3);
    effect_53_init(NULL, 0);
}

void bg0401_init01() {
    bgw_ptr->r_no_0++;
}

void bg0402() {
    void (*bg0402_jmp[3])() = { bg0402_init00, bg0402_init01, bg_base_move_common };
    bg0402_jmp[bgw_ptr->r_no_0]();
}

void bg0402_init00() {
    bgw_ptr->r_no_0++;
    bgw_ptr->old_pos_x = bgw_ptr->xy[0].disp.pos = bgw_ptr->pos_x_work = 0x200;
    bgw_ptr->hos_xy[0].cal = bgw_ptr->wxy[0].cal = bgw_ptr->xy[0].cal;
    bgw_ptr->zuubun = 0;
}

void bg0402_init01() {
    bgw_ptr->r_no_0++;
}
