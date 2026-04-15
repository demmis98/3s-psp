#ifndef PSP_ADX_H
#define PSP_ADX_H

#include "types.h"

typedef enum {
    ADX_STAT_STOP,
    ADX_STAT_PLAYING,
    ADX_STAT_PLAYEND,
    ADX_STAT_ERROR
} AdxStat;

/* State constants used by the new sound3rd.c BGM server */
#define ADX_STATE_PLAYING  ADX_STAT_PLAYING
#define ADX_STATE_PLAYEND  ADX_STAT_PLAYEND

// variables
extern volatile u16 Sound_Mono;

// Initialize/shutdown ADX audio system
void adxInit(void);
void adxShutdown(void);

// Play an ADX file from the AFS archive by file number
void adxPlay(u16 fnum);

// Play with loop points (sample numbers)
void adxPlayLoop(u16 fnum, u32 loop_start, u32 loop_end);

// Stop playback
void adxStop(void);

// Get current state
AdxStat adxGetStat(void);

// Call each frame to keep decode buffer filled
void adxUpdate(void);

// ── Compatibility wrappers for the new sound system ──────────────
// These bridge the branch's ADX_* API to our existing adx* functions.

void ADX_Init(void);
void ADX_Exit(void);
void ADX_Stop(void);
void ADX_StartAfs(s32 fnum);
void ADX_EntryAfs(s32 fnum);
void ADX_StartSeamless(void);
void ADX_StartMem(void* data, u32 size);
s32  ADX_GetState(void);
void ADX_SetOutVol(s32 vol);
void ADX_SetMono(s32 mono);
s32  ADX_GetNumFiles(void);
void ADX_ResetEntry(void);
void ADX_Pause(s32 pause);
s32  ADX_IsPaused(void);
void ADX_ProcessTracks(void);

#endif
