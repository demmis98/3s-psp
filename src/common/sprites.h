#ifndef SPRITES_H_	//  include guard
#define SPRITES_H_ 1

// libraries
#include <pspuser.h>
#include <stb_image.h>
#include <stdbool.h>

// custom
#include "math.h"

// c++ guard
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t colour;
    float x, y, z;
} ColorVertex;

/*
typedef struct {
    float u, v;
    uint32_t colour;
    float x, y, z;
} TextureVertex;
 */
 typedef struct {
    short u, v;
    uint32_t colour;
    float x, y, z;
} TextureVertex;

typedef struct {
    int width, height;
    int wRender, hRender;
    int mode;
    uint32_t * data;
} TexturePSP;

// functions
TexturePSP * loadTexture(const char * filename, bool inVram);

void setTexture(TexturePSP *texture, int tfx);
void drawTextureSet(float x1, float y1, float u1, float v1, float x2, float y2, float u2, float v2, u32 colour);

void drawTexture(TexturePSP * texture, float x1, float y1, float u1, float v1, float x2, float y2, float u2, float v2, u32 colour);
void drawTextureC(TexturePSP * texture, float x, float y, float w, float h);
void drawTextureF(TexturePSP * texture, float x, float y);

void drawTextureH(TexturePSP * texture, float x, float y, uint32_t color);

void drawRect(float x, float y, float w, float h, uint32_t colour);

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif	// SPRITES_H_
