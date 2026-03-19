#ifndef GRAPHICS_H_ //  include guard
#define GRAPHICS_H_ 1

// libraries
#include <memory.h>
#include <pspdisplay.h>
#include <pspgu.h>

// custom
#include "sprites.h"

// constants
#define BUFFER_WIDTH 512
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 272

// PSP Fullscreen Letterbox scaling (272 / 224 aspect ratio preservation)
#define PSP_SCALE    1.2142857f
#define PSP_OFFSET_X 6.857143f
#define SCALE_X(x) (((x) * PSP_SCALE) + PSP_OFFSET_X)
#define SCALE_Y(y) ((y) * PSP_SCALE)

// c++ guard
#ifdef __cplusplus
extern "C" {
#endif

// functions
void initGu();
void endGu();

void startFrame();
void endFrame();
void endFrameDebug();

uint32_t getBgColor();
void setBgColor(uint32_t color);
int getGuInit();

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_H_
