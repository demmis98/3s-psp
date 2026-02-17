#include <pspuser.h>
#include "game/game.h"
#include "inc/audio.h"
#include "inc/graphics.h"

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("tank", 0, 1, 1);
//PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

// global variables

Game game;


int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(cbid);
    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("update_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

void logic(){
	game.logic();
}

void draw(){
	game.draw();
}

int main(void)  {
    // Use above functions to make exiting possible
    setup_callbacks();
	
    game = Game();
    
    initGu();
    initMusic();
    startFrame();
    game.init();
    endFrame();
    while(1) {
        startFrame();

        draw();

        endFrame();

    	logic();
    }

    return 0;
}
