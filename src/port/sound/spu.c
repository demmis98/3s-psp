/**
 * @file spu.c
 * @brief Software PS2 SPU2 emulator — ADPCM decoding and ADSR envelope.
 *
 * Ported from the working PC (SDL) implementation to PSP audio callback.
 * Emulates the PS2 Sound Processing Unit: decodes VAG/ADPCM blocks,
 * runs per-voice ADSR envelopes, applies pitch interpolation, and
 * mixes 48 voices into a stereo output stream.
 */
#include "port/sound/spu.h"
#include "psp/adx.h"

#include "common.h"
#include <pspkernel.h>
#include <pspaudiolib.h>
#include <stdbool.h>
#include <string.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))
#define clamp(val, lo, hi) (((val) > (hi)) ? (hi) : (((val) < (lo)) ? (lo) : (val)))

#define VOICE_COUNT 48
#define MAX_ACTIVE_VOICES 10  // Cap concurrent voices for PSP CPU budget

#include "interp_table.inc"

enum {
    ADSR_PHASE_ATTACK,
    ADSR_PHASE_DECAY,
    ADSR_PHASE_SUSTAIN,
    ADSR_PHASE_RELEASE,
    ADSR_PHASE_STOPPED,
};

struct AdsrParamCache {
    bool decr;
    bool exp;
    u8 shift;
    s8 step;
    s32 target;
    bool infinite;
};

typedef struct {
    bool run;
    bool noise;
    bool endx;
    s16 decodeHist[2];
    u32 counter;
    u16 pitch;
    u16* sample;
    u32 ssa;
    u32 nax;
    u32 lsa;
    bool customLoop;

    s32 envx;
    s32 voll, volr;

    u16 adsr1, adsr2;
    bool pmon;
    s32 last_sample;

    u8 adsr_phase;
    u32 adsr_counter;
    struct AdsrParamCache adsr_param;

    s16 decodeBuf[0x40];
    u32 decRPos, decWPos, decLeft;
} SPU_Voice;

SceUID soundLock;

static SPU_Voice voices[VOICE_COUNT];
static u16 ram[(2 * 1024 * 1024) >> 1];
static s16 adpcm_coefs[5][2] = {
    { 0, 0 }, { 60, 0 }, { 115, -52 }, { 98, -55 }, { 122, -60 },
};

static uint64_t active_voices = 0;

static inline s16 SPU_ApplyVolume(s16 sample, s32 volume) {
    return (sample * volume) >> 15;
}

// ── ADSR Envelope (from PC port) ──────────────────────────────────

static void SPU_VoiceCacheADSR(SPU_Voice* v) {
    struct AdsrParamCache* pc = &v->adsr_param;

    switch (v->adsr_phase) {
    case ADSR_PHASE_ATTACK:
        pc->decr = false;
        pc->exp = ((v->adsr1 & 0x8000) != 0);
        pc->shift = (v->adsr1 >> 10) & 0x1f;
        pc->step = 7 - ((v->adsr1 >> 8) & 0x3);
        pc->target = 0x7fff;
        pc->infinite = ((v->adsr1 >> 8) & 0x7f) == 0x7f;
        break;
    case ADSR_PHASE_DECAY:
        pc->decr = true;
        pc->exp = true;
        pc->shift = (v->adsr1 >> 4) & 0xf;
        pc->step = -8;
        pc->target = ((v->adsr1 & 0xf) + 1) << 11;
        pc->infinite = ((v->adsr1 >> 4) & 0xf) == 0xf;
        break;
    case ADSR_PHASE_SUSTAIN:
        pc->decr = ((v->adsr2 & 0x4000) != 0);
        pc->exp = ((v->adsr2 & 0x8000) != 0);
        pc->shift = (v->adsr2 >> 8) & 0x1f;
        pc->step = 7 - ((v->adsr2 >> 6) & 0x3);
        pc->target = 0;
        pc->infinite = ((v->adsr2 >> 6) & 0x7f) == 0x7f;

        if (pc->decr) {
            pc->step = ~v->adsr_param.step;
        }
        break;
    case ADSR_PHASE_RELEASE:
        pc->decr = true;
        pc->exp = ((v->adsr2 & 0x20) != 0);
        pc->shift = v->adsr2 & 0x1f;
        pc->step = -8;
        pc->target = 0;
        pc->infinite = (v->adsr2 & 0x1f) == 0x1f;
        break;
    }
}

static void SPU_VoiceRunADSR(SPU_Voice* v) {
    struct AdsrParamCache* pc = &v->adsr_param;
    u32 counter_inc = 0x8000 >> max(0, pc->shift - 11);
    s32 level_inc = pc->step << max(0, 11 - pc->shift);

    if (pc->exp && !pc->decr && v->envx >= 0x6000) {
        if (pc->shift < 10) {
            level_inc >>= 2;
        } else if (pc->shift >= 11) {
            counter_inc >>= 2;
        } else {
            counter_inc >>= 1;
            level_inc >>= 1;
        }
    } else if (pc->exp && pc->decr) {
        level_inc = (level_inc * v->envx) >> 15;
        /* Prevent exponential decay from stalling at low envx.
           Integer truncation makes level_inc=0 when envx < 4096,
           causing the voice to never reach 0. PS2 hardware doesn't
           have this issue. Ensure minimum decay of -1. */
        if (level_inc == 0 && v->envx > 0) {
            level_inc = -1;
        }
    }

    if (!pc->infinite) {
        counter_inc = max(counter_inc, 1);
    }
    v->adsr_counter += counter_inc;

    if (v->adsr_counter & 0x8000) {
        v->adsr_counter = 0;
        v->envx = clamp(v->envx + level_inc, 0, INT16_MAX);
    }

    if (v->adsr_phase == ADSR_PHASE_SUSTAIN) {
        return;
    }

    if ((!pc->decr && v->envx >= pc->target) || ((pc->decr && v->envx <= pc->target))) {
        v->adsr_phase++;
        SPU_VoiceCacheADSR(v);
    }

    if (v->adsr_phase > ADSR_PHASE_RELEASE) {
        v->run = false;
        active_voices &= ~(1ULL << (int)(v - voices));
    }
}

// ── VAG/ADPCM Decode (from PC port) ──────────────────────────────

static void __attribute__((hot)) SPU_VoiceDecode(SPU_Voice* v) {
    u32 data;
    u16 header;
    s32 shift, c0, c1;
    s32 h0, h1;

    if (v->decLeft >= 16) {
        return;
    }

    data = ram[v->nax];
    header = ram[v->nax & ~0x7];
    shift = header & 0xf;
    u32 filter = (header >> 4) & 7;

    if (shift > 12) shift = 12;
    if (filter > 4) filter = 0;

    c0 = adpcm_coefs[filter][0];
    c1 = adpcm_coefs[filter][1];
    h0 = v->decodeHist[0];
    h1 = v->decodeHist[1];
    u32 wp = v->decWPos;

    // Unrolled: decode 4 nibbles from the 16-bit data word
    #define DECODE_NIBBLE(nib) do { \
        s32 s = (s16)(((nib) & 0xF) << 12) >> shift; \
        s += (c0 * h0 + c1 * h1) >> 6; \
        if (s > 32767) s = 32767; else if (s < -32768) s = -32768; \
        h1 = h0; h0 = s; \
        v->decodeBuf[wp] = s; \
        v->decodeBuf[wp | 0x20] = s; \
        wp = (wp + 1) & 0x1f; \
    } while(0)

    DECODE_NIBBLE(data);       data >>= 4;
    DECODE_NIBBLE(data);       data >>= 4;
    DECODE_NIBBLE(data);       data >>= 4;
    DECODE_NIBBLE(data);

    #undef DECODE_NIBBLE

    v->decodeHist[0] = h0;
    v->decodeHist[1] = h1;
    v->decWPos = wp;
    v->decLeft += 4;

    v->nax = (v->nax + 1) & 0xfffff;

    if ((v->nax & 0x7) == 0) {
        if (header & 0x100) {
            v->nax = v->lsa;
            v->endx = true;

            if ((header & 0x200) == 0) {
                if (!v->noise) {
                    v->envx = 0;
                    v->adsr_phase = ADSR_PHASE_STOPPED;
                    v->run = false;
                    active_voices &= ~(1ULL << (int)(v - voices));
                }
            }
        }

        header = ram[v->nax & ~0x7];
        if (header & 0x400) {
            v->lsa = v->nax;
        }

        v->nax = (v->nax + 1) & 0xfffff;
    }
}

// ── Per-voice tick with pitch interpolation (from PC port) ────────

static void __attribute__((hot)) SPU_VoiceTick(SPU_Voice* v, s32* output, s32 last_voice_sample) {
    s32 sample, pitchStep, decInc;

    SPU_VoiceDecode(v);

    // Linear interpolation (2 multiplies instead of 4-tap gaussian)
    {
        u32 frac = (v->counter & 0xfff);
        s16 *buff = &v->decodeBuf[v->decRPos];
        s32 s0 = buff[1];
        s32 s1 = buff[2];
        sample = s0 + ((s1-s0)*(s32)frac >> 12);
    }

    pitchStep = v->pitch;
    if (v->pmon) {
        pitchStep = (pitchStep * (0x8000 + last_voice_sample)) >> 15;
    }
    if (pitchStep > 0x3fff) pitchStep = 0x3fff;
    v->counter += pitchStep;

    decInc = v->counter >> 12;
    v->counter &= 0xfff;
    v->decRPos = (v->decRPos + decInc) & 0x1f;
    v->decLeft -= decInc;

    sample = (s16)((sample * v->envx) >> 15);
    v->last_sample = sample;
    output[0] = (s16)((sample * v->voll) >> 15);
    output[1] = (s16)((sample * v->volr) >> 15);

    SPU_VoiceRunADSR(v);
}

// ── Public API ────────────────────────────────────────────────────

bool SPU_VoiceIsFinished(int vnum) {
    if (voices[vnum].envx == 0 && voices[vnum].adsr_phase != ADSR_PHASE_ATTACK) {
        return true;
    }
    return false;
}

void SPU_VoiceKeyOff(int vnum) {
    if (voices[vnum].adsr_phase < ADSR_PHASE_RELEASE) {
        voices[vnum].adsr_phase = ADSR_PHASE_RELEASE;
        SPU_VoiceCacheADSR(&voices[vnum]);
    }
}

void SPU_VoiceStop(int vnum) {
    voices[vnum].envx = 0;
    voices[vnum].adsr_phase = ADSR_PHASE_STOPPED;
    voices[vnum].run = false;
    active_voices &= ~(1ULL << vnum);
}

void SPU_VoiceGetConf(int vnum, struct SPUVConf* conf) {
    SPU_Voice* v = &voices[vnum];
    conf->pitch = v->pitch;
    conf->voll = v->voll;
    conf->volr = v->volr;
    conf->adsr1 = v->adsr1;
    conf->adsr2 = v->adsr2;
    conf->pmon = v->pmon;
}

void SPU_VoiceSetConf(int vnum, struct SPUVConf* conf) {
    SPU_Voice* v = &voices[vnum];
    v->pitch = conf->pitch;
    v->voll = conf->voll << 1;
    v->volr = conf->volr << 1;
    v->adsr1 = conf->adsr1;
    v->adsr2 = conf->adsr2;
    v->pmon = conf->pmon;
}

void SPU_VoiceStart(int vnum, u32 start_addr) {
    SPU_Voice* v = &voices[vnum];
    u16 header;

    /* Reject voices with invalid start address (unloaded bank data) */
    if (start_addr == 0 || start_addr >= 0x100000) {
        v->run = false;
        active_voices &= ~(1ULL << vnum);
        return;
    }

    v->ssa = start_addr;
    v->lsa = start_addr;
    v->nax = v->ssa;
    v->run = true;
    active_voices |= (1ULL << vnum);
    v->envx = 0;

    v->adsr_counter = 0;
    v->adsr_phase = ADSR_PHASE_ATTACK;
    SPU_VoiceCacheADSR(v);

    header = ram[v->nax & ~0x7];
    if ((header >> 10) & 1) {
        v->lsa = v->nax;
    }

    v->nax = (v->nax + 1) & 0xfffff;
}

// ── SPU Tick and PSP Audio Callback ───────────────────────────────

void SPU_Tick(s16* output) {
    s32 acc[2] = {};
    s32 vout[2] = {};
    s32 count = 0;

    uint64_t mask = active_voices;
    while (mask) {
        int i = __builtin_ctzll(mask);
        mask &= mask - 1;

        if (count >= MAX_ACTIVE_VOICES) {
            /* CPU budget exceeded — still run ADSR so voices can complete
               release and free their slots. Without this, excess voices
               freeze mid-sound and never finish → sounds last too long. */
            SPU_VoiceRunADSR(&voices[i]);
            if (voices[i].adsr_phase > ADSR_PHASE_RELEASE) {
                voices[i].run = false;
                active_voices &= ~(1ULL << i);
            }
            continue;
        }

        s32 last_voice_sample = 0;
        if (i > 0 && voices[i].pmon) {
            last_voice_sample = voices[i - 1].last_sample;
        }

        SPU_VoiceTick(&voices[i], vout, last_voice_sample);
        acc[0] += vout[0];
        acc[1] += vout[1];
        count++;
    }

    output[0] = clamp(acc[0], INT16_MIN, INT16_MAX);
    output[1] = clamp(acc[1], INT16_MIN, INT16_MAX);
}

// PSP audio runs at 44100Hz, but SPU needs 48000Hz tick rate.
// Use fractional accumulator to resample: generate 48kHz ticks,
// nearest-neighbor downsample to 44.1kHz output.
#define SPU_TICK_RATE 48000
#define PSP_AUDIO_RATE 44100

static s16 spu_last_output[2] = {0, 0};
static volatile int spu_locked = 0;
static uint32_t spu_resample_frac = 0;

void SPU_PSP_CB(void* buf, unsigned int reqn, void* pdata) {
    s16* out = (s16*)buf;

    static int cb_timer = 192;
    uint32_t step = ((uint32_t)SPU_TICK_RATE << 16) / PSP_AUDIO_RATE;

    if (spu_locked) {
        for (unsigned int i = 0; i < reqn; i++) {
            out[i << 1]     = 0;
            out[(i << 1) + 1] = 0;
        }
        return;
    }

    if (active_voices == 0) {
        // Fast path: no voices active, output silence
        memset(out, 0, reqn << 2);
        // Still advance timer for emlShimWorkTick
        u32 ticks = (reqn * SPU_TICK_RATE + PSP_AUDIO_RATE/2) / PSP_AUDIO_RATE;
        if (cb_timer > (s32)ticks) {
            cb_timer -= ticks;
        } else {
            cb_timer = 192;
        }
        return;
    }

    u16 last_m;
    if(Sound_Mono){
        for (unsigned int i = 0; i < reqn; i++) {
            spu_resample_frac += step;

            while (spu_resample_frac >= 0x10000) {
                SPU_Tick(spu_last_output);
                spu_resample_frac -= 0x10000;

                cb_timer--;
                if (!cb_timer) {
                    cb_timer = 192;
                }
            }

            last_m = (spu_last_output[0] + spu_last_output[1]) >> 1;
            out[i << 1]     = last_m;
            out[(i << 1) + 1] = last_m;
        }
    }
    else{
        for (unsigned int i = 0; i < reqn; i++) {
            spu_resample_frac += step;

            while (spu_resample_frac >= 0x10000) {
                SPU_Tick(spu_last_output);
                spu_resample_frac -= 0x10000;

                cb_timer--;
                if (!cb_timer) {
                    cb_timer = 192;
                }
            }

            out[i << 1]     = spu_last_output[0];
            out[(i << 1) + 1] = spu_last_output[1];
        }
    }
}

void SPU_Lock() {
    spu_locked = 1;
    sceKernelWaitSema(soundLock, 1, NULL);
}

bool SPU_TryLock() {
    return sceKernelPollSema(soundLock, 1) >= 0;
}

void SPU_Unlock() {
    sceKernelSignalSema(soundLock, 1);
    spu_locked = 0;
}

void SPU_Init(void (*cb)()) {

    memset(voices, 0, sizeof(voices));
    soundLock = sceKernelCreateSema("soundLock", 0, 1, 1, NULL);

    pspAudioSetChannelCallback(0, SPU_PSP_CB, NULL);
}

void SPU_Upload(u32 dst, void* src, u32 size) {
    if (!src || size == 0) return;
    if (dst > 2 * 1024 * 1024 || size > 2 * 1024 * 1024) return;
    if (dst + size > 2 * 1024 * 1024) return;

    sceKernelWaitSema(soundLock, 1, NULL);
    memcpy(&ram[dst >> 1], src, size);
    sceKernelSignalSema(soundLock, 1);
}

u16* SPU_GetRAM() {
    return ram;
}
