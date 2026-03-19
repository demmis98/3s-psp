/**
 * @file flps2render.h
 * @brief Render state management and texture register setup.
 *
 * Provides the render state dispatch (flSetRenderState) for setting
 * texture stages and clear color, the GS texture register builder,
 * and the Z-buffer depth conversion function.
 *
 * Part of the AcrSDK ps2 module.
 * Originally from the PS2 SDK abstraction layer.
 */
#ifndef FLPS2RENDER_H
#define FLPS2RENDER_H

#include "structs.h"
#include "types.h"

#define MIP_TBP_SIZE 6
#define MIP_DBW_SIZE 6

s32 flSetRenderState(enum _FLSETRENDERSTATE func, u32 value);
f32 flPS2ConvScreenFZ(f32 z);
s32 flPS2SetTextureRegister(u32 th, u64* texA, u64* tex1, u64* tex0, u64* clamp, u64* miptbp1, u64* miptbp2,
                            u32 render_ope);

#endif
