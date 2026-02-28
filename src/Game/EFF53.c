#include "Game/EFF53.h"
#include "common.h"
#include "Game/CHARSET.h"
#include "Game/EFF54.h"
#include "Game/EFFECT.h"
#include "Game/PLS02.h"
#include "Game/SLOWF.h"
#include "Game/aboutspr.h"
#include "Game/ta_sub.h"
#include "Game/workuser.h"

const s16 eff53_vanish_time[8] = { 480, 600, 900, 1440, 480, 1080, 1500, 600 };

void effect_53_move(WORK_Other* ewk) {
    s16 work;

    if (obr_no_disp_check()) {
        return;
    }

    if (EXE_flag || Game_pause || !EXE_obroll) {
        return;
    }

    switch (ewk->wu.routine_no[0]) {
    case 0:
        ewk->wu.old_rno[2]--;

        if (ewk->wu.old_rno[2] <= 0) {
            ewk->wu.routine_no[0]++;
            ewk->wu.old_rno[0] = 30;
            ewk->wu.old_rno[1] = 0;
            ewk->wu.disp_flag = 1;
        }

        break;

    case 1:
        ewk->wu.old_rno[0]--;

        if (ewk->wu.old_rno[0] > 0) {
            break;
        }

        ewk->wu.disp_flag ^= 1;
        ewk->wu.old_rno[0] = 30;

        if (ewk->wu.disp_flag) {
            break;
        }

        ewk->wu.old_rno[1]++;

        if (ewk->wu.old_rno[1] < 6) {
            break;
        }

        ewk->wu.routine_no[0] = 0;
        work = random_16();
        work &= 7;
        ewk->wu.old_rno[2] = eff53_vanish_time[work];
        ewk->wu.disp_flag = 0;
        break;

    default:
        all_cgps_put_back(&ewk->wu);
        push_effect_work(&ewk->wu);
        break;
    }
}

s32 effect_53_init() {
    WORK_Other* ewk;
    s16 ix;

    if ((ix = pull_effect_work(4)) == -1) {
        return -1;
    }

    ewk = (WORK_Other*)frw[ix];
    ewk->wu.be_flag = 1;
    ewk->wu.id = 53;
    ewk->wu.work_id = 16;
    ewk->wu.cgromtype = 1;
    ewk->wu.disp_flag = 0;
    ewk->wu.old_rno[2] = 0;
    effect_54_init(ewk);
    return 0;
}
