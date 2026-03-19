/**
 * @file flps2debug.h
 * @brief Debug text rendering and system error display.
 *
 * Provides functions for printing debug text on-screen via a
 * RenderBuffer queue, setting debug text color, and displaying
 * fatal/recoverable system error messages.
 *
 * Part of the AcrSDK ps2 module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef FLPS2DEBUG_H
#define FLPS2DEBUG_H

#include "structs.h"
#include "types.h"

/// @brief Initialize debug string buffer
///
/// Allocates and initializes the buffer used by flPrintL for debug text rendering.
void flPS2DebugInit();
void flPS2SystemError(s32 error_level, s8* format, ...);
s32 flPrintL(s32 posi_x, s32 posi_y, const s8* format, ...);
s32 flPrintColor(u32 col);

#endif // FLPS2DEBUG_H
