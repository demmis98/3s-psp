#include "graphics.h"

// variables
//static unsigned int list[0x40000] __attribute__((aligned(64)));
//char list[0x20000] __attribute__((aligned(64)));
//static unsigned int __attribute__((aligned(16))) list[262144];
//static unsigned int __attribute__((aligned(16))) list[0x40000];
//static unsigned int __attribute__((aligned(16))) list[0x20000];
static unsigned int __attribute__((aligned(64))) list[0x20000];

static void * fbp0;
static void * fbp1;
static void * zBuff;

//variables
static uint32_t bg_color = 0xFF000000;

int my_gu_init = 0;

void initGu(){
    sceGuInit();

	fbp0 = guGetStaticVramBuffer(BUFFER_WIDTH,SCREEN_HEIGHT,GU_PSM_8888);
    fbp1 = guGetStaticVramBuffer(BUFFER_WIDTH,SCREEN_HEIGHT,GU_PSM_8888);
    zBuff = guGetStaticVramBuffer(BUFFER_WIDTH, SCREEN_HEIGHT, GU_PSM_4444);

    //Set up buffers
    sceGuStart(GU_DIRECT, list);
    sceGuDrawBuffer(GU_PSM_8888, fbp0, BUFFER_WIDTH);
    sceGuDispBuffer(SCREEN_WIDTH,SCREEN_HEIGHT,fbp1, BUFFER_WIDTH);
    
    // We do not care about the depth buffer in this example
    //sceGuDepthBuffer(fbp0, 0); // Set depth buffer to a length of 0
    sceGuDepthBuffer(zBuff, BUFFER_WIDTH);
    sceGuEnable(GU_DEPTH_TEST); // Disable depth testing
    sceGuDepthFunc(GU_LEQUAL);

    sceGuDisable(GU_CULL_FACE);
    sceGuDisable(GU_LIGHTING);
    sceGuDisable(GU_CLIP_PLANES);

    // alpha blending
    sceGuEnable(GU_BLEND);
    sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
    //sceGuBlendFunc(GU_MAX, GU_SRC_ALPHA, GU_SRC_ALPHA, 0, 0);

    sceGuEnable(GU_ALPHA_TEST);
    sceGuAlphaFunc(GU_GREATER, 0x00, 0xFF);  // Only draw pixels with alpha > 0

    //Set up viewport
    sceGuOffset(2048 - (SCREEN_WIDTH / 2), 2048 - (SCREEN_HEIGHT / 2));
    sceGuViewport(2048, 2048, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuScissor(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sceGuEnable(GU_SCISSOR_TEST);

    // Start a new frame and enable the display
    sceGuEnable(GU_TEXTURE_2D);
    sceGuFinish();

    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();

    sceGuDisplay(GU_TRUE);

    my_gu_init = 1;
}

void endGu(){
    sceGuDisplay(GU_FALSE);
    sceGuTerm();

    my_gu_init = 0;
}

void startFrame(){
    sceGuStart(GU_DIRECT, list);
    sceGuClearColor(bg_color); // black background
    sceGuClearDepth(0xFFFF);
    sceGuClear(GU_COLOR_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);
    sceGuEnable(GU_TEXTURE_2D);
}

void endFrame(){
    sceGuFinish();
    sceGuSync(GU_SYNC_FINISH, GU_SYNC_WHAT_DONE);
    sceDisplayWaitVblankStart();
    sceGuSwapBuffers();
}

void endFrameDebug(){
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
