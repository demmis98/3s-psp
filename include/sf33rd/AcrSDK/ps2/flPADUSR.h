/**
 * @file flPADUSR.h
 * @brief Default pad configuration and hardware-to-software button mapping table.
 *
 * Provides the baseline button configuration (fltpad_config_basic)
 * and the I/O bit-position map (flpad_io_map) that translates
 * hardware pad button indices to software button bits.
 *
 * Part of the AcrSDK ps2 module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef FLPADUSR_H
#define FLPADUSR_H

#include "sf33rd/AcrSDK/common/mlPAD.h"
#include "types.h"

extern const FLPAD_CONFIG fltpad_config_basic;
extern const u32 flpad_io_map[25];

#endif
