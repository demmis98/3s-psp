#ifndef STRUCTS_H
#define STRUCTS_H

//#include "sf33rd/AcrSDK/common/plcommon.h"
#include "types.h"
//#include <libdma.h>
//#include <libgraph.h>

typedef struct {
    // total size: 0x4
    s16 l; // offset 0x0, size 0x2
    s16 h; // offset 0x2, size 0x2
} LoHi16;

typedef union {
    s32 sp;      // offset 0x0, size 0x4
    LoHi16 real; // offset 0x0, size 0x4
} Reg32SpReal;

typedef union {
    s32 cal;    // offset 0x0, size 0x4
    LoHi16 pos; // offset 0x0, size 0x4
} Reg32CalPos;


typedef struct {
    // total size: 0x18
    f32 x0; // offset 0x0, size 0x4
    f32 y0; // offset 0x4, size 0x4
    f32 x1; // offset 0x8, size 0x4
    f32 y1; // offset 0xC, size 0x4
    f32 ne; // offset 0x10, size 0x4
    f32 fa; // offset 0x14, size 0x4
} VPRM;

typedef struct {
    // total size: 0x8
    u8* pFrame;  // offset 0x0, size 0x4
    s32 heapnum; // offset 0x4, size 0x4
} FMS_FRAME;

typedef struct {
    // total size: 0x4C
    FMS_FRAME fmsFrame;    // offset 0x0, size 0x8
    u8* ramcntBuff;        // offset 0x8, size 0x4
    VPRM vprm;             // offset 0xC, size 0x18
    s8 ds_h[2];            // offset 0x24, size 0x2
    s8 ds_v[2];            // offset 0x26, size 0x2
    s8 sysStop;            // offset 0x28, size 0x1
    s8 initTrainingData;   // offset 0x29, size 0x1
    s8 inGame;             // offset 0x2A, size 0x1
    s8 ctrDemo;            // offset 0x2B, size 0x1
    s8 language;           // offset 0x2C, size 0x1
    s8 langload;           // offset 0x2D, size 0x1
    s8 cutAnalogStickData; // offset 0x2E, size 0x1
    s8 useAnalogStickData; // offset 0x2F, size 0x1
    s8 pal50Hz;            // offset 0x30, size 0x1
    u32 hanFbTex;          // offset 0x34, size 0x4
    u8 useChar[20];        // offset 0x38, size 0x14
} MPP;

struct _TASK {
    // total size: 0x14
    void (*func_adrs)();     // offset 0x0, size 0x4
    void (*callback_adrs)(); // offset 0x4, size 0x4
    u8 r_no[4];              // offset 0x8, size 0x4
    u16 condition;           // offset 0xC, size 0x2
    s16 timer;               // offset 0xE, size 0x2
    u8 free[4];              // offset 0x10, size 0x4
};

typedef struct {
    // total size: 0x10
    u16 boix; // offset 0x0, size 0x2
    u16 bhix; // offset 0x2, size 0x2
    u16 haix; // offset 0x4, size 0x2
    union /* @anon10 */ {
        u16 full; // offset 0x0, size 0x2
        struct /* @anon12 */ {
            // total size: 0x2
            u8 bx; // offset 0x0, size 0x1
            u8 mv; // offset 0x1, size 0x1
        } half;    // offset 0x0, size 0x2
    } mf;          // offset 0x6, size 0x2
    u16 caix;      // offset 0x8, size 0x2
    u16 cuix;      // offset 0xA, size 0x2
    u16 atix;      // offset 0xC, size 0x2
    u16 hoix;      // offset 0xE, size 0x2
} UNK_0;

typedef struct {
    // total size: 0x20
    s16 body_dm[4][4]; // offset 0x0, size 0x20
} UNK_1;

typedef struct {
    // total size: 0x20
    s16 hand_dm[4][4]; // offset 0x0, size 0x20
} UNK_2;

typedef struct {
    // total size: 0x8
    s16 cat_box[4]; // offset 0x0, size 0x8
} UNK_3;

typedef struct {
    // total size: 0x8
    s16 cau_box[4]; // offset 0x0, size 0x8
} UNK_4;

typedef struct {
    // total size: 0x20
    s16 att_box[4][4]; // offset 0x0, size 0x20
} UNK_5;

typedef struct {
    // total size: 0x8
    s16 hos_box[4]; // offset 0x0, size 0x8
} UNK_6;

typedef struct {
    // total size: 0x10
    u8 reaction; // offset 0x0, size 0x1
    u8 level;    // offset 0x1, size 0x1
    u8 mkh_ix;   // offset 0x2, size 0x1
    u8 but_ix;   // offset 0x3, size 0x1
    u8 dipsw;    // offset 0x4, size 0x1
    u8 guard;    // offset 0x5, size 0x1
    u8 dir;      // offset 0x6, size 0x1
    u8 free;     // offset 0x7, size 0x1
    u8 pow;      // offset 0x8, size 0x1
    u8 impact;   // offset 0x9, size 0x1
    u8 piyo;     // offset 0xA, size 0x1
    u8 ng_type;  // offset 0xB, size 0x1
    s8 hs_me;    // offset 0xC, size 0x1
    s8 hs_you;   // offset 0xD, size 0x1
    u8 hit_mark; // offset 0xE, size 0x1
    u8 dmg_mark; // offset 0xF, size 0x1
} UNK_7;

typedef struct {
    // total size: 0x10
    s16 parts_hos_x; // offset 0x0, size 0x2
    s16 parts_hos_y; // offset 0x2, size 0x2
    u8 parts_colmd;  // offset 0x4, size 0x1
    u8 parts_colcd;  // offset 0x5, size 0x1
    u8 parts_prio;   // offset 0x6, size 0x1
    u8 parts_flip;   // offset 0x7, size 0x1
    u8 parts_timer;  // offset 0x8, size 0x1
    u8 parts_disp;   // offset 0x9, size 0x1
    s16 parts_mts;   // offset 0xA, size 0x2
    u16 parts_nix;   // offset 0xC, size 0x2
    u16 parts_char;  // offset 0xE, size 0x2
} UNK_8;

typedef struct {
    // total size: 0x8
    s16 olc_ix[4]; // offset 0x0, size 0x8
} UNK_9;

typedef struct {
    // total size: 0x8
    s16 catch_hos_x; // offset 0x0, size 0x2
    s16 catch_hos_y; // offset 0x2, size 0x2
    u8 catch_prio;   // offset 0x4, size 0x1
    u8 catch_flip;   // offset 0x5, size 0x1
    s16 catch_nix;   // offset 0x6, size 0x2
} CatchTable;

typedef struct {
    // total size: 0x8
    u16 code; // offset 0x0, size 0x2
    s16 koc;  // offset 0x2, size 0x2
    s16 ix;   // offset 0x4, size 0x2
    s16 pat;  // offset 0x6, size 0x2
} UNK11;

typedef struct {
    // total size: 0x18
    Reg32SpReal a[2]; // offset 0x0, size 0x8
    Reg32SpReal d[2]; // offset 0x8, size 0x8
    s16 kop[2];       // offset 0x10, size 0x4
    u16 index;        // offset 0x14, size 0x2
} MVXY;

typedef union {
    s32 cal; // offset 0x0, size 0x4
    struct {
        // total size: 0x4
        s16 low; // offset 0x0, size 0x2
        s16 pos; // offset 0x2, size 0x2
    } disp;      // offset 0x0, size 0x4
} XY;

typedef struct {
    // total size: 0xA8
    s16 total;             // offset 0x0, size 0x2
    s16 new_dm;            // offset 0x2, size 0x2
    s16 req_f;             // offset 0x4, size 0x2
    s16 old_r;             // offset 0x6, size 0x2
    s16 kind_of[10][4][2]; // offset 0x8, size 0xA0
} ComboType;

typedef struct {
    // total size: 0x14
    s8 flag;    // offset 0x0, size 0x1
    s16 genkai; // offset 0x2, size 0x2
    s16 time;   // offset 0x4, size 0x2
    union {
        s32 timer;       // offset 0x0, size 0x4
        LoHi16 quantity; // offset 0x0, size 0x4
    } now;               // offset 0x8, size 0x4
    s32 recover;         // offset 0xC, size 0x4
    s16 store;           // offset 0x10, size 0x2
    s16 again;           // offset 0x12, size 0x2
} PiyoriType;

typedef struct {
    // total size: 0x388
    s8 be_flag;       // offset 0x0, size 0x1
    s8 disp_flag;     // offset 0x1, size 0x1
    u8 blink_timing;  // offset 0x2, size 0x1
    u8 operator;      // offset 0x3, size 0x1
    u8 type;          // offset 0x4, size 0x1
    u8 charset_id;    // offset 0x5, size 0x1
    s16 work_id;      // offset 0x6, size 0x2
    s16 id;           // offset 0x8, size 0x2
    s8 rl_flag;       // offset 0xA, size 0x1
    s8 rl_waza;       // offset 0xB, size 0x1
    u32* target_adrs; // offset 0xC, size 0x4
    u32* hit_adrs;    // offset 0x10, size 0x4
    u32* dmg_adrs;    // offset 0x14, size 0x4

    /// Index of the struct that is in front of this one in the list.
    /// offset 0x18, size 0x2
    s16 before;

    /// Index of this struct.
    /// offset 0x1A, size 0x2
    s16 myself;

    /// Index of the struct that is behind this one in the list.
    /// offset 0x1C, size 0x2
    s16 behind;

    /// Index of the list that this struct is a part of.
    // offset 0x1E, size 0x2
    s16 listix;

    s16 dead_f;                  // offset 0x20, size 0x2
    s16 timing;                  // offset 0x22, size 0x2
    s16 routine_no[8];           // offset 0x24, size 0x10
    s16 old_rno[8];              // offset 0x34, size 0x10
    s16 hit_stop;                // offset 0x44, size 0x2
    s16 hit_quake;               // offset 0x46, size 0x2
    s8 cgromtype;                // offset 0x48, size 0x1
    u8 kage_flag;                // offset 0x49, size 0x1
    s16 kage_hx;                 // offset 0x4A, size 0x2
    s16 kage_hy;                 // offset 0x4C, size 0x2
    s16 kage_prio;               // offset 0x4E, size 0x2
    s16 kage_width;              // offset 0x50, size 0x2
    s16 kage_char;               // offset 0x52, size 0x2
    s16 position_x;              // offset 0x54, size 0x2
    s16 position_y;              // offset 0x56, size 0x2
    s16 position_z;              // offset 0x58, size 0x2
    s16 next_x;                  // offset 0x5A, size 0x2
    s16 next_y;                  // offset 0x5C, size 0x2
    s16 next_z;                  // offset 0x5E, size 0x2
    s16 scr_mv_x;                // offset 0x60, size 0x2
    s16 scr_mv_y;                // offset 0x62, size 0x2
    XY xyz[3];                   // offset 0x64, size 0xC
    s16 old_pos[3];              // offset 0x70, size 0x6
    s16 sync_suzi;               // offset 0x76, size 0x2
    u16* suzi_offset;            // offset 0x78, size 0x4
    MVXY mvxy;                   // offset 0x7C, size 0x18
    s16 direction;               // offset 0x94, size 0x2
    s16 dir_old;                 // offset 0x96, size 0x2
    s16 dir_step;                // offset 0x98, size 0x2
    s16 dir_timer;               // offset 0x9A, size 0x2
    s16 vitality;                // offset 0x9C, size 0x2
    s16 vital_new;               // offset 0x9E, size 0x2
    s16 vital_old;               // offset 0xA0, size 0x2
    s16 dm_vital;                // offset 0xA2, size 0x2
    s16 dmcal_m;                 // offset 0xA4, size 0x2
    s16 dmcal_d;                 // offset 0xA6, size 0x2
    s8 weight_level;             // offset 0xA8, size 0x1
    UNK11 cmoa;                  // offset 0xAA, size 0x8
    UNK11 cmsw;                  // offset 0xB2, size 0x8
    UNK11 cmlp;                  // offset 0xBA, size 0x8
    UNK11 cml2;                  // offset 0xC2, size 0x8
    UNK11 cmja;                  // offset 0xCA, size 0x8
    UNK11 cmj2;                  // offset 0xD2, size 0x8
    UNK11 cmj3;                  // offset 0xDA, size 0x8
    UNK11 cmj4;                  // offset 0xE2, size 0x8
    UNK11 cmj5;                  // offset 0xEA, size 0x8
    UNK11 cmj6;                  // offset 0xF2, size 0x8
    UNK11 cmj7;                  // offset 0xFA, size 0x8
    UNK11 cmms;                  // offset 0x102, size 0x8
    UNK11 cmmd;                  // offset 0x10A, size 0x8
    UNK11 cmyd;                  // offset 0x112, size 0x8
    UNK11 cmcf;                  // offset 0x11A, size 0x8
    UNK11 cmcr;                  // offset 0x122, size 0x8
    UNK11 cmbk;                  // offset 0x12A, size 0x8
    UNK11 cmb2;                  // offset 0x132, size 0x8
    UNK11 cmb3;                  // offset 0x13A, size 0x8
    UNK11 cmhs;                  // offset 0x142, size 0x8
    UNK11 cmr0;                  // offset 0x14A, size 0x8
    UNK11 cmr1;                  // offset 0x152, size 0x8
    UNK11 cmr2;                  // offset 0x15A, size 0x8
    UNK11 cmr3;                  // offset 0x162, size 0x8
    s16 cmwk[32];                // offset 0x16A, size 0x40
    u32* char_table[12];         // offset 0x1AC, size 0x30
    u32* se_random_table;        // offset 0x1DC, size 0x4
    s16* step_xy_table;          // offset 0x1E0, size 0x4
    s16* move_xy_table;          // offset 0x1E4, size 0x4
    UNK_8* overlap_char_tbl;     // offset 0x1E8, size 0x4
    UNK_9* olc_ix_table;         // offset 0x1EC, size 0x4
    UNK_9 cg_olc;                // offset 0x1F0, size 0x8
    CatchTable* rival_catch_tbl; // offset 0x1F8, size 0x4
    CatchTable* curr_rca;        // offset 0x1FC, size 0x4
    u32* set_char_ad;            // offset 0x200, size 0x4
    s16 cg_ix;                   // offset 0x204, size 0x2
    s16 now_koc;                 // offset 0x206, size 0x2
    s16 char_index;              // offset 0x208, size 0x2
    s16 current_colcd;           // offset 0x20A, size 0x2
    s16 cgd_type;                // offset 0x20C, size 0x2
    u8 pat_status;               // offset 0x20E, size 0x1
    u8 kind_of_waza;             // offset 0x20F, size 0x1
    u8 hit_range;                // offset 0x210, size 0x1
    u8 total_paring;             // offset 0x211, size 0x1
    u8 total_att_set;            // offset 0x212, size 0x1
    u8 sp_tech_id;               // offset 0x213, size 0x1
    u8 cg_type;                  // offset 0x214, size 0x1
    u8 cg_ctr;                   // offset 0x215, size 0x1
    u16 cg_se;                   // offset 0x216, size 0x2
    u16 cg_olc_ix;               // offset 0x218, size 0x2
    u16 cg_number;               // offset 0x21A, size 0x2
    u16 cg_hit_ix;               // offset 0x21C, size 0x2
    s16 cg_att_ix;               // offset 0x21E, size 0x2
    u8 cg_extdat;                // offset 0x220, size 0x1
    u8 cg_cancel;                // offset 0x221, size 0x1
    u8 cg_effect;                // offset 0x222, size 0x1
    u8 cg_eftype;                // offset 0x223, size 0x1
    u16 cg_zoom;                 // offset 0x224, size 0x2
    u16 cg_rival;                // offset 0x226, size 0x2
    u16 cg_add_xy;               // offset 0x228, size 0x2
    u8 cg_next_ix;               // offset 0x22A, size 0x1
    u8 cg_status;                // offset 0x22B, size 0x1
    s16 cg_wca_ix;               // offset 0x22C, size 0x2
    s16 cg_jphos;                // offset 0x22E, size 0x2
    u16 cg_meoshi;               // offset 0x230, size 0x2
    u8 cg_prio;                  // offset 0x232, size 0x1
    u8 cg_flip;                  // offset 0x233, size 0x1
    u16 old_cgnum;               // offset 0x234, size 0x2
    s16 floor;                   // offset 0x236, size 0x2
    u16 ccoff;                   // offset 0x238, size 0x2
    s16 colcd;                   // offset 0x23A, size 0x2
    s16 my_col_mode;             // offset 0x23C, size 0x2
    s16 my_col_code;             // offset 0x23E, size 0x2
    s16 my_priority;             // offset 0x240, size 0x2
    s16 my_family;               // offset 0x242, size 0x2
    s16 my_ext_pri;              // offset 0x244, size 0x2
    s16 my_bright_type;          // offset 0x246, size 0x2
    s16 my_bright_level;         // offset 0x248, size 0x2
    s16 my_clear_level;          // offset 0x24A, size 0x2
    s16 my_mts;                  // offset 0x24C, size 0x2
    s16 my_mr_flag;              // offset 0x24E, size 0x2
    struct /* @anon47 */ {
        // total size: 0x4
        struct /* @anon48 */ {
            // total size: 0x4
            s16 x;     // offset 0x0, size 0x2
            s16 y;     // offset 0x2, size 0x2
        } size;        // offset 0x0, size 0x4
    } my_mr;           // offset 0x250, size 0x4
    s16 my_trans_mode; // offset 0x254, size 0x2
    s16 my_roll_flag;  // offset 0x256, size 0x2
    struct /* @anon52 */ {
        // total size: 0x8
        s16 now;           // offset 0x0, size 0x2
        s16 add;           // offset 0x2, size 0x2
        s16 free;          // offset 0x4, size 0x2
        s16 old;           // offset 0x6, size 0x2
    } my_roll;             // offset 0x258, size 0x8
    s16 waku_work_index;   // offset 0x260, size 0x2
    s16 olc_work_ix[4];    // offset 0x262, size 0x8
    UNK_0* hit_ix_table;   // offset 0x26C, size 0x4
    UNK_0 cg_ja;           // offset 0x270, size 0x10
    UNK_1* body_adrs;      // offset 0x280, size 0x4
    UNK_1* h_bod;          // offset 0x284, size 0x4
    UNK_2* hand_adrs;      // offset 0x288, size 0x4
    UNK_2* h_han;          // offset 0x28C, size 0x4
    UNK_2* dumm_adrs;      // offset 0x290, size 0x4
    UNK_2* h_dumm;         // offset 0x294, size 0x4
    UNK_3* catch_adrs;     // offset 0x298, size 0x4
    UNK_3* h_cat;          // offset 0x29C, size 0x4
    UNK_4* caught_adrs;    // offset 0x2A0, size 0x4
    UNK_4* h_cau;          // offset 0x2A4, size 0x4
    UNK_5* attack_adrs;    // offset 0x2A8, size 0x4
    UNK_5* h_att;          // offset 0x2AC, size 0x4
    UNK_5* h_eat;          // offset 0x2B0, size 0x4
    UNK_6* hosei_adrs;     // offset 0x2B4, size 0x4
    UNK_6* h_hos;          // offset 0x2B8, size 0x4
    UNK_7* att_ix_table;   // offset 0x2BC, size 0x4
    UNK_7 att;             // offset 0x2C0, size 0x10
    u16 zu_flag;           // offset 0x2D0, size 0x2
    u16 at_attribute;      // offset 0x2D2, size 0x2
    s16 kezuri_pow;        // offset 0x2D4, size 0x2
    u16 add_arts_point;    // offset 0x2D6, size 0x2
    u16 buttobi_type;      // offset 0x2D8, size 0x2
    u16 att_zuru;          // offset 0x2DA, size 0x2
    u16 at_ten_ix;         // offset 0x2DC, size 0x2
    s16 dir_atthit;        // offset 0x2DE, size 0x2
    s16 vs_id;             // offset 0x2E0, size 0x2
    u8 att_hit_ok;         // offset 0x2E2, size 0x1
    u8 meoshi_hit_flag;    // offset 0x2E3, size 0x1
    u16 at_koa;            // offset 0x2E4, size 0x2
    u8 paring_attack_flag; // offset 0x2E6, size 0x1
    s8 no_death_attack;    // offset 0x2E7, size 0x1
    u8 jump_att_flag;      // offset 0x2E8, size 0x1
    s8 shell_vs_refrect;   // offset 0x2E9, size 0x1
    s16 renew_attack;      // offset 0x2EA, size 0x2
    u16 attack_num;        // offset 0x2EC, size 0x2
    u16 uketa_att[4];      // offset 0x2EE, size 0x8
    union /* @anon16 */ {
        struct /* @anon18 */ {
            // total size: 0x2
            u8 player;     // offset 0x0, size 0x1
            u8 effect;     // offset 0x1, size 0x1
        } hit;             // offset 0x0, size 0x2
        u16 hit_flag;      // offset 0x0, size 0x2
    } hf;                  // offset 0x2F6, size 0x2
    s16 hit_mark_x;        // offset 0x2F8, size 0x2
    s16 hit_mark_y;        // offset 0x2FA, size 0x2
    s16 hit_mark_z;        // offset 0x2FC, size 0x2
    s16 kohm;              // offset 0x2FE, size 0x2
    u8 dm_fushin;          // offset 0x300, size 0x1
    s8 dm_weight;          // offset 0x301, size 0x1
    u16 dm_butt_type;      // offset 0x302, size 0x2
    u16 dm_zuru;           // offset 0x304, size 0x2
    u16 dm_attribute;      // offset 0x306, size 0x2
    s16 dm_guard_success;  // offset 0x308, size 0x2
    s16 dm_plnum;          // offset 0x30A, size 0x2
    s16 dm_attlv;          // offset 0x30C, size 0x2
    s16 dm_dir;            // offset 0x30E, size 0x2
    s8 dm_rl;              // offset 0x310, size 0x1
    u8 dm_impact;          // offset 0x311, size 0x1
    s16 dm_stop;           // offset 0x312, size 0x2
    s16 dm_quake;          // offset 0x314, size 0x2
    u16 dm_piyo;           // offset 0x316, size 0x2
    u16 dm_ten_ix;         // offset 0x318, size 0x2
    u16 dm_koa;            // offset 0x31A, size 0x2
    s16 dm_work_id;        // offset 0x31C, size 0x2
    u16 dm_arts_point;     // offset 0x31E, size 0x2
    u8 dm_jump_att_flag;   // offset 0x320, size 0x1
    u8 dm_free;            // offset 0x321, size 0x1
    s16 dm_count_up;       // offset 0x322, size 0x2
    s8 dm_nodeathattack;   // offset 0x324, size 0x1
    u8 dm_exdm_ix;         // offset 0x325, size 0x1
    u8 dm_dip;             // offset 0x326, size 0x1
    u8 dm_kind_of_waza;    // offset 0x327, size 0x1
    s16 attpow;            // offset 0x328, size 0x2
    s16 defpow;            // offset 0x32A, size 0x2
    u32* my_effadrs;       // offset 0x32C, size 0x4
    s16 shell_ix[8];       // offset 0x330, size 0x10
    s16 hm_dm_side;        // offset 0x340, size 0x2
    s16 extra_col;         // offset 0x342, size 0x2
    s16 extra_col_2;       // offset 0x344, size 0x2
    s16 original_vitality; // offset 0x346, size 0x2
    u8 hit_work_id;        // offset 0x348, size 0x1
    u8 dmg_work_id;        // offset 0x349, size 0x1
    s8 K5_init_flag;       // offset 0x34A, size 0x1
    s8 K5_exec_ok;         // offset 0x34B, size 0x1
    u8 kow;                // offset 0x34C, size 0x1
    u8 swallow_no_effect;  // offset 0x34D, size 0x1
    s16 E3_work_index;     // offset 0x34E, size 0x2
    s16 E4_work_index;     // offset 0x350, size 0x2
    u8 kezurare_flag;      // offset 0x352, size 0x1
    u8 wrd_free[53];       // offset 0x353, size 0x35
} WORK;

typedef struct {
    // total size: 0x2C
    s16 kind_of_arts; // offset 0x0, size 0x2
    u8 nmsa_g_ix;     // offset 0x2, size 0x1
    u8 exsa_g_ix;     // offset 0x3, size 0x1
    u8 exs2_g_ix;     // offset 0x4, size 0x1
    u8 nmsa_a_ix;     // offset 0x5, size 0x1
    u8 exsa_a_ix;     // offset 0x6, size 0x1
    u8 exs2_a_ix;     // offset 0x7, size 0x1
    s8 gauge_type;    // offset 0x8, size 0x1
    s8 mp;            // offset 0x9, size 0x1
    s8 ok;            // offset 0xA, size 0x1
    s8 ex;            // offset 0xB, size 0x1
    s8 ba;            // offset 0xC, size 0x1
    s8 dtm_mul;       // offset 0xD, size 0x1
    s8 mp_rno;        // offset 0xE, size 0x1
    s8 mp_rno2;       // offset 0xF, size 0x1
    s8 sa_rno;        // offset 0x10, size 0x1
    s8 sa_rno2;       // offset 0x11, size 0x1
    s8 ex_rno;        // offset 0x12, size 0x1
    s8 gt2;           // offset 0x13, size 0x1
    s8 saeff_ok;      // offset 0x14, size 0x1
    s8 saeff_mp;      // offset 0x15, size 0x1
    s16 gauge_len;    // offset 0x16, size 0x2
    union {
        s32 i;       // offset 0x0, size 0x4
        LoHi16 s;    // offset 0x0, size 0x4
    } gauge;         // offset 0x18, size 0x4
    s32 dtm;         // offset 0x1C, size 0x4
    s16 store_max;   // offset 0x20, size 0x2
    s16 store;       // offset 0x22, size 0x2
    s16 id_arts;     // offset 0x24, size 0x2
    u8 ex4th_full;   // offset 0x26, size 0x1
    u8 ex4th_exec;   // offset 0x27, size 0x1
    s16 total_gauge; // offset 0x28, size 0x2
    s16 bacckup_g_h; // offset 0x2A, size 0x2
} SA_WORK;

typedef struct {
    // total size: 0x406
    u16 sw_lvbt;       // offset 0x0, size 0x2
    u16 sw_new;        // offset 0x2, size 0x2
    u16 sw_old;        // offset 0x4, size 0x2
    u16 sw_now;        // offset 0x6, size 0x2
    u16 sw_off;        // offset 0x8, size 0x2
    u16 sw_chg;        // offset 0xA, size 0x2
    u16 old_now;       // offset 0xC, size 0x2
    s16 lgp;           // offset 0xE, size 0x2
    u8 ca14;           // offset 0x10, size 0x1
    u8 ca25;           // offset 0x11, size 0x1
    u8 ca36;           // offset 0x12, size 0x1
    u8 calf;           // offset 0x13, size 0x1
    u8 calr;           // offset 0x14, size 0x1
    u8 lever_dir;      // offset 0x15, size 0x1
    s16 waza_flag[56]; // offset 0x16, size 0x70
    s16 reset[56];     // offset 0x86, size 0x70
    u8 waza_r[56][4];  // offset 0xF6, size 0xE0
    u16 btix[56];      // offset 0x1D6, size 0x70
    u16 exdt[56][4];   // offset 0x246, size 0x1C0
} WORK_CP;

typedef struct {
    // total size: 0x6
    s16 r_no;    // offset 0x0, size 0x2
    s16 char_ix; // offset 0x2, size 0x2
    s16 data_ix; // offset 0x4, size 0x2
} AS;

typedef struct {
    // total size: 0x46C
    WORK wu;           // offset 0x0, size 0x388
    WORK_CP* cp;       // offset 0x388, size 0x4
    u32 spmv_ng_flag;  // offset 0x38C, size 0x4
    u32 spmv_ng_flag2; // offset 0x390, size 0x4

    /// Number of the character that the player controls.
    /// offset 0x394, size 0x2
    s16 player_number;

    s16 zuru_timer;       // offset 0x396, size 0x2
    u16 zuru_ix_counter;  // offset 0x398, size 0x2
    u8 zuru_flag;         // offset 0x39A, size 0x1
    s8 tsukamarenai_flag; // offset 0x39B, size 0x1
    u8 kizetsu_kow;       // offset 0x39C, size 0x1
    u8 micchaku_flag;     // offset 0x39D, size 0x1
    u8 hos_fi_flag;       // offset 0x39E, size 0x1
    u8 hos_em_flag;       // offset 0x39F, size 0x1

    /// Number of the character the player is throwing.
    /// offset 0x3A0, size 0x2
    s16 tsukami_num;

    /// Flag that's set if the player is throwing the opponent.
    /// offset 0x3A2, size 0x1
    s8 tsukami_f;

    /// Flag that's set if the player is being thrown by the opponent.
    /// offset 0x3A3, size 0x1
    s8 tsukamare_f;

    s8 kind_of_catch;       // offset 0x3A4, size 0x1
    u8 old_gdflag;          // offset 0x3A5, size 0x1
    u8 guard_flag;          // offset 0x3A6, size 0x1
    u8 guard_chuu;          // offset 0x3A7, size 0x1
    s16 dm_ix;              // offset 0x3A8, size 0x2
    s16 hosei_amari;        // offset 0x3AA, size 0x2
    s8 dm_hos_flag;         // offset 0x3AC, size 0x1
    u8 dm_point;            // offset 0x3AD, size 0x1
    s16 muriyari_ugoku;     // offset 0x3AE, size 0x2
    s8 scr_pos_set_flag;    // offset 0x3B0, size 0x1
    s8 hoshi_flag;          // offset 0x3B1, size 0x1
    s8 the_same_players;    // offset 0x3B2, size 0x1
    s8* dm_step_tbl;        // offset 0x3B4, size 0x4
    s8 running_f;           // offset 0x3B8, size 0x1
    s8 cancel_timer;        // offset 0x3B9, size 0x1
    s8 jpdir;               // offset 0x3BA, size 0x1
    s8 jptim;               // offset 0x3BB, size 0x1
    s16 current_attack;     // offset 0x3BC, size 0x2
    const AS* as;           // offset 0x3C0, size 0x4
    SA_WORK* sa;            // offset 0x3C4, size 0x4
    ComboType* cb;          // offset 0x3C8, size 0x4
    PiyoriType* py;         // offset 0x3CC, size 0x4
    s8 wkey_flag;           // offset 0x3D0, size 0x1
    s8 dead_flag;           // offset 0x3D1, size 0x1
    s16 ukemi_ok_timer;     // offset 0x3D2, size 0x2
    s16 backup_ok_timer;    // offset 0x3D4, size 0x2
    s8 uot_cd_ok_flag;      // offset 0x3D6, size 0x1
    s8 ukemi_success;       // offset 0x3D7, size 0x1
    s16 old_pos_data[8];    // offset 0x3D8, size 0x10
    s16 move_distance;      // offset 0x3E8, size 0x2
    s16 move_power;         // offset 0x3EA, size 0x2
    s16 sa_stop_sai;        // offset 0x3EC, size 0x2
    u8 saishin_lvdir;       // offset 0x3EE, size 0x1
    u8 sa_stop_lvdir;       // offset 0x3EF, size 0x1
    u8 sa_stop_flag;        // offset 0x3F0, size 0x1
    u8 kezurijini_flag;     // offset 0x3F1, size 0x1
    WORK* illusion_work;    // offset 0x3F4, size 0x4
    s16 image_setup_flag;   // offset 0x3F8, size 0x2
    s16 image_data_index;   // offset 0x3FA, size 0x2
    u8 caution_flag;        // offset 0x3FC, size 0x1
    u8 tc_1st_flag;         // offset 0x3FD, size 0x1
    ComboType* rp;          // offset 0x400, size 0x4
    s16 bullet_hcnt;        // offset 0x404, size 0x2
    s16 bhcnt_timer;        // offset 0x406, size 0x2
    s8 cat_break_ok_timer;  // offset 0x408, size 0x1
    s8 cat_break_reserve;   // offset 0x409, size 0x1
    s8 hazusenai_flag;      // offset 0x40A, size 0x1
    s8 hurimukenai_flag;    // offset 0x40B, size 0x1
    u8 tk_success;          // offset 0x40C, size 0x1
    u8 resurrection_resv;   // offset 0x40D, size 0x1
    s16 tk_dageki;          // offset 0x40E, size 0x2
    s16 tk_nage;            // offset 0x410, size 0x2
    s16 tk_kizetsu;         // offset 0x412, size 0x2
    s16 tk_konjyou;         // offset 0x414, size 0x2
    s16 utk_dageki;         // offset 0x416, size 0x2
    s16 utk_nage;           // offset 0x418, size 0x2
    s16 utk_kizetsu;        // offset 0x41A, size 0x2
    u8 atemi_flag;          // offset 0x41C, size 0x1
    u8 atemi_point;         // offset 0x41D, size 0x1
    s16 dm_vital_backup;    // offset 0x41E, size 0x2
    u8 dm_refrect;          // offset 0x420, size 0x1
    u8 dm_vital_use;        // offset 0x421, size 0x1
    u8 exdm_ix;             // offset 0x422, size 0x1
    u8 meoshi_jump_flag;    // offset 0x423, size 0x1
    s16 cmd_request;        // offset 0x424, size 0x2
    s16 rl_save;            // offset 0x426, size 0x2
    u8 zettai_muteki_flag;  // offset 0x428, size 0x1
    u8 do_not_move;         // offset 0x429, size 0x1
    u16 just_sa_stop_timer; // offset 0x42A, size 0x2
    s16 total_att_hit_ok;   // offset 0x42C, size 0x2
    u8 sa_healing;          // offset 0x42E, size 0x1
    u8 auto_guard;          // offset 0x42F, size 0x1
    u8 hsjp_ok;             // offset 0x430, size 0x1
    u8 high_jump_flag;      // offset 0x431, size 0x1
    s16 att_plus;           // offset 0x432, size 0x2
    s16 def_plus;           // offset 0x434, size 0x2
    s8 bs2_on_car;          // offset 0x436, size 0x1
    s8 bs2_area_car;        // offset 0x437, size 0x1
    s8 bs2_over_car;        // offset 0x438, size 0x1
    s8 bs2_area_car2;       // offset 0x439, size 0x1
    s8 bs2_over_car2;       // offset 0x43A, size 0x1
    u8 micchaku_wall_time;  // offset 0x43B, size 0x1
    u8 extra_jump;          // offset 0x43C, size 0x1
    u8 air_jump_ok_time;    // offset 0x43D, size 0x1
    s16 waku_ram_index;     // offset 0x43E, size 0x2
    u16 permited_koa;       // offset 0x440, size 0x2
    u8 ja_nmj_rno;          // offset 0x442, size 0x1
    u8 ja_nmj_cnt;          // offset 0x443, size 0x1
    u8 kind_of_blocking;    // offset 0x444, size 0x1
    u8 metamorphose;        // offset 0x445, size 0x1
    s16 metamor_index;      // offset 0x446, size 0x2
    u8 metamor_over;        // offset 0x448, size 0x1
    u8 gill_ccch_go;        // offset 0x449, size 0x1
    u8 renew_attchar;       // offset 0x44A, size 0x1
    s16 omop_vital_timer;   // offset 0x44C, size 0x2
    s16 sfwing_pos;         // offset 0x44E, size 0x2
    u8 init_E3_flag;        // offset 0x450, size 0x1
    u8 init_E4_flag;        // offset 0x451, size 0x1
    u16 pl09_dat_index;     // offset 0x452, size 0x2
    s16 reserv_add_y;       // offset 0x454, size 0x2
    u8 pt_free[20];         // offset 0x456, size 0x14
} PLW;

typedef struct {
    WORK wu;             // offset 0x0, size 0x388
    u32* my_master;      // offset 0x388, size 0x4
    s16 master_work_id;  // offset 0x38C, size 0x2
    s16 master_id;       // offset 0x38E, size 0x2
    s16 master_player;   // offset 0x390, size 0x2
    s16 master_priority; // offset 0x392, size 0x2
    u8 dm_refrect;       // offset 0x394, size 0x1
    u8 refrected;        // offset 0x395, size 0x1
    s16 free;            // offset 0x396, size 0x2
    u32 master_ng_flag;  // offset 0x398, size 0x4
    u32 master_ng_flag2; // offset 0x39C, size 0x4
    u8 et_free[30];      // offset 0x3A0, size 0x1E
} WORK_Other;

typedef struct {
    // total size: 0x8
    s16 nx;  // offset 0x0, size 0x2
    s16 ny;  // offset 0x2, size 0x2
    s16 col; // offset 0x4, size 0x2
    u16 chr; // offset 0x6, size 0x2
} CONN;

typedef struct {
    // total size: 0x6F8
    WORK wu;             // offset 0x0, size 0x388
    u32* my_master;      // offset 0x388, size 0x4
    s16 master_work_id;  // offset 0x38C, size 0x2
    s16 master_id;       // offset 0x38E, size 0x2
    s16 master_player;   // offset 0x390, size 0x2
    s16 master_priority; // offset 0x392, size 0x2
    s16 prio_reverse;    // offset 0x394, size 0x2
    s16 num_of_conn;     // offset 0x396, size 0x2
    CONN conn[108];      // offset 0x398, size 0x360
} WORK_Other_CONN;

typedef struct {
    // total size: 0x6F8
    WORK wu;             // offset 0x0, size 0x388
    u32* my_master;      // offset 0x388, size 0x4
    s16 master_work_id;  // offset 0x38C, size 0x2
    s16 master_id;       // offset 0x38E, size 0x2
    s16 master_player;   // offset 0x390, size 0x2
    s16 master_priority; // offset 0x392, size 0x2
    s8 look_up_flag;     // offset 0x394, size 0x1
    s8 curr_ja;          // offset 0x395, size 0x1
    u16 ja_disp_bit;     // offset 0x396, size 0x2
    u16 ja_color_bit;    // offset 0x398, size 0x2
    s16 fade_cja;        // offset 0x39A, size 0x2
    s16 ja[62][2];       // offset 0x39C, size 0xF8
    s16 jx[15][4];       // offset 0x494, size 0x78
} WORK_Other_JUDGE;

typedef struct {
    // total size: 0xC
    u8 order;    // offset 0x0, size 0x1
    u8 kind_req; // offset 0x1, size 0x1
    u8 kind_cnt; // offset 0x2, size 0x1
    u8 request;  // offset 0x3, size 0x1
    u8 contents; // offset 0x4, size 0x1
    u8 timer;    // offset 0x5, size 0x1
    s16 pos_x;   // offset 0x6, size 0x2
    s16 pos_y;   // offset 0x8, size 0x2
    s16 pos_z;   // offset 0xA, size 0x2
} MessageData;

typedef struct {
    // total size: 0x48
    s8 contents[10][7]; // offset 0x0, size 0x46
    u16 sum;            // offset 0x46, size 0x2
} SystemDir;

typedef struct {
    // total size: 0xC
    u8 Shot[8];   // offset 0x0, size 0x8
    u8 Vibration; // offset 0x8, size 0x1
    u8 free[3];   // offset 0x9, size 0x3
} _PAD_INFOR;

typedef struct {
    // total size: 0x20
    s8 contents[4][8]; // offset 0x0, size 0x20
} _EXTRA_OPTION;

typedef struct {
    // total size: 0x14
    u8 name[3];      // offset 0x0, size 0x3
    u16 player;      // offset 0x4, size 0x2
    u32 score;       // offset 0x8, size 0x4
    s8 cpu_grade;    // offset 0xC, size 0x1
    s8 grade;        // offset 0xD, size 0x1
    u16 wins;        // offset 0xE, size 0x2
    u8 player_color; // offset 0x10, size 0x1
    u8 all_clear;    // offset 0x11, size 0x1
} RANK_DATA;

struct _SAVE_W {
    // total size: 0x208
    _PAD_INFOR Pad_Infor[2];    // offset 0x0, size 0x18
    u8 Difficulty;              // offset 0x18, size 0x1
    s8 Time_Limit;              // offset 0x19, size 0x1
    u8 Battle_Number[2];        // offset 0x1A, size 0x2
    u8 Damage_Level;            // offset 0x1C, size 0x1
    u8 Handicap;                // offset 0x1D, size 0x1
    u8 Partner_Type[2];         // offset 0x1E, size 0x2
    s8 Adjust_X;                // offset 0x20, size 0x1
    s8 Adjust_Y;                // offset 0x21, size 0x1
    u8 Screen_Size;             // offset 0x22, size 0x1
    u8 Screen_Mode;             // offset 0x23, size 0x1
    u8 GuardCheck;              // offset 0x24, size 0x1
    u8 Auto_Save;               // offset 0x25, size 0x1
    u8 AnalogStick;             // offset 0x26, size 0x1
    u8 BgmType;                 // offset 0x27, size 0x1
    u8 SoundMode;               // offset 0x28, size 0x1
    u8 BGM_Level;               // offset 0x29, size 0x1
    u8 SE_Level;                // offset 0x2A, size 0x1
    u8 Extra_Option;            // offset 0x2B, size 0x1
    u8 PL_Color[2][20];         // offset 0x2C, size 0x28
    _EXTRA_OPTION extra_option; // offset 0x54, size 0x20
    RANK_DATA Ranking[20];      // offset 0x74, size 0x190
    u32 sum;                    // offset 0x204, size 0x4
};

struct _player_infor {
    // total size: 0x4
    u8 my_char;     // offset 0x0, size 0x1
    s8 sa;          // offset 0x1, size 0x1
    s8 color;       // offset 0x2, size 0x1
    s8 player_type; // offset 0x3, size 0x1
};

struct _REP_GAME_INFOR {
    // total size: 0x24
    struct _player_infor player_infor[2]; // offset 0x0, size 0x8
    s8 stage;                             // offset 0x8, size 0x1
    s8 Direction_Working;                 // offset 0x9, size 0x1
    s8 Vital_Handicap[2];                 // offset 0xA, size 0x2
    s16 Random_ix16;                      // offset 0xC, size 0x2
    s16 Random_ix32;                      // offset 0xE, size 0x2
    s16 Random_ix16_ex;                   // offset 0x10, size 0x2
    s16 Random_ix32_ex;                   // offset 0x12, size 0x2
    s8* fname;                            // offset 0x14, size 0x4
    u8 winner;                            // offset 0x18, size 0x1
    u8 play_type;                         // offset 0x19, size 0x1
    u16 players_timer;                    // offset 0x1A, size 0x2
    s16 old_mes_no2;                      // offset 0x1C, size 0x2
    s16 old_mes_no3;                      // offset 0x1E, size 0x2
    s16 old_mes_no_pl;                    // offset 0x20, size 0x2
    s16 mes_already;                      // offset 0x22, size 0x2
};

struct _MINI_SAVE_W {
    // total size: 0x3C
    _PAD_INFOR Pad_Infor[2];    // offset 0x0, size 0x18
    s8 Time_Limit;              // offset 0x18, size 0x1
    u8 Battle_Number[2];        // offset 0x19, size 0x2
    u8 Damage_Level;            // offset 0x1B, size 0x1
    _EXTRA_OPTION extra_option; // offset 0x1C, size 0x20
};

typedef struct {
    // total size: 0x48
    u8 Handle_1P[18];   // offset 0x0, size 0x12
    u8 Id_1P[8];        // offset 0x12, size 0x8
    u8 Handle_2P[18];   // offset 0x1A, size 0x12
    u8 Id_2P[8];        // offset 0x2C, size 0x8
    u8 Serial_No[6];    // offset 0x34, size 0x6
    u8 Battle_Code[14]; // offset 0x3A, size 0xE
} _NET_INFOR;

typedef struct {
    // total size: 0x7448
    u8 header[640];                    // offset 0x0, size 0x280
    u8 sega_reserve[64];               // offset 0x280, size 0x40
    s16 Control_Time_Buff;             // offset 0x2C0, size 0x2
    u8 Difficulty;                     // offset 0x2C2, size 0x1
    u8 Monitor_Type;                   // offset 0x2C3, size 0x1
    u8 free_free[4];                   // offset 0x2C4, size 0x4
    struct _REP_GAME_INFOR game_infor; // offset 0x2C8, size 0x24
    struct _MINI_SAVE_W mini_save_w;   // offset 0x2EC, size 0x3C
    SystemDir system_dir;              // offset 0x328, size 0x48
    _NET_INFOR net_infor;              // offset 0x370, size 0x48
    struct {
        // total size: 0x7078
        u16 key_buff[2][7198]; // offset 0x0, size 0x7078
    } io_unit;                 // offset 0x3B8, size 0x7078
    u8 lag[14];                // offset 0x7430, size 0xE
    u32 sum;                   // offset 0x7440, size 0x4
    u8 champion;               // offset 0x7444, size 0x1
    u8 full_data;              // offset 0x7445, size 0x1
    u8 free;                   // offset 0x7446, size 0x1
    u8 extra_free;             // offset 0x7447, size 0x1
} _REPLAY_W;

struct _cursor_infor {
    // total size: 0x2
    u8 first_x; // offset 0x0, size 0x1
    u8 first_y; // offset 0x1, size 0x1
};

typedef struct {
    // total size: 0x18
    u8 ok[20];                            // offset 0x0, size 0x14
    struct _cursor_infor cursor_infor[2]; // offset 0x14, size 0x4
} Permission;

typedef struct {
    // total size: 0x44
    u16 sw_new;     // offset 0x0, size 0x2
    u16 sw_old;     // offset 0x2, size 0x2
    u16 sw_chg;     // offset 0x4, size 0x2
    u16 sw_now;     // offset 0x6, size 0x2
    u16 old_now;    // offset 0x8, size 0x2
    u16 now_lvbt;   // offset 0xA, size 0x2
    u16 old_lvbt;   // offset 0xC, size 0x2
    u16 new_lvbt;   // offset 0xE, size 0x2
    u16 sw_lever;   // offset 0x10, size 0x2
    u16 shot_up;    // offset 0x12, size 0x2
    u16 shot_down;  // offset 0x14, size 0x2
    u16 shot_ud;    // offset 0x16, size 0x2
    s16 lvr_status; // offset 0x18, size 0x2
    s16 jaku_cnt;   // offset 0x1A, size 0x2
    s16 chuu_cnt;   // offset 0x1C, size 0x2
    s16 kyou_cnt;   // offset 0x1E, size 0x2
    s16 up_cnt;     // offset 0x20, size 0x2
    s16 down_cnt;   // offset 0x22, size 0x2
    s16 left_cnt;   // offset 0x24, size 0x2
    s16 right_cnt;  // offset 0x26, size 0x2
    s16 s1_cnt;     // offset 0x28, size 0x2
    s16 s2_cnt;     // offset 0x2A, size 0x2
    s16 s3_cnt;     // offset 0x2C, size 0x2
    s16 s4_cnt;     // offset 0x2E, size 0x2
    s16 s5_cnt;     // offset 0x30, size 0x2
    s16 s6_cnt;     // offset 0x32, size 0x2
    s16 lu_cnt;     // offset 0x34, size 0x2
    s16 ld_cnt;     // offset 0x36, size 0x2
    s16 ru_cnt;     // offset 0x38, size 0x2
    s16 rd_cnt;     // offset 0x3A, size 0x2
    s16 waza_num;   // offset 0x3C, size 0x2
    s16 waza_no;    // offset 0x3E, size 0x2
    s16 wait_cnt;   // offset 0x40, size 0x2
    s16 cmd_r_no;   // offset 0x42, size 0x2
} T_PL_LVR;

typedef struct /* @anon31 */ {
    // total size: 0x1C
    s16 w_type;  // offset 0x0, size 0x2
    s16 w_int;   // offset 0x2, size 0x2
    s16 free1;   // offset 0x4, size 0x2
    s16 w_lvr;   // offset 0x6, size 0x2
    s16* w_ptr;  // offset 0x8, size 0x4
    s16 free2;   // offset 0xC, size 0x2
    s16 w_dead;  // offset 0xE, size 0x2
    s16 w_dead2; // offset 0x10, size 0x2
    union {
        struct {
            // total size: 0x6
            s16 flag;       // offset 0x0, size 0x2
            s16 shot_flag;  // offset 0x2, size 0x2
            s16 shot_flag2; // offset 0x4, size 0x2
        } tame;             // offset 0x0, size 0x6
        struct {
            // total size: 0x6
            s16 s_cnt; // offset 0x0, size 0x2
            s16 m_cnt; // offset 0x2, size 0x2
            s16 l_cnt; // offset 0x4, size 0x2
        } shot;        // offset 0x0, size 0x6
    } uni0;            // offset 0x12, size 0x6
    s16 free3;         // offset 0x18, size 0x2
    s16 shot_ok;       // offset 0x1A, size 0x2
} WAZA_WORK;

typedef struct {
    // total size: 0x26
    s16 timer;       // offset 0x0, size 0x2
    s16 jmplv;       // offset 0x2, size 0x2
    s16 kosuu;       // offset 0x4, size 0x2
    s16 bbdat[4][4]; // offset 0x6, size 0x20
} BBBSTable;

typedef union {
    s32 patl;    // offset 0x0, size 0x4
    LoHi16 pats; // offset 0x0, size 0x4
} SST;

typedef union {
    s32 l;    // offset 0x0, size 0x4
    LoHi16 w; // offset 0x0, size 0x4
} ST;

typedef struct {
    // total size: 0x4
    u16 bg_h_shift; // offset 0x0, size 0x2
    u16 bg_v_shift; // offset 0x2, size 0x2
} BackgroundParameters;

union POS_FLOAT {
    s32 long_pos;    // offset 0x0, size 0x4
    LoHi16 word_pos; // offset 0x0, size 0x4
};

typedef struct {
    // total size: 0x10
    union POS_FLOAT scr_x;      // offset 0x0, size 0x4
    union POS_FLOAT scr_x_buff; // offset 0x4, size 0x4
    union POS_FLOAT scr_y;      // offset 0x8, size 0x4
    union POS_FLOAT scr_y_buff; // offset 0xC, size 0x4
} BG_POS;

typedef struct {
    // total size: 0x4C
    s16 offence_total;  // offset 0x0, size 0x2
    s16 defence_total;  // offset 0x2, size 0x2
    s16 tech_pts_total; // offset 0x4, size 0x2
    s16 ex_point_total; // offset 0x6, size 0x2
    s16 em_stun;        // offset 0x8, size 0x2
    s16 max_combo;      // offset 0xA, size 0x2
    s16 clean_hits;     // offset 0xC, size 0x2
    s16 att_renew;      // offset 0xE, size 0x2
    s16 guard_succ;     // offset 0x10, size 0x2
    s16 vitality;       // offset 0x12, size: 0x2
    s16 nml_blocking;   // offset 0x14, size: 0x2
    s16 rpd_blocking;   // offset 0x16, size: 0x2
    s16 grd_blocking;   // offset 0x18, size: 0x2
    s16 def_free;       // offset 0x1A, size: 0x2
    s16 first_attack;   // offset 0x1C, size: 0x2
    s16 leap_attack;    // offset 0x1E, size: 0x2
    s16 target_combo;   // offset 0x20, size: 0x2
    s16 nml_nage;       // offset 0x22, size: 0x2
    s16 grap_def;       // offset 0x24, size: 0x2
    s16 quick_stand;    // offset 0x26, size: 0x2
    s16 personal_act;   // offset 0x28, size: 0x2
    s16 reversal;       // offset 0x2A, size: 0x2
    s16 comwaza;        // offset 0x2C, size: 0x2
    s16 sa_exec;        // offset 0x2E, size: 0x2
    s16 tairyokusa;     // offset 0x30, size: 0x2
    s16 kimarite;       // offset 0x32, size: 0x2
    s16 renshou;        // offset 0x34, size: 0x2
    s16 em_renshou;     // offset 0x36, size: 0x2
    s16 app_nml_block;  // offset 0x38, size: 0x2
    s16 app_rpd_block;  // offset 0x3A, size: 0x2
    s16 app_grd_block;  // offset 0x3C, size: 0x2
    s16 onaji_waza;     // offset 0x3E, size: 0x2
    s16 grd_miss;       // offset 0x40, size: 0x2
    s16 grd_mcnt;       // offset 0x42, size: 0x2
    s16 grade;          // offset 0x44, size: 0x2
    s16 round;          // offset 0x46, size: 0x2
    s16 win_round;      // offset 0x48, size: 0x2
    s16 no_lose;        // offset 0x4A, size: 0x2
} GradeData;

typedef struct {
    // total size: 0xAC
    s16 vs_cpu_result[16];  // offset 0x0, size 0x20
    s16 vs_cpu_grade[16];   // offset 0x20, size 0x20
    s16 vs_cpu_player[16];  // offset 0x40, size 0x20
    s16 vcr_ix;             // offset 0x60, size 0x2
    s16 grade;              // offset 0x62, size 0x2
    s16 all_clear;          // offset 0x64, size 0x2
    s16 keizoku;            // offset 0x66, size 0x2
    s16 sp_point;           // offset 0x68, size 0x2
    s16 fr_ix;              // offset 0x6A, size 0x2
    u8 fr_sort_data[16][4]; // offset 0x6C, size 0x40
} GradeFinalData;

typedef struct {
    // total size: 0xA
    s16 offence_total;  // offset 0x0, size 0x2
    s16 defence_total;  // offset 0x2, size 0x2
    s16 tech_pts_total; // offset 0x4, size 0x2
    s16 ex_point_total; // offset 0x6, size 0x2
    s16 grade;          // offset 0x8, size 0x2
} JudgeGals;

typedef struct {
    // total size: 0xC
    s16 offence_total;  // offset 0x0, size 0x2
    s16 defence_total;  // offset 0x2, size 0x2
    s16 tech_pts_total; // offset 0x4, size 0x2
    s16 ex_point_total; // offset 0x6, size 0x2
    s16 round;          // offset 0x8, size 0x2
    s16 grade;          // offset 0xA, size 0x2
} JudgeCom;

typedef struct {
    // total size: 0x10
    s16 pos_x;  // offset 0x0, size 0x2
    s16 pos_y;  // offset 0x2, size 0x2
    s16 pos_z;  // offset 0x4, size 0x2
    u16 cg_num; // offset 0x6, size 0x2
    s16 renew;  // offset 0x8, size 0x2
    u16 hit_ix; // offset 0xA, size 0x2
    s8 flip;    // offset 0xC, size 0x1
    u8 cg_flp;  // offset 0xD, size 0x1
    s16 kowaza; // offset 0xE, size 0x2
} ZanzouTableEntry;

typedef union {
    s32 pl;    // offset 0x0, size 0x4
    LoHi16 ps; // offset 0x0, size 0x4
} S32Split;

typedef struct {
    // total size: 0x2C
    s32 timer;  // offset 0x0, size 0x4
    s32 timer2; // offset 0x4, size 0x4
    S32Split x; // offset 0x8, size 0x4
    S32Split y; // offset 0xC, size 0x4
    s32 spx;    // offset 0x10, size 0x4
    s32 dlx;    // offset 0x14, size 0x4
    s32 spy;    // offset 0x18, size 0x4
    s32 dly;    // offset 0x1C, size 0x4
    s32 amx;    // offset 0x20, size 0x4
    s32 amy;    // offset 0x24, size 0x4
    s8 swx;     // offset 0x28, size 0x1
    s8 swy;     // offset 0x29, size 0x1
} MotionState;

typedef struct {
    // total size: 0x10
    s8 contents[2][2][4]; // offset 0x0, size 0x10
} TrainingData;

typedef union {
    s32 psi;    // offset 0x0, size 0x4
    LoHi16 pss; // offset 0x0, size 0x4
} MS;

typedef union {
    s32 dy;    // offset 0x0, size 0x4
    LoHi16 ry; // offset 0x0, size 0x4
} PS_DY;

typedef union {
    s32 psy;     // offset 0x0, size 0x4
    LoHi16 psys; // offset 0x0, size 0x4
} PS_UNI;

typedef union {
    s32 dp;    // offset 0x0, size 0x4
    LoHi16 rp; // offset 0x0, size 0x4
} PS_DP;

typedef struct {
    // total size: 0x30
    s16 data[4][6]; // offset 0x0, size 0x30
} UNK_Data;

typedef struct {
    // total size: 0x64
    u32* nmca;        // offset 0x0, size 0x4
    u32* dmca;        // offset 0x4, size 0x4
    u32* btca;        // offset 0x8, size 0x4
    u32* caca;        // offset 0xC, size 0x4
    u32* cuca;        // offset 0x10, size 0x4
    u32* atca;        // offset 0x14, size 0x4
    u32* saca;        // offset 0x18, size 0x4
    u32* exca;        // offset 0x1C, size 0x4
    u32* cbca;        // offset 0x20, size 0x4
    u32* yuca;        // offset 0x24, size 0x4
    s16* stxy;        // offset 0x28, size 0x4
    s16* mvxy;        // offset 0x2C, size 0x4
    u32* sernd;       // offset 0x30, size 0x4
    UNK_8* ovct;      // offset 0x34, size 0x4
    UNK_9* ovix;      // offset 0x38, size 0x4
    CatchTable* rict; // offset 0x3C, size 0x4
    UNK_0* hiit;      // offset 0x40, size 0x4
    UNK_1* boda;      // offset 0x44, size 0x4
    UNK_2* hana;      // offset 0x48, size 0x4
    UNK_3* cata;      // offset 0x4C, size 0x4
    UNK_4* caua;      // offset 0x50, size 0x4
    UNK_5* atta;      // offset 0x54, size 0x4
    UNK_6* hosa;      // offset 0x58, size 0x4
    UNK_7* atit;      // offset 0x5C, size 0x4
    UNK_Data* prot;   // offset 0x60, size 0x4
} CharInitData;

typedef struct {
    // total size: 0x8
    s16 my_cm; // offset 0x0, size 0x2
    s16 my_cc; // offset 0x2, size 0x2
    s16 my_pr; // offset 0x4, size 0x2
    s16 my_fm; // offset 0x6, size 0x2
} CharInitData2;

typedef struct {
    // total size: 0xC
    uintptr_t adr;  // offset 0x0, size 0x4
    size_t size;    // offset 0x4, size 0x4
    u8 search_type; // offset 0x8, size 0x1
    u8 group_num;   // offset 0x9, size 0x1
    u8 type;        // offset 0xA, size 0x1
    u8 use;         // offset 0xB, size 0x1
} RCKeyWork;

typedef struct {
    // total size: 0x32
    s8 type;         // offset 0x0, size 0x1
    s8 form;         // offset 0x1, size 0x1
    s8 end_flag[4];  // offset 0x2, size 0x4
    s8 dmm;          // offset 0x6, size 0x1
    s16 id;          // offset 0x8, size 0x2
    s16 r_no_0;      // offset 0xA, size 0x2
    s16 r_no_1;      // offset 0xC, size 0x2
    u8 rank_in;      // offset 0xE, size 0x1
    s16 rank_status; // offset 0x10, size 0x2
    s16 rank;        // offset 0x12, size 0x2
    s16 status;      // offset 0x14, size 0x2
    s16 index;       // offset 0x16, size 0x2
    s16 timer;       // offset 0x18, size 0x2
    s16 code[4];     // offset 0x1A, size 0x8
    s16 old_code[4]; // offset 0x22, size 0x8
    s16 count1[2];   // offset 0x2A, size 0x4
    s8 count2[2];    // offset 0x2E, size 0x2
    s16 wait_cnt;    // offset 0x30, size 0x2
} NAME_WK;

typedef struct {
    // total size: 0xC
    s8 type;           // offset 0x0, size 0x1
    s8 n_disp_flag;    // offset 0x1, size 0x1
    s16 c_cnt;         // offset 0x2, size 0x2
    s16 r_no_0;        // offset 0x4, size 0x2
    s16 r_no_1;        // offset 0x6, size 0x2
    s16 f_cnt;         // offset 0x8, size 0x2
    u8 tenmetsu_flag;  // offset 0xA, size 0x1
    u8 tenmetsu_place; // offset 0xB, size 0x1
} SC_NAME_WK;

typedef struct {
    // total size: 0x4
    s8 code[4]; // offset 0x0, size 0x4
} RANK_NAME_W;

typedef struct {
    // total size: 0x14
    s8 be;        // offset 0x0, size 0x1
    u8 c_mode;    // offset 0x1, size 0x1
    u16 total;    // offset 0x2, size 0x2
    u16* handle;  // offset 0x4, size 0x4
    s32 ixNum1st; // offset 0x8, size 0x4
    u8* srcAdrs;  // offset 0xC, size 0x4
    u32 srcSize;  // offset 0x10, size 0x4
} Palette;

typedef union {
    u32 b32;    // offset 0x0, size 0x4
    u16 b16[2]; // offset 0x0, size 0x4
    u8 b8[4];   // offset 0x0, size 0x4
} TextureHandle;

typedef struct {
    // total size: 0x20
    s8 be;                 // offset 0x0, size 0x1
    u8 flags;              // offset 0x1, size 0x1
    s16 arCnt;             // offset 0x2, size 0x2
    s16 arInit;            // offset 0x4, size 0x2
    u16 total;             // offset 0x6, size 0x2
    TextureHandle* handle; // offset 0x8, size 0x4
    s32 ixNum1st;          // offset 0xC, size 0x4
    u16 textures;          // offset 0x10, size 0x2
    u16 accnum;            // offset 0x12, size 0x2
    u32* offset;           // offset 0x14, size 0x4
    u8* srcAdrs;           // offset 0x18, size 0x4
    size_t srcSize;        // offset 0x1C, size 0x4
} Texture;

struct _VM_W {
    // total size: 0x6C
    u8 r_no[4];     // offset 0x0, size 0x4
    u8 r_sub[4];    // offset 0x4, size 0x4
    s32 Timer;      // offset 0x8, size 0x4
    s32 FreeMem[2]; // offset 0xC, size 0x8
    s8 Format[2];   // offset 0x14, size 0x2
    s8 Find[2];     // offset 0x16, size 0x2
    s8 Connect[2];  // offset 0x18, size 0x2
    s8 CheckDrive;  // offset 0x1A, size 0x1
    s8 AutoDrive;   // offset 0x1B, size 0x1
    s8 Drive;       // offset 0x1C, size 0x1
    s8 Access;      // offset 0x1D, size 0x1
    s8 Request;     // offset 0x1E, size 0x1
    s8 AutoLoaded;  // offset 0x1F, size 0x1
    struct {
        // total size: 0x8
        u16 year;      // offset 0x0, size 0x2
        u8 month;      // offset 0x2, size 0x1
        u8 day;        // offset 0x3, size 0x1
        u8 hour;       // offset 0x4, size 0x1
        u8 minute;     // offset 0x5, size 0x1
        u8 second;     // offset 0x6, size 0x1
        u8 dayofweek;  // offset 0x7, size 0x1
    } curTime[2];      // offset 0x20, size 0x10
    s32 curSize[2];    // offset 0x30, size 0x8
    s16 memKey;        // offset 0x38, size 0x2
    u8* memAdr;        // offset 0x3C, size 0x4
    s32 nowResult;     // offset 0x40, size 0x4
    s32 nowNumber;     // offset 0x44, size 0x4
    s32 polResult;     // offset 0x48, size 0x4
    s32 polNumber;     // offset 0x4C, size 0x4
    u8 File_Type;      // offset 0x50, size 0x1
    u8* File_Name;     // offset 0x54, size 0x4
    u32 Save_Size;     // offset 0x58, size 0x4
    u16 Block_Size;    // offset 0x5C, size 0x2
    u8 Icon_Type;      // offset 0x5E, size 0x1
    u8 Comment_Type;   // offset 0x5F, size 0x1
    u8 Target_Number;  // offset 0x60, size 0x1
    u8 Number;         // offset 0x61, size 0x1
    u8 Counter;        // offset 0x62, size 0x1
    u8 Save_Type;      // offset 0x63, size 0x1
    s8 New_File;       // offset 0x64, size 0x1
    s8 Header_Counter; // offset 0x65, size 0x1
    s8 padding[3];     // offset 0x66, size 0x3
};

typedef struct {
    u8 unit;
    u8 flag;
    s8 power;
    u8 freq;
} PULPARA;

typedef struct {
    // total size: 0x90
    s8 be_flag;        // offset 0x0, size 0x1
    s8 disp_flag;      // offset 0x1, size 0x1
    s16 fam_no;        // offset 0x2, size 0x2
    s16 r_no_0;        // offset 0x4, size 0x2
    s16 r_no_1;        // offset 0x6, size 0x2
    s16 r_no_2;        // offset 0x8, size 0x2
    s16 position_x;    // offset 0xA, size 0x2
    s16 position_y;    // offset 0xC, size 0x2
    s32 speed_x;       // offset 0x10, size 0x4
    s32 speed_y;       // offset 0x14, size 0x4
    XY xy[2];          // offset 0x18, size 0x8
    XY wxy[2];         // offset 0x20, size 0x8
    u16* bg_address;   // offset 0x28, size 0x4
    u16* suzi_adrs;    // offset 0x2C, size 0x4
    s16 old_pos_x;     // offset 0x30, size 0x2
    s32 zuubun;        // offset 0x34, size 0x4
    s16 no_suzi_line;  // offset 0x38, size 0x2
    u16* start_suzi;   // offset 0x3C, size 0x4
    s16 u_line;        // offset 0x40, size 0x2
    s16 d_line;        // offset 0x42, size 0x2
    s16 bg_adrs_c_no;  // offset 0x44, size 0x2
    s16 suzi_c_no;     // offset 0x46, size 0x2
    s16 pos_x_work;    // offset 0x48, size 0x2
    s16 pos_y_work;    // offset 0x4A, size 0x2
    s8 rewrite_flag;   // offset 0x4C, size 0x1
    s8 suzi_base_flag; // offset 0x4D, size 0x1
    XY hos_xy[2];      // offset 0x50, size 0x8
    XY chase_xy[2];    // offset 0x58, size 0x8
    s16 free;          // offset 0x60, size 0x2
    s16 frame_deff;    // offset 0x62, size 0x2
    s16 r_limit;       // offset 0x64, size 0x2
    s16 r_limit2;      // offset 0x66, size 0x2
    s16 l_limit;       // offset 0x68, size 0x2
    s16 l_limit2;      // offset 0x6A, size 0x2
    s16 y_limit;       // offset 0x6C, size 0x2
    s16 y_limit2;      // offset 0x6E, size 0x2
    u16* suzi_adrs2;   // offset 0x70, size 0x4
    u16* start_suzi2;  // offset 0x74, size 0x4
    s16 suzi_c_no2;    // offset 0x78, size 0x2
    s32 max_x_limit;   // offset 0x7C, size 0x4
    s16* deff_rl;      // offset 0x80, size 0x4
    s16* deff_plus;    // offset 0x84, size 0x4
    s16* deff_minus;   // offset 0x88, size 0x4
    s16 abs_x;         // offset 0x8C, size 0x2
    s16 abs_y;         // offset 0x8E, size 0x2
} BGW;

typedef struct {
    // total size: 0x428
    s8 bg_routine;     // offset 0x0, size 0x1
    s8 bg_r_1;         // offset 0x1, size 0x1
    s8 bg_r_2;         // offset 0x2, size 0x1
    s8 stage;          // offset 0x3, size 0x1
    s8 area;           // offset 0x4, size 0x1
    s8 compel_flag;    // offset 0x5, size 0x1
    s32 scroll_cg_adr; // offset 0x8, size 0x4
    s32 ake_cg_adr;    // offset 0xC, size 0x4
    u8 scno;           // offset 0x10, size 0x1
    u8 scrno;          // offset 0x11, size 0x1
    s16 bg2_sp_x;      // offset 0x12, size 0x2
    s16 bg2_sp_y;      // offset 0x14, size 0x2
    s16 scr_stop;      // offset 0x16, size 0x2
    s8 frame_flag;     // offset 0x18, size 0x1
    s8 chase_flag;     // offset 0x19, size 0x1
    s8 old_chase_flag; // offset 0x1A, size 0x1
    s8 old_frame_flag; // offset 0x1B, size 0x1
    s16 pos_offset;    // offset 0x1C, size 0x2
    s16 quake_x_index; // offset 0x1E, size 0x2
    s16 quake_y_index; // offset 0x20, size 0x2
    s16 bg_f_x;        // offset 0x22, size 0x2
    s16 bg_f_y;        // offset 0x24, size 0x2
    s16 bg2_sp_x2;     // offset 0x26, size 0x2
    s16 bg2_sp_y2;     // offset 0x28, size 0x2
    s16 frame_deff;    // offset 0x2A, size 0x2
    s16 center_x;      // offset 0x2C, size 0x2
    s16 center_y;      // offset 0x2E, size 0x2
    s16 bg_index;      // offset 0x30, size 0x2
    s8 frame_vol;      // offset 0x32, size 0x1
    s16 max_x;         // offset 0x34, size 0x2
    u8 bg_opaque;      // offset 0x36, size 0x1
    BGW bgw[7];        // offset 0x38, size 0x3F0
} BG;

typedef struct {
    // total size: 0x10
    union {
        u16 results; // offset 0x0, size 0x2
        struct {
            // total size: 0x2
            s8 att_result; // offset 0x0, size 0x1
            s8 cat_result; // offset 0x1, size 0x1
        } ca;              // offset 0x0, size 0x2
    } flag;                // offset 0x0, size 0x2
    u8 my_att;             // offset 0x2, size 0x1
    u8 dm_body;            // offset 0x3, size 0x1
    u16 my_hit;            // offset 0x4, size 0x2
    u16 dm_me;             // offset 0x6, size 0x2
    s16* ah;               // offset 0x8, size 0x4
    s16* dh;               // offset 0xC, size 0x4
} HS;

typedef struct /* @anon14 */ {
    // total size: 0x4
    s16 ix;    // offset 0x0, size 0x2
    s16 timer; // offset 0x2, size 0x2
} PPWORK_SUB_SUB;

typedef struct {
    u8 ppnew;
    u8 free;
    s16 data;
    s16 rno[4];
    s16 life;
    s16 exix;
    const PPWORK_SUB_SUB* padr;
} PPWORK_SUB;

typedef struct {
    // total size: 0x34
    u8 ok_dev;       // offset 0x0, size 0x1
    u8 id;           // offset 0x1, size 0x1
    u8 opck;         // offset 0x2, size 0x1
    u8 psix;         // offset 0x3, size 0x1
    s16 vital;       // offset 0x4, size 0x2
    u8 inStop;       // offset 0x6, size 0x1
    u8 free;         // offset 0x7, size 0x1
    u32 port;        // offset 0x8, size 0x4
    PPWORK_SUB p[2]; // offset 0xC, size 0x28
} PPWORK;

typedef struct {
    s16 prio;
    s16 rno;
    const PPWORK_SUB_SUB* adrs;
} PULREQ;

typedef struct {
    // total size: 0x48
    s8 contents[10][7]; // offset 0x0, size 0x46
    u16 sum;            // offset 0x46, size 0x2
} DirData;

typedef struct _anon6 {
    s8 xxxx[8][2][32];
} _anon6;

typedef struct _anon13 {
    s8 zzzz[4][4];
} _anon13;

typedef struct {
    // total size: 0x8
    s16 ptix; // offset 0x0, size 0x2
    s16 bank; // offset 0x2, size 0x2
    s16 port; // offset 0x4, size 0x2
    s16 code; // offset 0x6, size 0x2
} SoundPatchConfig;

typedef struct {
    // total size: 0xC
    u16 cp3code;          // offset 0x0, size 0x2
    u16 free;             // offset 0x2, size 0x2
    SoundPatchConfig rmc; // offset 0x4, size 0x8
} SoundPatch;             // Tentative name

typedef struct {
    // total size: 0x8
    union {
        s32 cal; // offset 0x0, size 0x4
        struct {
            // total size: 0x4
            u16 low; // offset 0x0, size 0x2
            s16 pos; // offset 0x2, size 0x2
        } disp;      // offset 0x0, size 0x4
    } iw[2];         // offset 0x0, size 0x8
} Ideal_W;

typedef struct {
    // total size: 0x14
    u8 name[3];      // offset 0x0, size 0x3
    u16 player;      // offset 0x4, size 0x2
    u32 score;       // offset 0x8, size 0x4
    s8 cpu_grade;    // offset 0xC, size 0x1
    s8 grade;        // offset 0xD, size 0x1
    u16 wins;        // offset 0xE, size 0x2
    u8 player_color; // offset 0x10, size 0x1
    u8 all_clear;    // offset 0x11, size 0x1
} ScoreRankingEntry;

typedef struct {
    // total size: 0xC
    f32 x; // offset 0x0, size 0x4
    f32 y; // offset 0x4, size 0x4
    f32 z; // offset 0x8, size 0x4
} Vec3;

typedef struct {
    // total size: 0x10
    f32 x; // offset 0x0, size 0x4
    f32 y; // offset 0x4, size 0x4
    f32 z; // offset 0x8, size 0x4
    f32 w; // offset 0xC, size 0x4
} Vec4;

typedef struct {
    // total size: 0x8
    s16 x1; // offset 0x0, size 0x2
    s16 y1; // offset 0x2, size 0x2
    s16 x2; // offset 0x4, size 0x2
    s16 y2; // offset 0x6, size 0x2
} Rect;

typedef struct {
    // total size: 0xC
    s8 ok;                   // offset 0x0, size 0x1
    s8 type;                 // offset 0x1, size 0x1
    s16 key;                 // offset 0x2, size 0x2
    uintptr_t texture_table; // offset 0x4, size 0x4
    uintptr_t trans_table;   // offset 0x8, size 0x4
} TEX_GRP_LD;

typedef struct {
    // total size: 0xB0
    Palette palDC;  // offset 0x0, size 0x14
    Palette palCP3; // offset 0x14, size 0x14
    s16 req[32][2]; // offset 0x28, size 0x80
    s16 reqNum;     // offset 0xA8, size 0x2
    u32 upBits;     // offset 0xAC, size 0x4
} Col3rd_W;

typedef struct {
    // total size: 0x28
    u8 be;           // offset 0x0, size 0x1
    u8 type;         // offset 0x1, size 0x1
    s16 id;          // offset 0x2, size 0x2
    u8 rno;          // offset 0x4, size 0x1
    u8 retry;        // offset 0x5, size 0x1
    u8 ix;           // offset 0x6, size 0x1
    u8 frre;         // offset 0x7, size 0x1
    s16 key;         // offset 0x8, size 0x2
    u8 kokey;        // offset 0xA, size 0x1
    u8 group;        // offset 0xB, size 0x1
    u8* result;      // offset 0xC, size 0x4
    s32 size;        // offset 0x10, size 0x4
    s32 sect;        // offset 0x14, size 0x4
    u16 fnum;        // offset 0x18, size 0x2
    u8 free[2];      // offset 0x1A, size 0x2
    TEX_GRP_LD* lds; // offset 0x1C, size 0x4
    struct {
        // total size: 0x8
        u32 number; // offset 0x0, size 0x4
        u32 size;   // offset 0x4, size 0x4
    } info;         // offset 0x20, size 0x8
} REQ;

typedef struct {
    // total size: 0x40
    f32 _11; // offset 0x0, size 0x4
    f32 _12; // offset 0x4, size 0x4
    f32 _13; // offset 0x8, size 0x4
    f32 _14; // offset 0xC, size 0x4
    f32 _21; // offset 0x10, size 0x4
    f32 _22; // offset 0x14, size 0x4
    f32 _23; // offset 0x18, size 0x4
    f32 _24; // offset 0x1C, size 0x4
    f32 _31; // offset 0x20, size 0x4
    f32 _32; // offset 0x24, size 0x4
    f32 _33; // offset 0x28, size 0x4
    f32 _34; // offset 0x2C, size 0x4
    f32 _41; // offset 0x30, size 0x4
    f32 _42; // offset 0x34, size 0x4
    f32 _43; // offset 0x38, size 0x4
    f32 _44; // offset 0x3C, size 0x4
} Matrix;

typedef union {
    f32 f[16];   // offset 0x0, size 0x40
    f32 a[4][4]; // offset 0x0, size 0x40
    Matrix m;    // offset 0x0, size 0x40
} MTX;

typedef struct {
    float x, y, z;        // 12 bytes
    short u, v;           // 4 bytes
    u32 col;              // 4 bytes
} Polygon;

typedef struct {
    // total size: 0x8
    f32 x; // offset 0x0, size 0x4
    f32 y; // offset 0x4, size 0x4
} PAL_CURSOR_P;

typedef union {
    u32 color; // offset 0x0, size 0x4
    struct {
        // total size: 0x4
        s16 u; // offset 0x0, size 0x2
        s16 v; // offset 0x2, size 0x2
    } tex;     // offset 0x0, size 0x4
    struct {
        // total size: 0x4
        u8 b; // offset 0x0, size 0x1
        u8 g; // offset 0x1, size 0x1
        u8 r; // offset 0x2, size 0x1
        u8 a; // offset 0x3, size 0x1
    } argb;   // offset 0x0, size 0x4
} PAL_CURSOR_COL;

typedef struct {
    // total size: 0x30
    PAL_CURSOR_P pal_cursor_p[4];     // offset 0x0, size 0x20
    PAL_CURSOR_COL pal_cursor_col[4]; // offset 0x20, size 0x10
} PAL_CURSOR_TBL;

typedef struct {
    // total size: 0x10
    PAL_CURSOR_P* p;     // offset 0x0, size 0x4
    PAL_CURSOR_COL* col; // offset 0x4, size 0x4
    PAL_CURSOR_COL* tex; // offset 0x8, size 0x4
    u32 num;             // offset 0xC, size 0x4
} PAL_CURSOR;

typedef union {
    s32 full; // offset 0x0, size 0x4
    struct {
        // total size: 0x4
        u8 B; // offset 0x0, size 0x1
        u8 G; // offset 0x1, size 0x1
        u8 R; // offset 0x2, size 0x1
        u8 A; // offset 0x3, size 0x1
    } rgb;    // offset 0x0, size 0x4
} OPTW_Color;

typedef struct {
    // total size: 0x20
    u32 g_no;       // offset 0x0, size 0x4
    u16 hv;         // offset 0x4, size 0x2
    s16 off_x;      // offset 0x6, size 0x2
    s16 off_y;      // offset 0x8, size 0x2
    f32 zx;         // offset 0xC, size 0x4
    f32 zy;         // offset 0x10, size 0x4
    s32 prio;       // offset 0x14, size 0x4
    s32 trans;      // offset 0x18, size 0x4
    OPTW_Color col; // offset 0x1C, size 0x4
} OPTW;

typedef struct {
    // total size: 0x10
    u32 g_no;       // offset 0x0, size 0x4
    s32 trans;      // offset 0x4, size 0x4
    u16 hv;         // offset 0x8, size 0x2
    s16 ok;         // offset 0xA, size 0x2
    OPTW_Color col; // offset 0xC, size 0x4
} OPTW_Small;

typedef struct {
    // total size: 0x10C
    s8 r_no_0;            // offset 0x0, size 0x1
    s8 r_no_1;            // offset 0x1, size 0x1
    s8 dir;               // offset 0x2, size 0x1
    s8 ctr;               // offset 0x3, size 0x1
    s16 bg_no;            // offset 0x4, size 0x2
    u16 blk_no;           // offset 0x6, size 0x2
    s32 prio;             // offset 0x8, size 0x4
    OPTW_Small map[4][4]; // offset 0xC, size 0x100
} OPBW;

typedef struct {
    // total size: 0x330
    s8 r_no_0;     // offset 0x0, size 0x1
    s8 r_no_1;     // offset 0x1, size 0x1
    s8 r_no_2;     // offset 0x2, size 0x1
    s8 old_rno;    // offset 0x3, size 0x1
    s16 index;     // offset 0x4, size 0x2
    s16 mv_ctr;    // offset 0x6, size 0x2
    s16 free_work; // offset 0x8, size 0x2
    s16 dummy;     // offset 0xA, size 0x2
    OPBW bgw[3];   // offset 0xC, size 0x324
} OP_W;

typedef struct {
    // total size: 0xD0
    u16 x16_map[4][16]; // offset 0x0, size 0x80
    u8 x32_map[10][8];  // offset 0x80, size 0x50
} PatternMap;

typedef struct {
    Texture* texture;      // pointer to PSP texture
    
    int frameCount;        // total frames in animation
    int currentFrame;      // current frame index
    int frameTime;         // time per frame
    int frameTimer;        // current timer
    
    int frameWidth;        // width of one frame
    int frameHeight;       // height of one frame
    
    u32 attribute;         // blending / flags
    u8 id;
    u8 ext;
    s16 mode;              // render mode
} MultiTexture;

typedef struct _MEMMAN_CELL {
    // total size: 0xC
    struct _MEMMAN_CELL* prev; // offset 0x0, size 0x4
    struct _MEMMAN_CELL* next; // offset 0x4, size 0x4
    ssize_t size;              // offset 0x8, size 0x4
} _MEMMAN_CELL;

typedef struct {
    // total size: 0x2C
    u8* memHead;                   // offset 0x0, size 0x4
    ssize_t memSize;               // offset 0x4, size 0x4
    u32 ownNumber;                 // offset 0x8, size 0x4
    s32 ownUnit;                   // offset 0xC, size 0x4
    ssize_t remainder;             // offset 0x10, size 0x4
    ssize_t remainderMin;          // offset 0x14, size 0x4
    struct _MEMMAN_CELL* cell_1st; // offset 0x18, size 0x4
    struct _MEMMAN_CELL* cell_fin; // offset 0x1C, size 0x4
    u8* oriHead;                   // offset 0x20, size 0x4
    s32 oriSize;                   // offset 0x24, size 0x4
    s32 debIndex;                  // offset 0x28, size 0x4
} _MEMMAN_OBJ;

typedef struct {
    // total size: 0x20
    u8 cmd;       // offset 0x0, size 0x1
    u8 flags;     // offset 0x1, size 0x1
    u8 prog;      // offset 0x2, size 0x1
    u8 note;      // offset 0x3, size 0x1
    u8 attr;      // offset 0x4, size 0x1
    u8 vol;       // offset 0x5, size 0x1
    u8 pan;       // offset 0x6, size 0x1
    s16 pitch;    // offset 0x8, size 0x2
    u8 prio;      // offset 0xA, size 0x1
    u8 id1;       // offset 0xB, size 0x1
    u8 id2;       // offset 0xC, size 0x1
    u32 kofftime; // offset 0x10, size 0x4
    u8 limit;     // offset 0x14, size 0x1
    u16 param0;   // offset 0x16, size 0x2
    u16 param1;   // offset 0x18, size 0x2
    u16 param2;   // offset 0x1A, size 0x2
    u16 param3;   // offset 0x1C, size 0x2
    u16 link;     // offset 0x1E, size 0x2
} SoundEvent;     // Tentative name

typedef struct {
    // total size: 0x14
    u16 num_of_1st; // offset 0x0, size 0x2
    u16 apfn;       // offset 0x2, size 0x2
    s16 conv;       // offset 0x4, size 0x2
    s16 ix1st;      // offset 0x6, size 0x2
    u32 use;        // offset 0x8, size 0x4
    u32 to_tex;     // offset 0xC, size 0x4
    u32 to_chd;     // offset 0x10, size 0x4
} TexGroupData;

typedef struct {
    // total size: 0x10
    s16 hx;     // offset 0x0, size 0x2
    s16 hy;     // offset 0x2, size 0x2
    s16 hz;     // offset 0x4, size 0x2
    s8 sel_pri; // offset 0x6, size 0x1
    s8 sel_rl;  // offset 0x7, size 0x1
    s16 color;  // offset 0x8, size 0x2
    s8 sel_col; // offset 0xA, size 0x1
    s8 dspf;    // offset 0xB, size 0x1
    s8 ichi;    // offset 0xC, size 0x1
    s8 mts;     // offset 0xD, size 0x1
    s16 chix;   // offset 0xE, size 0x2
} PLEF;

typedef struct {
    // total size: 0x1C
    s16 my_wkid;     // offset 0x0, size 0x2
    u8 waza_num;     // offset 0x2, size 0x1
    u8 vs_refrect;   // offset 0x3, size 0x1
    u16 koa;         // offset 0x4, size 0x2
    u8 kind_of_tama; // offset 0x6, size 0x1
    u8 kage_index;   // offset 0x7, size 0x1
    u8 chix;         // offset 0x8, size 0x1
    u8 ernm;         // offset 0x9, size 0x1
    u8 erht;         // offset 0xA, size 0x1
    u8 erdf;         // offset 0xB, size 0x1
    u8 erex;         // offset 0xC, size 0x1
    u8 col_1p;       // offset 0xD, size 0x1
    u8 col_2p;       // offset 0xE, size 0x1
    u8 data00;       // offset 0xF, size 0x1
    u8 data01;       // offset 0x10, size 0x1
    u8 disp_type;    // offset 0x11, size 0x1
    s16 def_power;   // offset 0x12, size 0x2
    s16 life_time;   // offset 0x14, size 0x2
    s16 hos_x;       // offset 0x16, size 0x2
    s16 hos_y;       // offset 0x18, size 0x2
    u8 kz_blocking;  // offset 0x1A, size 0x1
    u8 free;         // offset 0x1B, size 0x1
} TAMA;

typedef struct {
} TexturePoolUsed;

typedef struct {
} PatternState;

#endif
