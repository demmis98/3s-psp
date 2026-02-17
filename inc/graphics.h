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

// c++ guard
#ifdef __cplusplus
extern "C" {
#endif

// functions
void initGu();
void endGu();

void startFrame();
void endFrame();

uint32_t getBgColor();
void setBgColor(uint32_t color);

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_H_
