/**
 * @file adx_decoder.h
 * @brief CRI ADX audio decoder types and API.
 */
#ifndef ADX_DECODER_H
#define ADX_DECODER_H

#include "types.h"
#include <stdbool.h>
#include <stddef.h>

#define ADX_MAX_CHANNELS 2

typedef struct {
    s32 prev1;
    s32 prev2;
} ADXChannelState;

typedef struct {
    s32 channels;
    s32 sample_rate;
    s32 total_samples;
    s32 block_size;        // Size of one block in bytes per channel (usually 18)
    s32 frame_size;        // Size of one frame (block_size * channels)
    s32 samples_per_block; // Usually 32
    s32 data_offset;
    s32 coeff1;
    s32 coeff2;
    ADXChannelState ch_state[ADX_MAX_CHANNELS];
} ADXContext;

// Initializes the decoder context by parsing the header.
// Returns 0 on success, -1 on failure.
int ADX_InitContext(ADXContext* ctx, const u8* header, size_t header_size);

// Decodes one or more full frames (interleaved blocks) from input.
// in_buffer: Pointer to ADX data (after header).
// in_size: Available bytes in in_buffer.
// out_buffer: Pointer to write decoded interleaved 16-bit PCM samples.
// out_samples: [In/Out] On input, capacity of out_buffer in samples (s16 elements).
//              On output, number of samples written (total, i.e. samples_per_frame * channels * frames_decoded).
// bytes_consumed: [Out] Number of bytes read from in_buffer.
// Returns 0 on success, -1 on error.
int ADX_Decode(ADXContext* ctx, const u8* in_buffer, size_t in_size, s16* out_buffer, s32* out_samples,
               s32* bytes_consumed);

#endif
