/**
 * @file adx.c
 * @brief CRI ADX audio playback engine with loop support.
 */
#include "port/sound/adx.h"
#include "common.h"
#include "port/sound/adx_decoder.h"
#include "psp/files.h"

#include <pspkernel.h>
#include <pspaudiolib.h>

#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define ADX_POOL_BUF_SIZE (512 * 1024)
#define ADX_POOL_COUNT 10
static uint8_t adx_pool[ADX_POOL_COUNT][ADX_POOL_BUF_SIZE];
static bool adx_pool_used[ADX_POOL_COUNT] = { false };
static void* pool_alloc(size_t size) {
    if (size <= ADX_POOL_BUF_SIZE) {
        for (int i = 0; i < ADX_POOL_COUNT; i++) {
            if (!adx_pool_used[i]) { adx_pool_used[i] = true; return adx_pool[i]; }
        }
    }
    return malloc(size);
}
static void pool_free(void* ptr) {
    uint8_t* p = (uint8_t*)ptr;
    uint8_t* pool_start = &adx_pool[0][0];
    uint8_t* pool_end = &adx_pool[ADX_POOL_COUNT - 1][ADX_POOL_BUF_SIZE];
    if (p >= pool_start && p < pool_end) {
        int index = (int)((p - pool_start) / ADX_POOL_BUF_SIZE);
        adx_pool_used[index] = false; return;
    }
    free(ptr);
}

#define PSP_OUTPUT_RATE 44100
#define N_CHANNELS 2
#define BYTES_PER_SAMPLE 2
#define MIN_QUEUED_DATA_MS 400
#define MIN_QUEUED_DATA (int)((float)PSP_OUTPUT_RATE * MIN_QUEUED_DATA_MS / 1000 * N_CHANNELS * BYTES_PER_SAMPLE)
#define TRACKS_MAX 10
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

#define ADX_RB16(p) ((uint16_t)(((const uint8_t*)(p))[0] << 8 | ((const uint8_t*)(p))[1]))
#define ADX_RB32(p) ((uint32_t)(((const uint8_t*)(p))[0] << 24 | ((const uint8_t*)(p))[1] << 16 | ((const uint8_t*)(p))[2] << 8 | ((const uint8_t*)(p))[3]))

typedef struct ADXLoopInfo {
    bool looping_enabled;
    int start_sample;
    int end_sample;
    uint8_t* data;
    int data_size;
    int position;
} ADXLoopInfo;

typedef struct ADXTrack {
    int size;
    uint8_t* data;
    bool should_free_data_after_use;
    int used_bytes;
    int processed_samples;
    ADXLoopInfo loop_info;
    ADXContext ctx;
} ADXTrack;

static ADXTrack tracks[TRACKS_MAX] = { 0 };
static int num_tracks = 0;
static int first_track_index = 0;
static bool has_tracks = false;

// Ring buffer
#define ADX_RING_SIZE (128 * 1024)
static uint8_t adx_ring[ADX_RING_SIZE];
static int adx_ring_head = 0;
static int adx_ring_tail = 0;
static bool adx_paused = false;
static float current_gain = 1.0f;
static int adx_source_rate = 44100;  // Detected from ADX file header
static uint32_t adx_resample_frac = 0;  // Fractional accumulator for resampling

static int stream_queued() {
    int count = adx_ring_tail - adx_ring_head;
    if (count < 0) count += ADX_RING_SIZE;
    return count;
}
static int stream_data_needed() {
    return MIN_QUEUED_DATA - stream_queued();
}
static bool stream_needs_data() {
    return stream_data_needed() > 0;
}
static bool stream_is_empty() {
    return stream_queued() <= 0;
}
static void stream_put(void* data, int len) {
    uint8_t* p = (uint8_t*)data;
    for (int i=0; i<len; i++) {
        adx_ring[adx_ring_tail] = p[i];
        adx_ring_tail = (adx_ring_tail + 1) % ADX_RING_SIZE;
    }
}
static void stream_clear() {
    adx_ring_head = adx_ring_tail = 0;
}

// Read one stereo sample (4 bytes) from the ring buffer
static inline void stream_read_sample(int16_t* left, int16_t* right) {
    uint8_t b1 = adx_ring[adx_ring_head];
    adx_ring_head = (adx_ring_head + 1) % ADX_RING_SIZE;
    uint8_t b2 = adx_ring[adx_ring_head];
    adx_ring_head = (adx_ring_head + 1) % ADX_RING_SIZE;
    *left = (int16_t)(b1 | (b2 << 8));
    uint8_t b3 = adx_ring[adx_ring_head];
    adx_ring_head = (adx_ring_head + 1) % ADX_RING_SIZE;
    uint8_t b4 = adx_ring[adx_ring_head];
    adx_ring_head = (adx_ring_head + 1) % ADX_RING_SIZE;
    *right = (int16_t)(b3 | (b4 << 8));
}

void ADX_PSP_CB(void* buf, unsigned int reqn, void* pdata) {
    int16_t* out = (int16_t*)buf;
    int out_frames = reqn;  // reqn = number of stereo frames
    int bytes_per_frame = N_CHANNELS * BYTES_PER_SAMPLE;  // 4 bytes per stereo frame

    if (adx_paused || stream_queued() < bytes_per_frame * 2) {
        memset(buf, 0, out_frames * bytes_per_frame);
        return;
    }

    // Resample from adx_source_rate to PSP_OUTPUT_RATE using fractional accumulator
    // step = source_rate / output_rate, stored as 16.16 fixed-point
    uint32_t step = ((uint32_t)adx_source_rate << 16) / PSP_OUTPUT_RATE;
    static int16_t last_l = 0, last_r = 0;

    for (int i = 0; i < out_frames; i++) {
        // Accumulate fractional position
        adx_resample_frac += step;

        // Consume source samples as needed (integer part of accumulator)
        while (adx_resample_frac >= 0x10000) {
            if (stream_queued() < bytes_per_frame) break;
            stream_read_sample(&last_l, &last_r);
            adx_resample_frac -= 0x10000;
        }

        out[i * 2]     = (int16_t)(last_l * current_gain);
        out[i * 2 + 1] = (int16_t)(last_r * current_gain);
    }
}

static void* load_file(int file_id, int* size) {
    const char* filepath = getFile(file_id);
    if (!filepath) { *size = 0; return NULL; }
    SceUID fd = sceIoOpen(filepath, PSP_O_RDONLY, 0777);
    if (fd < 0) { *size = 0; return NULL; }
    const int file_size = sceIoLseek32(fd, 0, PSP_SEEK_END);
    sceIoLseek32(fd, 0, PSP_SEEK_SET);
    *size = file_size;
    const unsigned int sectors = (file_size + 2048 - 1) / 2048;
    const size_t buff_size = (size_t)sectors * 2048;
    void* buff = pool_alloc(buff_size);
    if (!buff) {
        sceIoClose(fd);
        *size = 0;
        return NULL;
    }
    sceIoRead(fd, buff, file_size);
    sceIoClose(fd);
    return buff;
}

static bool track_reached_eof(ADXTrack* track) {
    if (track->ctx.frame_size > 0) return (track->size - (int)track->used_bytes) < track->ctx.frame_size;
    return (track->size - (int)track->used_bytes) <= 0;
}
static bool track_loop_filled(ADXTrack* track) {
    if (track->loop_info.looping_enabled) return track->processed_samples >= track->loop_info.end_sample;
    return false;
}
static bool track_needs_decoding(ADXTrack* track) {
    if (track->loop_info.looping_enabled) return !track_loop_filled(track);
    return !track_reached_eof(track);
}
static bool track_exhausted(ADXTrack* track) {
    if (track->loop_info.looping_enabled) return false;
    return track_reached_eof(track);
}

static int track_add_samples_to_loop(ADXTrack* track, uint8_t* buf, int num_samples) {
    ADXLoopInfo* loop_info = &track->loop_info;
    if (!loop_info->looping_enabled) return 0;
    const int buf_sample_start = MAX(loop_info->start_sample - track->processed_samples, 0);
    const int buf_sample_end = MIN(loop_info->end_sample - track->processed_samples, num_samples);
    if (buf_sample_end > buf_sample_start) {
        const int buf_start = buf_sample_start * N_CHANNELS * BYTES_PER_SAMPLE;
        const int buf_end = buf_sample_end * N_CHANNELS * BYTES_PER_SAMPLE;
        const int buf_len = buf_end - buf_start;
        if (loop_info->position + buf_len <= loop_info->data_size) {
            memcpy(loop_info->data + loop_info->position, buf + buf_start, buf_len);
            loop_info->position += buf_len;
            if (loop_info->position == loop_info->data_size) loop_info->position = 0;
        }
    }
    const int overflow = MAX(track->processed_samples + num_samples - loop_info->end_sample, 0);
    track->processed_samples += num_samples;
    return overflow;
}

static void loop_info_init(ADXLoopInfo* info, const uint8_t* data) {
    const uint8_t version = data[0x12];
    switch (version) {
    case 3: {
        const uint16_t loop_enabled_16 = ADX_RB16(data + 0x16);
        if (loop_enabled_16 == 1) {
            info->looping_enabled = true;
            info->start_sample = ADX_RB32(data + 0x1C);
            info->end_sample = ADX_RB32(data + 0x24);
        }
        break;
    }
    case 4: {
        const uint32_t loop_enabled_32 = ADX_RB32(data + 0x24);
        if (loop_enabled_32 == 1) {
            info->looping_enabled = true;
            info->start_sample = ADX_RB32(data + 0x28);
            info->end_sample = ADX_RB32(data + 0x30);
        }
        break;
    }
    }
    if (info->looping_enabled) {
        info->data_size = (info->end_sample - info->start_sample) * BYTES_PER_SAMPLE * N_CHANNELS;
        info->data = malloc(info->data_size);
        info->position = 0;
    }
}

static void loop_info_destroy(ADXLoopInfo* info) {
    if (info->looping_enabled) free(info->data);
    memset(info, 0, sizeof(*info));
}

static void process_track(ADXTrack* track) {
    int16_t decode_buf[2048 * N_CHANNELS];
    const int max_samples = 2048 * N_CHANNELS;
    while (stream_needs_data() && track_needs_decoding(track)) {
        int samples_to_decode = max_samples;
        int bytes_consumed = 0;
        int ret = ADX_Decode(&track->ctx, track->data + track->used_bytes, track->size - track->used_bytes, decode_buf, &samples_to_decode, &bytes_consumed);
        if (ret < 0) break;
        if (samples_to_decode == 0) break;
        track->used_bytes += bytes_consumed;
        int samples_per_channel = samples_to_decode / track->ctx.channels;
        int overflow = track_add_samples_to_loop(track, (uint8_t*)decode_buf, samples_per_channel);
        int samples_overflow = overflow * track->ctx.channels;
        int samples_to_queue = samples_to_decode - samples_overflow;
        if (samples_to_queue > 0) {
            int bytes_to_queue = samples_to_queue * sizeof(int16_t);
            stream_put(decode_buf, bytes_to_queue);
        }
    }
    while (track_loop_filled(track) && stream_needs_data()) {
        const int available_data = track->loop_info.data_size - track->loop_info.position;
        const int data_to_queue = MIN(stream_data_needed(), available_data);
        stream_put(track->loop_info.data + track->loop_info.position, data_to_queue);
        track->loop_info.position += data_to_queue;
        if (track->loop_info.position == track->loop_info.data_size) track->loop_info.position = 0;
    }
}

static void track_init(ADXTrack* track, int file_id, void* buf, size_t buf_size, bool looping_allowed) {
    if (file_id != -1) {
        track->data = load_file(file_id, &track->size);
        if (!track->data) return;
        track->should_free_data_after_use = true;
    } else {
        track->data = buf;
        track->size = (int)buf_size;
        track->should_free_data_after_use = false;
    }
    if (ADX_InitContext(&track->ctx, track->data, track->size) < 0) return;
    adx_source_rate = track->ctx.sample_rate;  // Use file's native sample rate for resampling
    printf("[ADX] source_rate=%d channels=%d\n", adx_source_rate, track->ctx.channels);
    adx_resample_frac = 0;  // Reset resampler state
    track->used_bytes = track->ctx.data_offset;
    track->processed_samples = 0;
    if (looping_allowed) loop_info_init(&track->loop_info, track->data);
    process_track(track);
}

static void track_destroy(ADXTrack* track) {
    loop_info_destroy(&track->loop_info);
    if (track->should_free_data_after_use) pool_free(track->data);
    memset(track, 0, sizeof(*track));
}

static ADXTrack* alloc_track() {
    const int index = (first_track_index + num_tracks) % TRACKS_MAX;
    num_tracks += 1;
    has_tracks = true;
    return &tracks[index];
}

void ADX_ProcessTracks() {
    if (!stream_needs_data()) return;
    const int first_track_index_old = first_track_index;
    const int num_tracks_old = num_tracks;
    for (int i = 0; i < num_tracks_old; i++) {
        const int j = (first_track_index_old + i) % TRACKS_MAX;
        ADXTrack* track = &tracks[j];
        process_track(track);
        if (!track_exhausted(track)) break;
        track_destroy(track);
        num_tracks -= 1;
        if (num_tracks > 0) first_track_index += 1;
        else first_track_index = 0;
    }
}

void ADX_Init() {
    pspAudioSetChannelCallback(1, ADX_PSP_CB, NULL);
}

void ADX_Exit() {
    ADX_Stop();
}

void ADX_Stop() {
    ADX_Pause(true);
    stream_clear();
    for (int i = 0; i < num_tracks; i++) {
        const int j = (first_track_index + i) % TRACKS_MAX;
        track_destroy(&tracks[j]);
    }
    num_tracks = 0;
    first_track_index = 0;
    has_tracks = false;
}

int ADX_IsPaused() { return adx_paused; }
void ADX_Pause(int pause) { adx_paused = pause; }

void ADX_StartMem(void* buf, size_t size) {
    ADX_Stop();
    ADXTrack* track = alloc_track();
    track_init(track, -1, buf, size, true);
}

int ADX_GetNumFiles() { return num_tracks; }
void ADX_EntryAfs(int file_id) { ADXTrack* track = alloc_track(); track_init(track, file_id, NULL, 0, false); }
void ADX_StartSeamless() { ADX_Pause(false); }
void ADX_ResetEntry() {}

void ADX_StartAfs(int file_id) {
    ADX_Stop();
    ADXTrack* track = alloc_track();
    track_init(track, file_id, NULL, 0, true);
}

void ADX_SetOutVol(int volume) {
    current_gain = powf(10.0f, (float)volume / 200.0f);
}

void ADX_SetMono(bool mono) { (void)mono; }

ADXState ADX_GetState() {
    if (!has_tracks) return ADX_STATE_STOP;
    if (stream_is_empty()) return ADX_STATE_PLAYEND;
    else return ADX_IsPaused() ? ADX_STATE_STOP : ADX_STATE_PLAYING;
}
