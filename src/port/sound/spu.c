/**
 * @file spu.c
 * @brief PSP SPU emulator running on a Media Engine audio callback.
 */

#include "port/sound/spu.h"
#include "common.h"
#include <pspkernel.h>
#include <pspaudiolib.h>
#include <string.h>

#define SPU_RAM_SIZE (2 * 1024 * 1024)

#define ADSR_PHASE_ATTACK 0
#define ADSR_PHASE_DECAY 1
#define ADSR_PHASE_SUSTAIN 2
#define ADSR_PHASE_RELEASE 3
#define ADSR_PHASE_STOPPED 4

#define min(a, b) ((a) < (b) ? (a) : (b))

static u16 ram[SPU_RAM_SIZE / 2];

typedef struct {
    u32 addr;       // Current byte address in SPU RAM (VAG data)
    u16 frac;
    u16 pitch;
    u16 vol;
    u16 pan;
    u16 adsr1;
    u16 adsr2;
    u8 adsr_phase;
    s32 adsr_vol;
    // VAG ADPCM decode state
    s32 vag_s1;     // Previous sample
    s32 vag_s2;     // Previous-previous sample
    s16 vag_buf[28]; // Decoded sample buffer
    u8 vag_pos;     // Current position in decoded buffer (0-27)
    u8 vag_flags;   // Last VAG block flags
} SPU_Voice;

SPU_Voice voices[48];
u64 active_voices = 0;
SceUID soundLock;

static void (*timer_cb)();

static inline s16 SPU_GetRAM(u32 addr) {
    if (addr >= SPU_RAM_SIZE)
        return 0;
    return ram[addr >> 1];
}

static inline void SPU_SetRAM(u32 addr, s16 val) {
    if (addr >= SPU_RAM_SIZE)
        return;
    ram[addr >> 1] = val;
}

// PS2 VAG ADPCM coefficient table
static const s16 vag_coeff[5][2] = {
    {   0,   0 },
    {  60,   0 },
    { 115, -52 },
    {  98, -55 },
    { 122, -60 },
};

// Decode one 16-byte VAG block into 28 s16 samples
static void vag_decode_block(SPU_Voice* v) {
    u32 ba = v->addr;
    // First two bytes: predict/shift and flags (packed in one u16 word)
    u16 header_word = (u16)SPU_GetRAM(ba);
    u8 predict_shift = (u8)(header_word & 0xFF);
    u8 flags = (u8)((header_word >> 8) & 0xFF);

    u8 predict = (predict_shift >> 4) & 0xF;
    u8 shift = predict_shift & 0xF;
    if (predict > 4) predict = 0;

    s32 c1 = vag_coeff[predict][0];
    s32 c2 = vag_coeff[predict][1];
    v->vag_flags = flags;

    // 14 data bytes (7 u16 words at ba+2..ba+14) = 28 nibbles = 28 samples
    for (int i = 0; i < 28; i++) {
        int byte_offset = 2 + (i / 2);       // byte index within the 16-byte block
        u16 w = (u16)SPU_GetRAM(ba + (byte_offset & ~1));
        u8 bv = (byte_offset & 1) ? (u8)(w >> 8) : (u8)(w & 0xFF);

        s32 nib = (i & 1) ? ((bv >> 4) & 0xF) : (bv & 0xF);
        if (nib >= 8) nib -= 16;

        s32 sample = (nib << (12 - shift)) + ((c1 * v->vag_s1 + c2 * v->vag_s2) >> 6);
        if (sample > 32767) sample = 32767;
        if (sample < -32768) sample = -32768;

        v->vag_buf[i] = (s16)sample;
        v->vag_s2 = v->vag_s1;
        v->vag_s1 = sample;
    }

    v->vag_pos = 0;
    v->addr += 16; // Advance to next 16-byte block
}

// Get next decoded sample from a voice
static inline s16 vag_next_sample(SPU_Voice* v) {
    if (v->vag_pos >= 28) {
        vag_decode_block(v);
        if (v->vag_flags & 1) {
            // End-of-sample flag
            v->adsr_phase = ADSR_PHASE_STOPPED;
            active_voices &= ~(1ULL << (v - voices));
            return 0;
        }
    }
    return v->vag_buf[v->vag_pos++];
}


void SPU_Write(u32 addr, s16 data) {
    if (addr < 0x2000) {
        if (addr >= 0x1C00) {
            u32 voice = (addr - 0x1C00) / 0x10;
            switch (addr & 0xF) {
            case 0x0:
                voices[voice].adsr1 = data;
                break;
            case 0x2:
                voices[voice].adsr2 = data;
                break;
            case 0x4:
                voices[voice].pitch = data;
                break;
            case 0x6:
                voices[voice].vol = data;
                break;
            case 0xA:
                voices[voice].pan = data;
                break;
            case 0xE:
                voices[voice].addr = (u32)data << 16;
                // Trigger KeyOn
                active_voices |= (1ULL << voice);
                voices[voice].adsr_phase = ADSR_PHASE_ATTACK;
                voices[voice].adsr_vol = 0;
                break;
            }
        } else if (addr >= 0x1800) {
            u32 voice = (addr - 0x1800) / 0x10;
            if ((addr & 0xF) == 0xE) {
                // KeyOff
                voices[voice].adsr_phase = ADSR_PHASE_RELEASE;
            }
        }
    }
}

static inline s32 envelope_step(SPU_Voice* v) {
    switch (v->adsr_phase) {
    case ADSR_PHASE_ATTACK: {
        u16 ar = (v->adsr1 >> 8) & 0x7F;
        // Scale: ar=0 → fast attack (~128 ticks), ar=127 → slow (~16384 ticks)
        s32 inc = 0x7FFF / (128 + ar * 128);
        if (inc < 1) inc = 1;
        v->adsr_vol += inc;
        if (v->adsr_vol >= 0x7FFF) {
            v->adsr_vol = 0x7FFF;
            v->adsr_phase = ADSR_PHASE_DECAY;
        }
        break;
    }
    case ADSR_PHASE_DECAY: {
        u16 dr = (v->adsr1 >> 4) & 0xF;
        s32 sl = (v->adsr1 & 0xF) << 11;
        s32 dec = (0x7FFF - sl) / (256 + dr * 256);
        if (dec < 1) dec = 1;
        v->adsr_vol -= dec;
        if (v->adsr_vol <= sl) {
            v->adsr_vol = sl;
            v->adsr_phase = ADSR_PHASE_SUSTAIN;
        }
        break;
    }
    case ADSR_PHASE_SUSTAIN: {
        u16 sr = (v->adsr2 >> 6) & 0x7F;
        if (sr) {
            s32 step = 0x7FFF / (256 + sr * 128);
            if (step < 1) step = 1;
            v->adsr_vol += (v->adsr2 & 0x8000) ? step : -step;
            if (v->adsr_vol <= 0) {
                v->adsr_vol = 0;
                v->adsr_phase = ADSR_PHASE_STOPPED;
                active_voices &= ~(1ULL << (v - voices));
            } else if (v->adsr_vol >= 0x7FFF) {
                v->adsr_vol = 0x7FFF;
            }
        }
        break;
    }
    case ADSR_PHASE_RELEASE: {
        u16 rr = v->adsr2 & 0x1F;
        s32 dec = 0x7FFF / (256 + rr * 256);
        if (dec < 1) dec = 1;
        v->adsr_vol -= dec;
        if (v->adsr_vol <= 0) {
            v->adsr_vol = 0;
            v->adsr_phase = ADSR_PHASE_STOPPED;
            active_voices &= ~(1ULL << (v - voices));
        }
        break;
    }
    }
    return v->adsr_vol;
}

void SPU_Tick(s16* output) {
    s32 acc[2] = {};

    uint64_t mask = active_voices;
    while (mask) {
        int i = __builtin_ctzll(mask);
        mask &= mask - 1;
        SPU_Voice* v = &voices[i];

        if (v->adsr_phase == ADSR_PHASE_STOPPED)
            continue;

        // Get next VAG-decoded sample
        s16 sample = vag_next_sample(v);
        if (v->adsr_phase == ADSR_PHASE_STOPPED)
            continue;

        // TODO: implement proper ADSR envelope timing
        // Apply volume (vol is 14-bit 0-0x3FFF) and panning (pan is 15-bit 0-0x7FFF)
        s32 scaled = ((s32)sample * (s32)v->vol) >> 14;
        s32 left  = (scaled * (s32)(0x7FFF - v->pan)) >> 15;
        s32 right = (scaled * (s32)v->pan) >> 15;

        acc[0] += left;
        acc[1] += right;
    }

    acc[0] = (acc[0] < -32768) ? -32768 : ((acc[0] > 32767) ? 32767 : acc[0]);
    acc[1] = (acc[1] < -32768) ? -32768 : ((acc[1] > 32767) ? 32767 : acc[1]);

    output[0] = (s16)acc[0];
    output[1] = (s16)acc[1];
}

static SceUID lock_owner = 0;
static int lock_count = 0;

void SPU_Lock() {
    SceUID me = sceKernelGetThreadId();
    if (lock_owner == me) {
        lock_count++;
        return;
    }
    sceKernelWaitSema(soundLock, 1, 0);
    lock_owner = me;
    lock_count = 1;
}

void SPU_Unlock() {
    SceUID me = sceKernelGetThreadId();
    if (lock_owner == me) {
        lock_count--;
        if (lock_count == 0) {
            lock_owner = 0;
            sceKernelSignalSema(soundLock, 1);
        }
    }
}

void SPU_PSP_CB(void* buf, unsigned int reqn, void* pdata) {
    u32 samples_per_channel = reqn;
    int remaining = samples_per_channel;
    static s16 outbuf[4096] = {};
    static int cb_timer = 192;

    while (remaining) {
        u32 batch_count = min(remaining, 256);

        SPU_Lock();

        s16* p = outbuf;
        for (u32 i = 0; i < batch_count; i++) {
            SPU_Tick(p);
            p += 2;

            cb_timer--;
            if (!cb_timer) {
                if (timer_cb) {
                    timer_cb();
                }
                cb_timer = 192;
            }
        }

        SPU_Unlock();

        memcpy((s16*)buf + (samples_per_channel - remaining) * 2, outbuf, batch_count * sizeof(s16) * 2);
        remaining -= batch_count;
    }
}

static void nullcb() {}

void SPU_Init(void (*cb)()) {
    timer_cb = cb;
    if (!cb) {
        timer_cb = nullcb;
    }

    memset(voices, 0, sizeof(voices));
    soundLock = sceKernelCreateSema("soundLock", 0, 1, 1, NULL);

    // pspAudioInit(); // Now handled centrally by pspmain.c
    pspAudioSetChannelCallback(0, SPU_PSP_CB, NULL);
    pspAudioSetVolume(0, 0x8000, 0x8000);
}

void SPU_Upload(u32 dst, void* src, u32 size) {
    SPU_Lock();

    memcpy(&ram[dst >> 1], src, size);
    printf("[SPU] Upload: dst=0x%x size=%u first_word=0x%04x\n", dst, size, ram[dst >> 1]);

    SPU_Unlock();
}

void SPU_VoiceStart(int vnum, u32 start_addr) {
    if (vnum < 0 || vnum >= 48) return;
    SPU_Voice* v = &voices[vnum];
    // start_addr comes as a u16 index (s_addr >> 1 from emlShim),
    // but SPU_GetRAM expects byte addresses (it does >> 1 internally).
    // Convert back to byte address to avoid double-shifting.
    v->addr = start_addr * 2;
    v->frac = 0;
    v->vag_s1 = 0;
    v->vag_s2 = 0;
    v->vag_pos = 28; // Force decode on first sample
    v->vag_flags = 0;
    v->adsr_phase = ADSR_PHASE_ATTACK;
    v->adsr_vol = 0;
    active_voices |= (1ULL << vnum);
    printf("[SPU] VoiceStart: v=%d addr=0x%x raw=0x%x first_word=0x%04x\n", vnum, v->addr, start_addr, SPU_GetRAM(v->addr));
}

void SPU_VoiceGetConf(int vnum, struct SPUVConf* conf) {
    if (vnum < 0 || vnum >= 48 || !conf) return;
    SPU_Voice* v = &voices[vnum];
    conf->pitch = v->pitch;
    conf->voll = v->vol;
    conf->volr = v->vol;
    conf->adsr1 = v->adsr1;
    conf->adsr2 = v->adsr2;
    conf->pmon = 0;
}

void SPU_VoiceSetConf(int vnum, struct SPUVConf* conf) {
    if (vnum < 0 || vnum >= 48 || !conf) return;
    SPU_Voice* v = &voices[vnum];
    v->pitch = (u16)conf->pitch;
    u32 vl = conf->voll;
    u32 vr = conf->volr;
    v->vol = (u16)((vl + vr) / 2);
    if (vl + vr > 0) {
        v->pan = (u16)((vr * 0x7FFF) / (vl + vr));
    } else {
        v->pan = 0x3FFF;
    }
    v->adsr1 = conf->adsr1;
    v->adsr2 = conf->adsr2;
}

bool SPU_VoiceIsFinished(int vnum) {
    if (vnum < 0 || vnum >= 48) return true;
    return voices[vnum].adsr_phase == ADSR_PHASE_STOPPED;
}

void SPU_VoiceKeyOff(int vnum) {
    if (vnum < 0 || vnum >= 48) return;
    if (voices[vnum].adsr_phase != ADSR_PHASE_STOPPED) {
        voices[vnum].adsr_phase = ADSR_PHASE_RELEASE;
    }
}

void SPU_VoiceStop(int vnum) {
    if (vnum < 0 || vnum >= 48) return;
    voices[vnum].adsr_phase = ADSR_PHASE_STOPPED;
    voices[vnum].adsr_vol = 0;
    active_voices &= ~(1ULL << vnum);
}
