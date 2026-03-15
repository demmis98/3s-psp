#include <pspuser.h>
#include <psppower.h>

#include "common/audio.h"
#include "common/graphics.h"

#include "Game/main.h"

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("3rd-strike", 0, 5, 1);
//PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);

// global variables

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

int main(void)  {
    // Use above functions to make exiting possible
    setup_callbacks();

    scePowerSetClockFrequency(333, 333, 166);

    initGu();

    initMusic();

    AcrMain();

    return 0;
}
