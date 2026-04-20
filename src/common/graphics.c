
#include "graphics.h"
#include "sprites.h"
#include <psprtc.h>

//just for this project
#include "Game/WORK_SYS.h"
#include "Game/sc_data.h"

extern void ppgResetTextureCache(void);
s16 render_mode = SCREEN_DEFAULT_MODE;

// variables
static unsigned int __attribute__((aligned(64))) list[0x40000];

static void * fbp0;
static void * fbp1;
static void * zBuff;
static void * rttBuf;

static uint32_t bg_color = 0xFF000000;

/* CPS3 clipping bounds — used by decompiled game code (bg.c, DC_Ghost.c, etc.) */
float Min_X = 0.0f;
float Max_X = 384.0f;
float Min_Y = 0.0f;
float Max_Y = 224.0f;

int my_gu_init = 0;
int RTT_Enabled = SCREEN_DEFAULT_RTT;  /* 0=direct rendering, 1=RTT (no seams) */

/* CPS3 native resolution */
#define CPS3_WIDTH  384
#define CPS3_HEIGHT 224
#define RTT_BUF_WIDTH 512  /* power-of-2 stride for texture */

/* Track back buffer for double-buffering */
static int backBuf = 0;

/* Blit quad coordinates */
static float blit_x0, blit_y0, blit_x1, blit_y1;
static s32 scissor_trim_right = 0;
s32 blit_filter = SCREEN_DEFAULT_FILTER;  /* 0=bilinear, 1=nearest */

/* SCALE_X/SCALE_Y — identity during RTT rendering */
float Scale_Factor_X = 1.0f;
float Scale_Factor_Y = 1.0f;
float Scale_Off_X = 0.0f;
float Scale_Off_Y = 0.0f;

/* keep track of the scaling mode */
int RTT_Enabled_temp = -1;
int Scaling_mode_temp = -1;

void setupScaling(int mode) {
    if(RTT_Enabled_temp == RTT_Enabled && Scaling_mode_temp == mode)
        return;
    RTT_Enabled_temp = RTT_Enabled;
    Scaling_mode_temp = mode;

    if(RTT_Enabled){
        switch (mode) {
        case SCREEN_MODE_STRETCH: /* Stretch — fill entire PSP screen */
            blit_x0 = 0.0f;
            blit_y0 = 0.0f;
            blit_x1 = 480.0f;
            blit_y1 = 272.0f;
            break;
        case SCREEN_MODE_SQUARE: /* Pseudo 4:3 */
            blit_x0 = 40.0f;
            blit_y0 = 0.0f;
            blit_x1 = 440.0f;
            blit_y1 = 272.0f;
            break;
        case SCREEN_MODE_NATIVE: { /* 1:1 pixel perfect */
            blit_x0 = (480.0f - CPS3_WIDTH) / 2.0f;
            blit_y0 = (272.0f - CPS3_HEIGHT) / 2.0f;
            blit_x1 = blit_x0 + CPS3_WIDTH;
            blit_y1 = blit_y0 + CPS3_HEIGHT;
            break;
        }
        case SCREEN_MODE_VERTICAL: { /* 4:3 filling height / 1:1 pixels with extended viewport */
            float w = 272.0f * (4.0f / 3.0f);
            blit_x0 = (480.0f - w) / 2.0f;
            blit_y0 = 0.0f;
            blit_x1 = blit_x0 + w;
            blit_y1 = 272.0f;
            break;
        }
        case SCREEN_MODE_EXTENDED: /* 4:3 Native / Extended viewport */
        default:
            blit_x0 = (480.0f - CPS3_WIDTH) / 2.0f;
            blit_y0 = (272.0f - CPS3_HEIGHT) / 2.0f;
            blit_x1 = blit_x0 + CPS3_WIDTH;
            blit_y1 = blit_y0 + CPS3_HEIGHT;
            break;
        }

        //Fade_Pos_tbl[8] = { 0, 0, 640, 0, 0, 448, 640, 448 }
        Fade_Pos_tbl[0] = 0;
        Fade_Pos_tbl[1] = 0;
        Fade_Pos_tbl[2] = 640;
        Fade_Pos_tbl[3] = 0;
        Fade_Pos_tbl[4] = 0;
        Fade_Pos_tbl[5] = 480;
        Fade_Pos_tbl[6] = 640;
        Fade_Pos_tbl[7] = 480;

        Scale_Factor_X = 1.0f;
        Scale_Factor_Y = 1.0f;
        Scale_Off_X = 0.0f;
        Scale_Off_Y = 0.0f;
    }
    else{switch (mode) {
    case SCREEN_MODE_STRETCH: /* Stretch — fill entire PSP screen */
        Scale_Factor_X = 480.0f / 384.0f;
        Scale_Factor_Y = 272.0f / 224.0f;
        Scale_Off_X = 0.0f;
        Scale_Off_Y = 0.0f;

        Min_X = 0.0f;
        Max_X = 384.0f;
        Min_Y = 0.0f;
        Max_Y = 224.0f;
        break;
    case SCREEN_MODE_SQUARE: /* Pseudo 4:3 */
        Scale_Factor_X = 400.0f / 384.0f;
        Scale_Factor_Y = 272.0f / 246.0f;
        Scale_Off_X = 40.0f;
        Scale_Off_Y = 16.0f * Scale_Factor_Y;

        Min_X = 0.0f;
        Max_X = 384.0f;
        Min_Y = -16.0f;
        Max_Y = 230.0f;
        break;
    case SCREEN_MODE_NATIVE: { /* 1:1 pixel perfect */
        Scale_Factor_X = 1.0f;
        Scale_Factor_Y = 1.0f;
        Scale_Off_X = 48.0f;
        Scale_Off_Y = 24.0f;

        Min_X = 0.0f;
        Max_X = 384.0f;
        Min_Y = 0.0f;
        Max_Y = 224.0f;
        break;
    }
    case SCREEN_MODE_VERTICAL: { /* 4:3 filling height / 1:1 pixels with extended viewport */
        Scale_Factor_X = 1.0f;
        Scale_Factor_Y = 1.0f;
        Scale_Off_X = 48.0f;
        Scale_Off_Y = 24.0f;

        Min_X = 0.0f;
        Max_X = 384.0f;
        Min_Y = -16.0f;
        Max_Y = 230.0f;
        break;
    }
    case SCREEN_MODE_EXTENDED: /* 4:3 Native / Extended viewport */
    default:
        Scale_Factor_X = 1.0f;
        Scale_Factor_Y = 1.0f;
        Scale_Off_X = 48.0f;
        Scale_Off_Y = 24.0f;

        Min_X = -Scale_Off_X;
        Max_X = SCREEN_WIDTH;
        Min_Y = -Scale_Off_Y;
        Max_Y = SCREEN_HEIGHT;
        break;
    }

    //Fade_Pos_tbl[8] = { 0, 0, 640, 0, 0, 448, 640, 448 }
    Fade_Pos_tbl[0] = Min_X * 640 / 384;
    Fade_Pos_tbl[1] = Min_Y * 488 / 224;
    Fade_Pos_tbl[2] = Max_X * 640 / 384;
    Fade_Pos_tbl[3] = Min_Y * 640 / 384;
    Fade_Pos_tbl[4] = Min_Y * 480 / 224;
    Fade_Pos_tbl[5] = Max_Y * 480 / 224;
    Fade_Pos_tbl[6] = Max_X * 640 / 384;
    Fade_Pos_tbl[7] = Max_Y * 480 / 224;

    blit_x0 = 0.0f;
    blit_y0 = 0.0f;
    blit_x1 = 1.0f;
    blit_y1 = 1.0f;
    }

    __asm__ volatile (
        // Load constants once
        "mtv %0, S410\n"  // load Scale_Factor_X to matrix
        "mtv %1, S411\n"  // load Scale_Factor_Y to matrix
        "mtv %0, S412\n"  // load Scale_Factor_X to matrix
        "mtv %1, S413\n"  // load Scale_Factor_Y to matrix        
        "mtv %2, S420\n"  // load Scale_Off_X to matrix
        "mtv %3, S421\n"  // load Scale_Off_Y to matrix
        "mtv %2, S422\n"  // load Scale_Off_X to matrix
        "mtv %3, S423\n"  // load Scale_Off_Y to matrix
        :
        : "r"(Scale_Factor_X), "r"(Scale_Factor_Y), // %0 = Scale_Factor_X, %1 = Scale_Factor_Y
        "r"(Scale_Off_X), "r"(Scale_Off_Y)  // %2 = Scale_Off_X, %3 = Scale_Off_Y
    );
}

void enableOffscreenMode() { }

void initGu(){
    sceGuInit();

    fbp0 = guGetStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
    fbp1 = guGetStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_8888);
    zBuff = guGetStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_4444);
    rttBuf = guGetStaticVramBuffer(RTT_BUF_WIDTH, CPS3_HEIGHT, GU_PSM_8888);  /* 256 = power-of-2 for clean texture sampling */

    backBuf = 0;

    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, fbp0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH, SCREEN_HEIGHT, fbp1, BUFFER_WIDTH);

    sceGuDepthBuffer(zBuff, BUFFER_WIDTH);
    sceGuEnable(GU_DEPTH_TEST);
    sceGuDepthFunc(GU_LEQUAL);

    sceGuDisable(GU_CULL_FACE);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_CLIP_PLANES);

    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);

    sceGuEnable(GU_ALPHA_TEST);
    sceGuAlphaFunc(GU_GREATER, 0x00, 0xFF);

    sceGuOffset(2048 - (SCREEN_WIDTH / 2) + 10, 2048 - (SCREEN_HEIGHT / 2) + 10);
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);

    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);
    sceGuTexWrap(GU_CLAMP, GU_CLAMP);
    sceGuEnable(GU_TEXTURE_2D);
    sceGuFinish();

    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();
    sceGuDisplay(GU_TRUE);

    my_gu_init = 1;
    backBuf = 0;
}

void endGu(){
    sceGuDisplay(GU_FALSE);
    sceGuTerm();
    my_gu_init = 0;
}

/* Vertex for the final blit quad */
typedef struct { float u, v; float x, y, z; } BlitVertex;

void startFrame(){
    //sceGuSync(GU_SYNC_LIST, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
    backBuf ^= 1;

    setupScaling(render_mode);

    sceGuStart(GU_DIRECT, list);

    if (RTT_Enabled) {
        sceGuDrawBufferList(GU_PSM_8888, rttBuf, RTT_BUF_WIDTH);
        sceGuOffset(2048 - (CPS3_WIDTH / 2) + 10, 2048 - (CPS3_HEIGHT / 2) + 10);
        sceGuViewport(2048, 2048, CPS3_WIDTH, CPS3_HEIGHT);
    }
    else{
        sceGuOffset(2048 - (SCREEN_WIDTH / 2) + 10, 2048 - (SCREEN_HEIGHT / 2) + 10);
        sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    }
    ppgResetTextureCache();

    sceGuClearColor(bg_color);
    sceGuClearDepth(0xFFFF);
    if (RTT_Enabled) {
        /* Scissor BEFORE clear — prevents clear from overflowing
           RTT buffer into adjacent VRAM texture cache */
        sceGuScissor(0, 0, CPS3_WIDTH, CPS3_HEIGHT);
        sceGuEnable(GU_SCISSOR_TEST);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
    }

    else {
        sceGuDisable(GU_SCISSOR_TEST);

        sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
        s32 sx = (s32)SCALE_X(Min_X);
        if(sx < 0)
            sx = 0;
        s32 sy = (s32)SCALE_Y(Min_Y);
        if(sy < 0)
            sy = 0;
        s32 sw = (s32)(Max_X - Min_X) * Scale_Factor_X;
        if(sw + sx > SCREEN_WIDTH)
            sw = SCREEN_WIDTH - sx;
        s32 sh = (s32)(Max_Y - Min_Y) * Scale_Factor_Y;
        if(sh + sy > SCREEN_HEIGHT)
            sh = SCREEN_HEIGHT - sy;
        sceGuScissor(sx, sy, sw, sh);

        sceGuEnable(GU_SCISSOR_TEST);
    }
    //sceGuEnable(GU_SCISSOR_TEST);
    sceGuEnable(GU_TEXTURE_2D);
}

void endFrame(){

    sceGuTexFilter(blit_filter ? GU_NEAREST : GU_LINEAR,
                   blit_filter ? GU_NEAREST : GU_LINEAR);
    if (!RTT_Enabled) {
        /* Direct path — just finish and swap */
        sceGuFinish();
        return;
    }

    /* RTT path — switch to screen, blit */
    void *curBack = backBuf ? fbp1 : fbp0;
    sceGuDrawBufferList(GU_PSM_8888, curBack, BUFFER_WIDTH);
    //ppgResetTextureCache();

    sceGuOffset(2048 - (SCREEN_WIDTH / 2) + 10, 2048 - (SCREEN_HEIGHT / 2) + 10);
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);

    sceGuDisable(GU_SCISSOR_TEST);
    if (blit_x0 > 0.5f || blit_y0 > 0.5f) {
        sceGuClearColor(0xFF000000);
        sceGuClear(GU_COLOR_BUFFER_BIT);
    }

    sceGuDisable(GU_DEPTH_TEST);
    sceGuDisable(GU_ALPHA_TEST);
    /* Force opaque blit — use fixed blend factors to ignore any alpha */
    //sceGuEnable(GU_BLEND);
    //sceGuBlendFunc(GU_ADD, GU_FIX, GU_FIX, 0xFFFFFFFF, 0x00000000);
    sceGuDisable(GU_BLEND);
    sceGuTexMode(GU_PSM_8888, 0, 0, 0);
    void *rttAbs = (void*)((u32)sceGeEdramGetAddr() + (u32)rttBuf);
    sceGuTexImage(0, RTT_BUF_WIDTH, 256, RTT_BUF_WIDTH, rttAbs);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGB);
    sceGuEnable(GU_TEXTURE_2D);

    BlitVertex *v = (BlitVertex*)sceGuGetMemory(2 * sizeof(BlitVertex));
    v[0].u = 0.5f;                     v[0].v = 0.5f;
    v[0].x = blit_x0;                  v[0].y = blit_y0;       v[0].z = 0.0f;
    v[1].u = (float)CPS3_WIDTH - 0.5f; v[1].v = (float)CPS3_HEIGHT - 0.5f;
    v[1].x = blit_x1 - scissor_trim_right;
    v[1].y = blit_y1;                  v[1].z = 0.0f;
    sceGuDrawArray(GU_SPRITES,
        GU_TEXTURE_32BITF | GU_VERTEX_32BITF | GU_TRANSFORM_2D,
        2, 0, v);


    /* Restore state for next frame */
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    sceGuTexFunc(GU_TFX_MODULATE, GU_TCC_RGBA);
    sceGuTexFilter(GU_NEAREST, GU_NEAREST);

    sceGuEnable(GU_DEPTH_TEST);
    sceGuEnable(GU_ALPHA_TEST);
    sceGuEnable(GU_BLEND);

    sceGuFinish();
}

void endFrameDebug(){
    /* Restore screen draw buffer on debug/skip frames */
    void *curBack = backBuf ? fbp1 : fbp0;
    sceGuDrawBufferList(GU_PSM_8888, curBack, BUFFER_WIDTH);
    sceGuOffset(2048 - (SCREEN_WIDTH / 2) + 10, 2048 - (SCREEN_HEIGHT / 2) + 10);
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();
}

uint32_t getBgColor(){
    return bg_color;
}

void setBgColor(uint32_t color){
    bg_color = color;
}

int getGuInit(){
    return my_gu_init;
}
