#include <pspuser.h>
#include <psppower.h>

#include "common/audio.h"
#include "common/graphics.h"
#include "psp/adx.h"
#include "psp/afs.h"
#include <pspaudiolib.h>

#include "Game/main.h"

// PSP_MODULE_INFO is required
PSP_MODULE_INFO("3rd-strike", PSP_MODULE_USER, 1, 0);
//PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);
PSP_MAIN_THREAD_ATTR(PSP_THREAD_ATTR_VFPU | PSP_THREAD_ATTR_USER);
PSP_HEAP_SIZE_KB(-1024);
PSP_HEAP_THRESHOLD_SIZE_KB(1024);

//#define IGNORE_CLOCK

// global variables
volatile int g_request_pause = 0;
extern int RTT_Enabled;

extern void adxSuspend(void);
extern void adxResume(void);

volatile int clock_mode = CLOCK_300;
volatile int clock_mode_temp = -1;

void updateClock(){
    if(clock_mode_temp != clock_mode){
        clock_mode_temp = clock_mode;
        
        #ifdef IGNORE_CLOCK
        return;
        #endif

        switch (clock_mode) {
        case CLOCK_222:
            scePowerSetClockFrequency(222, 222, 111);
            break;
        case CLOCK_266:
            scePowerSetClockFrequency(266, 266, 133);
            break;
        case CLOCK_300:
            scePowerSetClockFrequency(300, 300, 150);
            break;
        case CLOCK_333:
            scePowerSetClockFrequency(333, 333, 166);
            break;
        default:
            break;
        }
    }
}

void setClock(int clockMode){
    clock_mode = clockMode;
}

void forceClock(int clockMode){
    clock_mode_temp = -1;
    setClock(clockMode);
}

int power_callback(int unknown, int powerInfo, void *common) {
    if (powerInfo & PSP_POWER_CB_SUSPENDING) {
        g_request_pause = 1;
        /* Silence audio callback during sleep */
        adxSuspend();
        /* Close AFS fds — prevents stale fd reads */
        afsSuspend();
        /* Trigger in-game pause on resume */
        sceKernelDelayThread(2000);
    }
    if (powerInfo & PSP_POWER_CB_RESUME_COMPLETE) {
        /* Proactively reopen AFS fds — sceIoRead on stale fd may hang
           rather than return error on some firmware */
        afsReopen();
        /* Restore Clock Frequency — OS may reset clock after sleep */
        if(RTT_Enabled)
            forceClock(CLOCK_300);
        else
            forceClock(CLOCK_266);
        /* Resume audio playback */
        g_request_pause = 1;
        adxResume();
    }
    return 0;
}

int exit_callback(int arg1, int arg2, void *common) {
    sceKernelExitGame();
    return 0;
}

int callback_thread(SceSize args, void *argp) {
    int exit_cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
    sceKernelRegisterExitCallback(exit_cbid);

    int power_cbid = sceKernelCreateCallback("Power Callback", power_callback, NULL);
    scePowerRegisterCallback(0, power_cbid);

    sceKernelSleepThreadCB();
    return 0;
}

int setup_callbacks(void) {
    int thid = sceKernelCreateThread("callback_thread", callback_thread, 0x11, 0xFA0, 0, 0);
    if(thid >= 0)
        sceKernelStartThread(thid, 0, 0);
    return thid;
}

extern int DEMMA_DEBUG;

int main(void)  {
    // Use above functions to make exiting possible
    setup_callbacks();

    initGu();

    pspAudioInit();  // Init pspaudiolib with NULL callbacks (silence)
    // SPU_Init sets channel 0 callback for SFX
    // ADX uses sceAudioChReserve for BGM on a separate channel
    AcrMain();

    return 0;
}
