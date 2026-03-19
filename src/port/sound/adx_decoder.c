/**
 * @file adx_decoder.c
 * @brief CRI ADX ADPCM frame decoder.
 *
 * Implements the standard CRI ADX decoding algorithm: parses ADX
 * headers (v3/v4), computes prediction coefficients from a 500 Hz
 * cutoff filter, and decodes 4-bit ADPCM blocks into 16-bit PCM
 * with per-channel state tracking.
 */
#include "port/sound/adx_decoder.h"
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static u16 read_u16be(const u8* p) {
    return (u16)((p[0] << 8) | p[1]);
}

static u32 read_u32be(const u8* p) {
    return (u32)((p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3]);
}

static s32 sign_extend_4bit(int val) {
    return (val & 8) ? (val - 16) : val;
}

static s16 clamp16(s32 val) {
    if (val > 32767) {
        return 32767;
    }
    if (val < -32768) {
        return -32768;
    }
    return (s16)val;
}

int ADX_InitContext(ADXContext* ctx, const u8* header, size_t header_size) {
    if (!ctx || !header || header_size < 16) {
        return -1;
    }

    // Check magic (0x80)
    // header[0] is 0x80. header[1] is encoding (usually 0x03)
    if (header[0] != 0x80) {
        return -1;
    }

    // The offset to data is usually stored at 0x02 (u16), representing the offset to the copyright string.
    // The data starts after the header + copyright + padding.
    // Typically, data_offset = read_u16be(header + 2) + 4.
    u16 copyright_offset = read_u16be(header + 2);
    ctx->data_offset = (s32)copyright_offset + 4;

    // Safety check - though we often Init with just the first few bytes,
    // we should be careful if we were to read past header_size.
    // For now we trust the caller provided enough of the header (at least 16 bytes).

    ctx->channels = header[7];
    if (ctx->channels <= 0 || ctx->channels > ADX_MAX_CHANNELS) {
        return -1;
    }

    ctx->sample_rate = (s32)read_u32be(header + 8);
    ctx->total_samples = (s32)read_u32be(header + 12);
    ctx->block_size = header[5];

    if (ctx->block_size < 3) {
        return -1; // Must be at least scale (2 bytes) + 1 byte of data
    }

    ctx->samples_per_block = (ctx->block_size - 2) * 2;
    ctx->frame_size = ctx->block_size * ctx->channels;

    // Calculate coefficients based on standard CRI ADX algorithm (cutoff 500Hz)
    double cutoff = 500.0;
    double sample_rate = (double)ctx->sample_rate;
    if (sample_rate < 1000.0) {
        sample_rate = 1000.0; // Avoid division by zero/weirdness
    }

    double w = 2.0 * M_PI * cutoff / sample_rate;
    double x = sqrt(2.0) - cos(w);
    double y = sqrt(2.0) - 1.0;
    double z = (x - sqrt((x + y) * (x - y))) / y;

    ctx->coeff1 = (s32)(z * 8192.0);
    ctx->coeff2 = (s32)(z * z * -4096.0);

    // Reset channel state
    memset(ctx->ch_state, 0, sizeof(ctx->ch_state));

    return 0;
}

static void decode_block(ADXContext* ctx, int channel, const u8* block, s16* out, int stride) {
    int scale = read_u16be(block);
    int c1 = ctx->coeff1;
    int c2 = ctx->coeff2;
    int p1 = ctx->ch_state[channel].prev1;
    int p2 = ctx->ch_state[channel].prev2;

    const u8* data = block + 2;
    for (int i = 0; i < ctx->samples_per_block / 2; i++) {
        int byte = data[i];

        // High nibble
        int d1 = sign_extend_4bit(byte >> 4);
        int s1 = (d1 * scale) + ((c1 * p1 + c2 * p2) >> 12);
        s16 val1 = clamp16(s1);
        *out = val1;
        out += stride;
        p2 = p1;
        p1 = (int)val1;

        // Low nibble
        int d2 = sign_extend_4bit(byte & 0x0F);
        int s2 = (d2 * scale) + ((c1 * p1 + c2 * p2) >> 12);
        s16 val2 = clamp16(s2);
        *out = val2;
        out += stride;
        p2 = p1;
        p1 = (int)val2;
    }

    ctx->ch_state[channel].prev1 = p1;
    ctx->ch_state[channel].prev2 = p2;
}

int ADX_Decode(ADXContext* ctx, const u8* in_buffer, size_t in_size, s16* out_buffer, s32* out_samples,
               s32* bytes_consumed) {
    if (!ctx || !in_buffer || !out_buffer || !out_samples || !bytes_consumed) {
        return -1;
    }

    s32 max_out_samples = *out_samples; // Total samples capacity (all channels interleaved)

    if (ctx->frame_size == 0 || ctx->samples_per_block == 0) {
        return -1;
    }

    // Calculate how many full frames we can process from input
    s32 frames_available = (s32)(in_size / ctx->frame_size);

    // Calculate how many frames fit in output buffer
    // Each frame produces (samples_per_block * channels) samples
    s32 samples_per_frame_total = ctx->samples_per_block * ctx->channels;
    s32 frames_that_fit = max_out_samples / samples_per_frame_total;

    s32 frames_to_decode = (frames_available < frames_that_fit) ? frames_available : frames_that_fit;

    s32 total_samples_written = 0;
    s32 total_bytes = 0;

    const u8* src = in_buffer;
    s16* dst = out_buffer;

    for (int f = 0; f < frames_to_decode; f++) {
        // For stereo: Block L (18 bytes), then Block R (18 bytes).
        // decode_block writes interleaved samples to dst.
        for (int ch = 0; ch < ctx->channels; ch++) {
            decode_block(ctx, ch, src, dst + ch, ctx->channels);
            src += ctx->block_size;
        }

        s32 samples_produced = samples_per_frame_total;
        dst += samples_produced;
        total_samples_written += samples_produced;
        total_bytes += ctx->frame_size;
    }

    *out_samples = total_samples_written;
    *bytes_consumed = total_bytes;

    return 0;
}
