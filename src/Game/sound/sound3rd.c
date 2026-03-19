
#include "sf33rd/Source/Common/PPGFile.h"
#include "sf33rd/Source/PS2/cseDataFiles/CSEData.h"

/**
 * @file sound3rd.c
 * @brief Main sound system controller — BGM and SE engine.
 *
 * Manages all sound output: BGM via ADX streaming (with fade, seamless, and
 * memory-loaded tracks), SE via SPU banks (CSE engine shim). Contains the
 * BGM table data for both arranged and arcade soundtracks, the per-frame
 * BGM_Server() state machine, and all SsRequest/SsBgm* public API functions.
 *
 * Part of the sound module.
 * Originally from the PS2 game module.
 */

#include "Game/sound/sound3rd.h"
#include "common.h"
#include "../main.h"

// Sound port specific shims
#include "port/sound/adx.h"
#include "port/sound/emlShim.h"

// Main CPS3 types
#include "structs.h"


extern s32 system_init_level;

// AcrSDK Legacy Sound Engine (Depends on structs.h for CSE_ECHOWORK)
#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/cse.h"
#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlMemMap.h"
#include "sf33rd/AcrSDK/MiddleWare/PS2/CapSndEng/emlTSB.h"



#include "Game/debug/Debug.h"
#include "Game/effect.h"
#include "Game/workuser.h"
#include "Game/gd3rd.h"
#include "Game/color3rd.h"
#include "Game/sound/se.h"
#include "Game/sound/se_data.h"
#include "Game/sound/sound_ids.h"
#include "Game/ramcnt.h"
#include "Game/sys_sub.h"
#include "Game/work_sys.h"

#include <SDL3/SDL.h>

#define ADX_STM_WORK_SIZE 252388
#define BGM_TABLE_SIZE 68
#define BGM_EXDATA_ARRANGED_SIZE 48
#define BGM_EXDATA_ARCADE_SIZE 32
#define ADX_VOLUME_TABLE_SIZE 128
#define BGM_TYPE_COUNT 2
#define BGM_PTIX 0x7F       /* ProcessSoundRequest sentinel: route to BGM subsystem */
#define PTIX_SKIP 0x7FFF    /* Sound request sentinel: skip this request entirely */
#define BGM_CODE_VS 0x33    /* BGM code for vocal/VS memory-loaded track */
#define BGM_CODE_EMSEL 0x39 /* BGM code for EmSel memory-loaded track */

// sbss
s16 se_level;
s16 bgm_level;
s16 bgm_vol_mix;
s16 bgm_vol_now;
s16 bgm_fade_ix;
s16 bgm_half_down;
s16 current_bgm;
s16 bgm_seamless_always;
BGMFade bgm_fade;
BGMExecution bgm_exe;
BGMRequest bgm_req;
s8* sdbd[3];

/* Master volume multiplier (0.0 = mute, 1.0 = full). Set via --volume CLI. */
float g_master_volume = 1.0f;

// SPU bank state - global for CSE inline migration
CSE_SYSWORK g_cseSysWork __attribute__((aligned(16)));

// bss
u8 adx_VS[198954];
u8 adx_EmSel[391168];
s8 adx_stm_work[ADX_STM_WORK_SIZE];

// data
BGMTableEntry bgm_table_arranged[68] = {
    { 0, 0, 0 },         { 16384, 58, 572 },  { 16385, 64, 588 },  { 16386, 64, 598 },  { 16387, 60, 616 },
    { 16388, 60, 642 },  { 16389, 60, 653 },  { 16390, 72, 681 },  { 16391, 72, 695 },  { 16392, 72, 716 },
    { 16393, 74, 728 },  { 16394, 76, 742 },  { 16395, 74, 758 },  { 16396, 80, 776 },  { 16397, 72, 790 },
    { 16398, 76, 808 },  { 16399, 72, 822 },  { 16400, 72, 833 },  { 16401, 72, 844 },  { 16402, 72, 857 },
    { 16403, 72, 871 },  { 16404, 72, 885 },  { 16405, 64, 904 },  { 16406, 64, 918 },  { 16407, 64, 932 },
    { 16408, 64, 946 },  { 16409, 64, 963 },  { 16410, 64, 977 },  { 16411, 78, 994 },  { 16412, 74, 1012 },
    { 16413, 74, 1026 }, { 16414, 78, 1044 }, { 16415, 76, 1064 }, { 16416, 76, 1075 }, { 16417, 78, 1093 },
    { 16418, 78, 1110 }, { 16419, 78, 1124 }, { 16420, 68, 1140 }, { 16421, 68, 1154 }, { 16422, 68, 1168 },
    { 16423, 72, 1186 }, { 16424, 72, 1204 }, { 16425, 72, 1223 }, { 16426, 56, 1241 }, { 16427, 56, 1255 },
    { 16428, 56, 1269 }, { 16429, 64, 1288 }, { 16430, 64, 1306 }, { 16431, 64, 1325 }, { 0, 72, 1344 },
    { 0, 72, 1345 },     { 0, 72, 1346 },     { 0, 84, 1347 },     { 0, 86, 1348 },     { 0, 56, 1349 },
    { 0, 76, 1350 },     { 0, 72, 1351 },     { 0, 72, 1352 },     { 0, 64, 1353 },     { 0, 72, 1354 },
    { 0, 56, 1355 },     { 0, 60, 1356 },     { 0, 60, 1357 },     { 0, 72, 1358 },     { 0, 72, 1359 },
    { 0, 48, 1360 },     { 0, 64, 1361 },     { 0, 112, 1362 }
};

BGMExecutionData bgm_exdata_arranged[48] = {
    { 573, 587, 574, 0 },    { 589, 597, 590, 0 },    { 599, 615, 600, 0 },    { 617, 641, 618, 0 },
    { 643, 652, 645, 0 },    { 654, 680, 657, 0 },    { 682, 694, 687, 0 },    { 696, 715, 700, 0 },
    { 717, 727, 720, 0 },    { 729, 741, 730, 0 },    { 743, 757, 746, 0 },    { 759, 775, 764, 0 },
    { 777, 789, 778, 0 },    { 791, 807, 792, 0 },    { 809, 821, 810, 0 },    { 823, 832, 824, 0 },
    { 834, 843, 835, 0 },    { 845, 856, 846, 0 },    { 858, 870, 859, 0 },    { 872, 884, 873, 0 },
    { 886, 903, 888, 0 },    { 905, 917, 906, 0 },    { 919, 931, 920, 0 },    { 933, 945, 934, 0 },
    { 947, 962, 951, 0 },    { 964, 976, 965, 0 },    { 978, 993, 982, 0 },    { 995, 1011, 1000, 0 },
    { 1013, 1025, 1014, 0 }, { 1027, 1043, 1028, 0 }, { 1045, 1063, 1048, 0 }, { 1065, 1074, 1067, 0 },
    { 1076, 1092, 1077, 0 }, { 1094, 1109, 1096, 0 }, { 1111, 1123, 1112, 0 }, { 1125, 1139, 1126, 0 },
    { 1141, 1153, 1142, 0 }, { 1155, 1167, 1156, 0 }, { 1169, 1185, 1170, 0 }, { 1187, 1203, 1188, 0 },
    { 1205, 1222, 1207, 0 }, { 1224, 1240, 1225, 0 }, { 1242, 1254, 1243, 0 }, { 1256, 1268, 1257, 0 },
    { 1270, 1287, 1272, 0 }, { 1289, 1305, 1290, 0 }, { 1307, 1324, 1309, 0 }, { 1326, 1343, 1328, 0 }
};

BGMTableEntry bgm_table_arcade[68] = {
    { 0, 0, 0 },         { 16384, 104, 91 },  { 16385, 104, 107 }, { 32768, 0, 0 },     { 16386, 104, 119 },
    { 16387, 104, 133 }, { 32768, 0, 0 },     { 16388, 104, 144 }, { 16389, 104, 159 }, { 32768, 0, 0 },
    { 16390, 104, 182 }, { 16391, 104, 196 }, { 32768, 0, 0 },     { 16392, 104, 212 }, { 16393, 104, 227 },
    { 32768, 0, 0 },     { 16394, 104, 246 }, { 16395, 104, 257 }, { 32768, 0, 0 },     { 16396, 104, 268 },
    { 16397, 104, 282 }, { 32768, 0, 0 },     { 16398, 104, 296 }, { 16399, 104, 310 }, { 32768, 0, 0 },
    { 16400, 104, 325 }, { 16401, 104, 342 }, { 32768, 0, 0 },     { 16402, 104, 356 }, { 16403, 104, 374 },
    { 32768, 0, 0 },     { 16404, 104, 389 }, { 16405, 104, 409 }, { 32768, 0, 0 },     { 16406, 104, 419 },
    { 16407, 104, 436 }, { 32768, 0, 0 },     { 16408, 104, 451 }, { 16409, 104, 465 }, { 32768, 0, 0 },
    { 16410, 104, 481 }, { 16411, 104, 499 }, { 32768, 0, 0 },     { 16412, 104, 517 }, { 16413, 104, 525 },
    { 32768, 0, 0 },     { 16414, 104, 534 }, { 16415, 104, 544 }, { 32768, 0, 0 },     { 0, 104, 555 },
    { 0, 104, 556 },     { 0, 104, 557 },     { 0, 104, 558 },     { 0, 104, 559 },     { 0, 104, 560 },
    { 0, 104, 561 },     { 0, 104, 562 },     { 0, 104, 563 },     { 0, 104, 564 },     { 0, 104, 565 },
    { 0, 104, 566 },     { 0, 104, 567 },     { 0, 104, 568 },     { 0, 104, 569 },     { 0, 72, 1359 },
    { 0, 48, 1360 },     { 0, 104, 570 },     { 0, 104, 571 }
};

BGMExecutionData bgm_exdata_arcade[32] = {
    { 92, 106, 93, 0 },   { 108, 118, 111, 0 }, { 120, 132, 121, 0 }, { 134, 143, 136, 0 }, { 145, 158, 151, 0 },
    { 160, 181, 166, 0 }, { 183, 195, 184, 0 }, { 197, 211, 200, 0 }, { 213, 226, 215, 0 }, { 228, 245, 230, 0 },
    { 247, 256, 249, 0 }, { 258, 267, 260, 0 }, { 269, 281, 270, 0 }, { 283, 295, 284, 0 }, { 297, 309, 298, 0 },
    { 311, 324, 313, 0 }, { 326, 341, 330, 0 }, { 343, 355, 344, 0 }, { 357, 373, 362, 0 }, { 375, 388, 377, 0 },
    { 390, 408, 393, 0 }, { 410, 418, 411, 0 }, { 420, 435, 422, 0 }, { 437, 450, 439, 0 }, { 452, 464, 453, 0 },
    { 466, 480, 469, 0 }, { 482, 498, 483, 0 }, { 500, 516, 501, 0 }, { 518, 524, 519, 0 }, { 526, 533, 528, 0 },
    { 535, 543, 536, 0 }, { 545, 554, 547, 0 }
};

// sdata
SoundEvent* cseTSBDataTable[21] = { TSB_SE,   TSB_PL00, TSB_PL01, TSB_PL02, TSB_PL03, TSB_PL04, TSB_PL05,
                                    TSB_PL06, TSB_PL07, TSB_PL08, TSB_PL09, TSB_PL10, TSB_PL11, TSB_PL12,
                                    TSB_PL13, TSB_PL14, TSB_PL15, TSB_PL16, TSB_PL17, TSB_PL18, TSB_PL19 };

s8* csePHDDataTable[21] = { PHD_SE,   PHD_PL00, PHD_PL01, PHD_PL02, PHD_PL03, PHD_PL04, PHD_PL05,
                            PHD_PL06, PHD_PL07, PHD_PL08, PHD_PL09, PHD_PL10, PHD_PL11, PHD_PL12,
                            PHD_PL13, PHD_PL14, PHD_PL15, PHD_PL16, PHD_PL17, PHD_PL18, PHD_PL19 };

u8 adx_NowOnMemoryType = 0xFF;

BGMTableEntry* bgm_table[2] = { bgm_table_arranged, bgm_table_arcade };
BGMExecutionData* bgm_exdata[2] = { bgm_exdata_arranged, bgm_exdata_arcade };

// Forward decls
static s32 adx_now_playing();
void spu_all_off();
static void sound_bgm_off();
void SsBgmOff();
static void bgm_play_request(s32 filenum, s32 flag);
static void bgm_seamless_clear();
static s32 bgm_separate_check();
static void bgm_volume_setup(s16 data);

extern const s16 adx_volume[128];

/** @brief Initialize the sound system — ADX, CSE/SPU shim, default levels. */
void Init_sound_system() {
    se_level = 15;
    bgm_level = 15;
    bgm_half_down = 0;
    current_bgm = 0;
    bgm_seamless_always = 0;
    sys_w.sound_mode = 0;
    sys_w.bgm_type = BGM_ARRANGED;
    ADX_Init();
    system_init_level |= 2;
    // Inline expansion of cseInitSndDrv()
    emlShimInit();
    mlTsbInit();
    g_cseSysWork.InitializeFlag = 1;
    g_cseSysWork.Counter = 0;
    for (u32 i = 0; i < SPUBANKID_MAX; i++) {
        g_cseSysWork.SpuBankId[i] = (u32)-1;
    }
    system_init_level |= 1;
}

/** @brief Check if a voice transfer is complete (stub — always returns 1). */
s32 sndCheckVTransStatus(s32 type) {
    // Keeping this for now, might use later?
    return 1;
}

/** @brief Load the initial SE bank and memory-map it for the SPU. */
void sndInitialLoad() {
    mlMemMapInit(&SpuMap);
    mlMemMapSetPhdAddr(0, *csePHDDataTable);
    mlTsbSetBankAddr(0, *cseTSBDataTable);
    load_any_color(109, 20); // This loads SE.bd (index 7)
}

/** @brief Load ADX vocal/EmSel files matching the current BGM type (arranged/arcade). */
void checkAdxFileLoaded() {
    u8* adr;
    s16 key;
    u16 fnum;

    if (adx_NowOnMemoryType == sys_w.bgm_type) {
        return;
    }

    if (sys_w.bgm_type == BGM_ARRANGED) {
        fnum = 89;
    } else {
        fnum = 90;
    }

    do {
        key = load_it_use_any_key(fnum, 21, 0);
    } while (key == 0);

    adr = (u8*)Get_ramcnt_address(key);
    ppgSetupCmpChunk(adr, 0, adx_VS);
    ppgSetupCmpChunk(adr, 1, adx_EmSel);
    Push_ramcnt_key(key);
    adx_NowOnMemoryType = sys_w.bgm_type;
}

/** @brief Shut down the sound system — stop ADX and SPU. */
void Exit_sound_system() {
    if (system_init_level & 2) {
        ADX_Exit();
        system_init_level &= ~2;
    }

    if (system_init_level & 1) {
        spu_all_off();
        system_init_level &= ~1;
    }
}

/** @brief Clear the BGM execution + request work areas. */
void Init_bgm_work() {
    SDL_zero(bgm_exe);
    SDL_zero(bgm_req);
}

/** @brief Stop all BGM and SE output. */
void sound_all_off() {
    sound_bgm_off();
    spu_all_off();
}

/** @brief Stop all SPU sound effect voices. */
void spu_all_off() {
    if (system_init_level & 1) {
        mlTsbStopAll();
        emlShimSeStopAll();
    }
}

/** @brief Stop BGM (ADX) if currently playing. */
static void sound_bgm_off() {
    if ((system_init_level & 2) && (adx_now_playing() != 0)) {
        SsBgmOff();
    }
}

/** @brief Apply the current SE volume level to the SPU bank. */
void setSeVolume(s16 level) {
    (void)level;
    f32 vol;

    if (system_init_level & 2) {
        vol = (127.0f / 15.0f) * se_level * g_master_volume;
        CSE_SYS_PARAM_BANKVOL param = {};
        param.bank = 0xFF;
        param.vol = (s32)vol;
        emlShimSysSetVolume(&param);
    }
}

/** @brief Apply mono/stereo sound mode to both CSE and ADX. */
void setupSoundMode() {
    if (system_init_level & 2) {
        ADX_SetMono(sys_w.sound_mode);
    }
}

/**
 * @brief Route a resolved sound request to the SPU or BGM subsystem.
 *
 * If ptix != 0x7F, dispatches to the SPU bank (cseTsbRequest).
 * If ptix == 0x7F, queues a BGM operation (play/stop/fade/seamless)
 * into bgm_req for the next BGM_Server() frame.
 */
static void ProcessSoundRequest(SoundRequestData* rmc, s16 pan) {
    if (rmc->ptix != BGM_PTIX) {
        if (pan < -0x20) {
            pan = -0x20;
        }

        if (pan > 0x20) {
            pan = 0x20;
        }

        if (rmc->code > 0x7F) {
            rmc->port = 0;
        }

        // Inline expansion of cseTsbRequest varargs
        s32 rtpc[10] = { 0 };
        rtpc[6] = pan;       // pan parameter (cmd=6)
        rtpc[2] = rmc->port; // port parameter (cmd=2)
        mlTsbRequest(rmc->ptix, rmc->code, rtpc);
        return;
    }

    bgm_req.req = 1;

    switch (bgm_req.kind = rmc->bank) {
    case 5:
        if (bgm_exe.kind == 5) {
            bgm_req.req = 0;
            break;
        }

    case 7:
        bgm_req.data = rmc->port;
        bgm_req.code = -1;
        break;

    case 9:
        if ((adx_now_playing() != 0) && (bgm_exe.code == rmc->code)) {
            bgm_req.kind = 7;
            bgm_req.data = 0;
            bgm_req.code = -1;
            return;
        }

        bgm_req.kind = 4;
        /* fallthrough */

    case 2:
    case 4:
        bgm_req.data = 0;
        bgm_req.code = rmc->code;
        break;

    case 6:
        bgm_req.data = rmc->port;
        bgm_req.code = rmc->code;
        break;

    case 0:
    case 1:
    case 3:
    case 8:
        bgm_req.data = 0;
        bgm_req.code = -1;
        /* fallthrough */

    default:
        break;
    }
}

/**
 * @brief Per-frame BGM state machine.
 *
 * Processes queued bgm_req commands (play, stop, fade-in/out, seamless,
 * half-volume) and drives ADX playback. Handles seamless track looping
 * by auto-queuing the next entry from bgm_exdata when a track ends.
 */
void BGM_Server() {
    if (!(system_init_level & 2)) {
        return;
    }

    ADX_ProcessTracks();

    if (sys_w.bgm_type >= BGM_TYPE_COUNT) {
        return;
    }

    if (bgm_req.req) {
        bgm_req.req = 0;
        bgm_exe.kind = bgm_req.kind;
        bgm_exe.data = bgm_req.data;

        if (bgm_req.code != -1) {
            bgm_exe.code = bgm_req.code;
        }

        bgm_exe.rno = 0;

        if (bgm_exe.code < 0 || bgm_exe.code >= BGM_TABLE_SIZE) {
            bgm_exe.kind = 0;
            return;
        }

        if (bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0x8000) {
            bgm_exe.kind = 0;
        }
    }

    if (bgm_exe.code > 0 && bgm_exe.code < BGM_TABLE_SIZE) {
        bgm_vol_mix = bgm_level * bgm_table[sys_w.bgm_type][bgm_exe.code].vol / 15;
    }

    switch (bgm_exe.kind) {
    case 1:
        ADX_Stop();
        bgm_seamless_clear();
        current_bgm = 0;
        bgm_exe.kind = 0;
        break;

    case 2:
        ADX_Stop();

        if ((bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0x4000) && (bgm_separate_check() != 0)) {
            bgm_exe.exIndex = bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0xFF;
            bgm_exe.exEntry = bgm_exdata[sys_w.bgm_type][bgm_exe.exIndex].numStart;
            bgm_volume_setup(0);
            ADX_Pause(1);

            bgm_play_request(bgm_exe.exEntry, 0);
            bgm_exe.nowSeamless = 1;

            ADX_StartSeamless();
        } else {
            bgm_seamless_clear();
            bgm_volume_setup(0);

            ADX_Pause(1);

            if (adx_NowOnMemoryType == sys_w.bgm_type) {
                switch (bgm_exe.code) {
                case BGM_CODE_VS:
                    ADX_StartMem(adx_VS, sizeof(adx_VS));
                    break;

                case BGM_CODE_EMSEL:
                    ADX_StartMem(adx_EmSel, sizeof(adx_EmSel));
                    break;

                default:
                    bgm_play_request(bgm_exe.code, 1);
                    break;
                }
            } else {
                bgm_play_request(bgm_exe.code, 1);
            }
        }

        current_bgm = bgm_exe.code;
        bgm_exe.kind = 0;
        break;

    case 3:
        ADX_Pause(0);

        bgm_exe.kind = 0;
        break;

    case 4:
        if ((bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0x4000) && (bgm_separate_check() != 0)) {
            if ((bgm_exe.nowSeamless == 0) || (bgm_exe.code != current_bgm)) {
                bgm_exe.exIndex = bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0xFF;
                bgm_exe.exEntry = bgm_exdata[sys_w.bgm_type][bgm_exe.exIndex].numStart;

                if (bgm_exe.nowSeamless == 0) {
                    ADX_Stop();
                    bgm_volume_setup(0);
                }

                bgm_play_request(bgm_exe.exEntry, 0);

                if (bgm_exe.nowSeamless == 0) {
                    bgm_exe.nowSeamless = 1;

                    ADX_StartSeamless();
                }
            }
        } else {
            bgm_seamless_clear();
            bgm_volume_setup(0);

            if (adx_NowOnMemoryType == sys_w.bgm_type) {
                switch (bgm_exe.code) {
                case BGM_CODE_VS:
                    ADX_StartMem(adx_VS, sizeof(adx_VS));
                    break;

                case BGM_CODE_EMSEL:
                    ADX_StartMem(adx_EmSel, sizeof(adx_EmSel));
                    break;

                default:
                    bgm_play_request(bgm_exe.code, 1);
                    break;
                }
            } else {
                bgm_play_request(bgm_exe.code, 1);
            }
        }

        if (ADX_IsPaused()) {
            ADX_Pause(0);
        }

        current_bgm = bgm_exe.code;
        bgm_exe.kind = 0;
        break;

    case 5:
        switch (bgm_exe.rno) {
        case 0:
            bgm_fade.in.dex.hi = bgm_vol_now;
            bgm_fade.in.dex.low = -0x8000;
            bgm_fade.speed = -(bgm_fade.in.cal / bgm_exe.data);
            bgm_fade.in.cal = 0;
            bgm_exe.rno = 1;
            /* fallthrough */

        case 1:
            if (adx_now_playing() == 0) {
                bgm_exe.rno = 3;
                break;
            } else {
                bgm_exe.rno = 2;
                bgm_exe.volume = 0;
            }

            /* fallthrough */

        case 2:
            bgm_fade.in.cal += bgm_fade.speed;
            bgm_volume_setup(bgm_fade.in.dex.hi);

            if (bgm_vol_now) {
                break;
            }

            /* fallthrough */

        default:
            bgm_exe.kind = 1;
            break;
        }

        break;

    case 6:
        switch (bgm_exe.rno) {
        case 0:
            bgm_fade.in.dex.hi = bgm_vol_mix;
            bgm_fade.in.dex.low = -0x8000;
            bgm_fade.speed = bgm_fade.in.cal / bgm_exe.data;

            if ((bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0x4000) && (bgm_separate_check() != 0)) {
                if ((bgm_exe.nowSeamless == 0) || (bgm_exe.code != current_bgm)) {
                    bgm_exe.exIndex = bgm_table[sys_w.bgm_type][bgm_exe.code].data & 0xFF;
                    bgm_exe.exEntry = bgm_exdata[sys_w.bgm_type][bgm_exe.exIndex].numStart;

                    if (bgm_exe.nowSeamless == 0) {
                        ADX_Stop();
                    }

                    bgm_play_request(bgm_exe.exEntry, 0);

                    if (bgm_exe.nowSeamless == 0) {
                        bgm_exe.nowSeamless = 1;

                        ADX_StartSeamless();
                    }
                }
            } else {
                bgm_seamless_clear();

                if (adx_NowOnMemoryType == sys_w.bgm_type) {
                    switch (bgm_exe.code) {
                    case BGM_CODE_VS:
                        ADX_StartMem(adx_VS, sizeof(adx_VS));
                        break;

                    case BGM_CODE_EMSEL:
                        ADX_StartMem(adx_EmSel, sizeof(adx_EmSel));
                        break;

                    default:
                        bgm_play_request(bgm_exe.code, 1);
                        break;
                    }
                } else {
                    bgm_play_request(bgm_exe.code, 1);
                }
            }

            if (ADX_IsPaused()) {
                ADX_Pause(0);
            }

            bgm_volume_setup(-0x7F);
            current_bgm = bgm_exe.code;
            bgm_exe.rno = 1;
            bgm_fade.in.dex.hi = -bgm_vol_mix;
            bgm_fade.in.dex.low = -0x8000;
            /* fallthrough */

        case 1:
            if (adx_now_playing() != 0) {
                bgm_exe.rno = 2;
                bgm_exe.volume = 0;
            } else {
                break;
            }

            /* fallthrough */

        case 2:
            bgm_fade.in.cal += bgm_fade.speed;
            bgm_volume_setup(bgm_fade.in.dex.hi);

            if (bgm_vol_now < bgm_vol_mix) {
                break;
            }

            /* fallthrough */

        default:
            bgm_exe.kind = 0;
            break;
        }

        break;

    case 7:
        bgm_vol_mix = bgm_level * bgm_table[sys_w.bgm_type][current_bgm].vol / 15;
        bgm_volume_setup(bgm_exe.data);
        bgm_exe.kind = 0;
        break;

    case 8:
        bgm_exe.kind = 0;
        break;
    }

    if (bgm_exe.nowSeamless && (ADX_GetNumFiles() <= 0)) {
        bgm_exe.exEntry += 1;

        if (bgm_exe.exEntry > bgm_exdata[sys_w.bgm_type][bgm_exe.exIndex].numEnd) {
            bgm_exe.exEntry = bgm_exdata[sys_w.bgm_type][bgm_exe.exIndex].numLoop;
        }

        bgm_play_request(bgm_exe.exEntry, 0);
    }
}

/** @brief Check whether seamless (multi-file) BGM playback is allowed. */
static s32 bgm_separate_check() {
    if (Debug_w[DEBUG_SOUND_SEAMLESS]) {
        return 1;
    }

    return (mpp_w.inGame | bgm_seamless_always) != 0;
}

/** @brief Enable/disable always-seamless BGM flag. */
void setupAlwaysSeamlessFlag(s16 flag) {
    bgm_seamless_always = flag;
}

/** @brief Queue a BGM file for ADX playback (seamless entry or direct start). */
static void bgm_play_request(s32 filenum, s32 flag) {
    if (flag == 0) {
        ADX_EntryAfs(filenum);
    } else {
        ADX_StartAfs(bgm_table[sys_w.bgm_type][filenum].fnum);
    }
}

/** @brief Stop seamless playback and reset the ADX entry queue. */
static void bgm_seamless_clear() {
    if (!bgm_exe.nowSeamless) {
        return;
    }

    bgm_exe.nowSeamless = 0;

    ADX_Stop();
    ADX_ResetEntry();
}

/** @brief Set ADX output volume from a signed offset + mix level. */
static void bgm_volume_setup(s16 data) {
    s16 bhd;

    bgm_fade_ix = data;
    bhd = bgm_fade_ix;
    bgm_vol_now = bhd + bgm_vol_mix;

    if (bgm_half_down) {
        bgm_vol_now = 0;
    }

    if (bgm_vol_now > bgm_vol_mix) {
        bgm_vol_now = bgm_vol_mix;
    }

    if (bgm_vol_now < 0) {
        bgm_vol_now = 0;
    }

    if (bgm_vol_now >= ADX_VOLUME_TABLE_SIZE) {
        bgm_vol_now = ADX_VOLUME_TABLE_SIZE - 1;
    }

    if (Debug_w[DEBUG_PUB_BGM_OFF]) {
        bgm_vol_now = 0;
    }

    ADX_SetOutVol(adx_volume[(s32)(bgm_vol_now * g_master_volume)]);
}

/** @brief Check if ADX is currently playing. */
static s32 adx_now_playing() {
    bgm_exe.state = ADX_GetState();

    if (bgm_exe.state == ADX_STATE_PLAYING) {
        return 1;
    }

    return 0;
}

/** @brief Check if ADX playback has finished. */
s32 adx_now_playend() {
    bgm_exe.state = ADX_GetState();

    if (bgm_exe.state == ADX_STATE_PLAYEND) {
        return 1;
    }

    return 0;
}

/** @brief Return BGM play status: 0=stopped, 1=fading, 2=playing. */
s32 bgm_play_status() {
    if (bgm_exe.kind == 5) {
        return 1;
    }

    if (adx_now_playing() != 0) {
        return 2;
    }

    return 0;
}

/** @brief Check if the given BGM code is marked as skip (0x8000 flag). */
s32 bgmSkipCheck(s32 code) {
    if (code < 0 || code >= BGM_TABLE_SIZE || sys_w.bgm_type >= BGM_TYPE_COUNT) {
        return 0;
    }
    return (bgm_table[sys_w.bgm_type][code].data & 0x8000) != 0;
}

/** @brief Stop all notes (BGM + SE) — legacy wrapper. */
void SsAllNoteOff() {
    sound_all_off();
}

/**
 * @brief Resolve a sound request ID and dispatch with stereo panning.
 *
 * Looks up the CPS3 sound code in the lookup table, then routes through
 * ProcessSoundRequest() with the given pan offset.
 */
void SsRequestPan(u16 reqNum, s16 start, s16 /* unused */, s32 /* unused */, s32 /* unused */) {
    SoundRequestData rmcode;

    start -= 0x40;

    if (start < -0x3F) {
        start = -0x3F;
    }

    if (start > 0x3F) {
        start = 0x3F;
    }

    if (reqNum == 0) {
        rmcode.ptix = 0x7FFF;
        rmcode.bank = 0;
        rmcode.port = 0;
        rmcode.code = 0;
    } else {
        const SoundLookupEntry* lookup = Get_Sound_Lookup((SoundRequest)reqNum);
        if (lookup) {
            rmcode.ptix = lookup->ptix;
            rmcode.bank = lookup->bank;
            rmcode.port = lookup->port;
            rmcode.code = lookup->engine_code;
        } else {
            while (1) {
                flPrintL(3, 5, "MISSING SOUND MAPPING (PAN): %X", reqNum);
                njWaitVSync_with_N();
            }
        }
    }

    // Legacy behavior: some low IDs were skipped.
    // In extraction, we filtered ptix=0x7FFF.
    // If ptix is 0x7FFF, we should probably return or not call ProcessSoundRequest?
    // But ProcessSoundRequest handles ptix != 0x7F by calling cseTsbRequest.
    // If ptix is 0x7FFF, cseTsbRequest(0x7FFF...) is likely invalid.

    if (rmcode.ptix == PTIX_SKIP) {
        return;
    }

    Store_Sound_Code(reqNum, &rmcode);
    ProcessSoundRequest(&rmcode, start);
}

/** @brief Resolve a sound request ID and dispatch (center pan). */
void SsRequest(u16 ReqNumber) {
    SoundRequestData rmcode;

    if (ReqNumber == 0) {
        rmcode.ptix = PTIX_SKIP;
        rmcode.bank = 0;
        rmcode.port = 0;
        rmcode.code = 0;
    } else {
        const SoundLookupEntry* lookup = Get_Sound_Lookup((SoundRequest)ReqNumber);
        if (lookup) {
            rmcode.ptix = lookup->ptix;
            rmcode.bank = lookup->bank;
            rmcode.port = lookup->port;
            rmcode.code = lookup->engine_code;
        } else {
            // Hard Fail: ID not found in lookup table
            while (1) {
                flPrintL(3, 5, "MISSING SOUND MAPPING: %X", ReqNumber);
                njWaitVSync_with_N();
            }
        }
    }

    Store_Sound_Code(ReqNumber, &rmcode);
    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Like SsRequest but uses bank=9 (current-code collision check). */
void SsRequest_CC(u16 num) {
    SoundRequestData rmcode;

    if (num == 0) {
        rmcode.ptix = PTIX_SKIP;
        rmcode.bank = 0;
        rmcode.port = 0;
        rmcode.code = 0;
    } else {
        const SoundLookupEntry* lookup = Get_Sound_Lookup((SoundRequest)num);
        if (lookup) {
            rmcode.ptix = lookup->ptix;
            rmcode.bank = lookup->bank;
            rmcode.port = lookup->port;
            rmcode.code = lookup->engine_code;
        } else {
            // Hard Fail: ID not found in lookup table
            while (1) {
                flPrintL(3, 5, "MISSING SOUND MAPPING: %X", num);
                njWaitVSync_with_N();
            }
        }
    }

    Store_Sound_Code(num, &rmcode);
    rmcode.bank = 9;
    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Queue a BGM track for standby (bank=2, no immediate play). */
void Standby_BGM(u16 num) {
    SoundRequestData rmcode;

    if (num == 0) {
        rmcode.ptix = PTIX_SKIP;
        rmcode.bank = 0;
        rmcode.port = 0;
        rmcode.code = 0;
    } else {
        const SoundLookupEntry* lookup = Get_Sound_Lookup((SoundRequest)num);
        if (lookup) {
            rmcode.ptix = lookup->ptix;
            rmcode.bank = lookup->bank;
            rmcode.port = lookup->port;
            rmcode.code = lookup->engine_code;
        } else {
            // Hard Fail: ID not found in lookup table
            while (1) {
                flPrintL(3, 5, "MISSING SOUND MAPPING: %X", num);
                njWaitVSync_with_N();
            }
        }
    }

    Store_Sound_Code(num, &rmcode);
    rmcode.bank = 2;
    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Resume/unpause BGM (bank=3 request). */
void Go_BGM() {
    SoundRequestData rmcode;

    rmcode.ptix = BGM_PTIX;
    rmcode.bank = 3;
    rmcode.port = 0;
    rmcode.code = 0;
    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Stop BGM (bank=1 request). */
void SsBgmOff() {
    SoundRequestData rmcode;

    rmcode.ptix = BGM_PTIX;
    rmcode.bank = 1;
    rmcode.port = 0;
    rmcode.code = 0;
    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Start BGM with a fade-in effect (bank=6 request). */
void SsBgmFadeIn(u16 ReqNumber, u16 FadeSpeed) {
    SoundRequestData rmcode;
    if (FadeSpeed == 0) {
        return;
    }
    s32 fade_time = 0x8000 / FadeSpeed;

    if (ReqNumber == 0) {
        rmcode.ptix = PTIX_SKIP;
        rmcode.bank = 0;
        rmcode.port = 0;
        rmcode.code = 0;
    } else {
        const SoundLookupEntry* lookup = Get_Sound_Lookup((SoundRequest)ReqNumber);
        if (lookup) {
            rmcode.ptix = lookup->ptix;
            rmcode.bank = lookup->bank;
            rmcode.port = lookup->port;
            rmcode.code = lookup->engine_code;
        } else {
            // Hard Fail: ID not found in lookup table
            while (1) {
                flPrintL(3, 5, "MISSING SOUND MAPPING: %X", ReqNumber);
                njWaitVSync_with_N();
            }
        }
    }

    if (rmcode.ptix == BGM_PTIX) {
        Store_Sound_Code(ReqNumber, &rmcode);
        rmcode.bank = 6;
        rmcode.port = fade_time;
        ProcessSoundRequest(&rmcode, 0);
    }
}

/** @brief Fade out the current BGM over the given frame count (bank=5). */
void SsBgmFadeOut(u16 time) {
    SoundRequestData rmcode;
    if (time == 0) {
        return;
    }
    s32 fade_time = 0x8000 / time;

    rmcode.ptix = BGM_PTIX;
    rmcode.bank = 5;
    rmcode.code = 0;
    rmcode.port = fade_time;

    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Set BGM volume offset (bank=7 request), clamped to [-0x7F, 0]. */
void SsBgmControl(s8 /* unused */, s8 VOLUME) {
    SoundRequestData rmcode;

    rmcode.ptix = BGM_PTIX;
    rmcode.bank = 7;
    rmcode.code = 0;
    rmcode.port = VOLUME;

    if (rmcode.port < -0x7F) {
        rmcode.port = -0x7F;
    }

    if (rmcode.port > 0) {
        rmcode.port = 0;
    }

    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Set half-volume mode for BGM (used during voice-over scenes). */
void SsBgmHalfVolume(s16 flag) {
    SoundRequestData rmcode;

    bgm_half_down = flag;
    rmcode.ptix = BGM_PTIX;
    rmcode.bank = 7;
    rmcode.code = 0;
    rmcode.port = 0;

    ProcessSoundRequest(&rmcode, 0);
}

/** @brief Play the menu cursor-move sound effect. */
void SE_cursor_move() {
    SsRequest(96);
}

/** @brief Play the menu selection confirm sound effect. */
void SE_selected() {
    SsRequest(98);
}

/** @brief Play the direction-select cursor-move sound effect. */
void SE_dir_cursor_move() {
    SsRequest(343);
}

/** @brief Play the direction-select confirm sound effect. */
void SE_dir_selected() {
    SsRequest(98);
}

const s16 adx_volume[128] = { -999, -608, -576, -544, -512, -480, -448, -416, -400, -384, -368, -352, -336, -320, -304,
                              -288, -280, -272, -264, -256, -248, -240, -232, -224, -216, -208, -200, -192, -184, -176,
                              -168, -160, -156, -152, -148, -144, -140, -136, -132, -128, -124, -120, -116, -112, -108,
                              -104, -100, -96,  -94,  -92,  -90,  -88,  -86,  -84,  -82,  -80,  -78,  -76,  -74,  -72,
                              -70,  -68,  -66,  -64,  -63,  -62,  -61,  -60,  -59,  -58,  -57,  -56,  -55,  -54,  -53,
                              -52,  -51,  -50,  -49,  -48,  -47,  -46,  -45,  -44,  -43,  -42,  -41,  -40,  -39,  -38,
                              -37,  -36,  -35,  -34,  -33,  -32,  -31,  -30,  -29,  -28,  -27,  -26,  -25,  -24,  -23,
                              -22,  -21,  -20,  -19,  -18,  -17,  -16,  -15,  -14,  -13,  -12,  -11,  -10,  -9,   -8,
                              -7,   -6,   -5,   -4,   -3,   -2,   -1,   0 };
