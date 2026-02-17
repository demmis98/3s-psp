#ifndef SPRITES_H_	//  include guard
#define SPRITES_H_ 1

// libraries
#include <pspuser.h>
#include <stb_image.h>

// custom
#include "../inc/math.h"

// c++ guard
#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    unsigned short u, v;
    short x, y, z;
} Vertex;

typedef struct {
    float u, v;
    uint32_t colour;
    float x, y, z;
} TextureVertex;

typedef struct {
    int width, height;
    int wRender, hRender;
    uint32_t * data;
} Texture;

// functions
Texture * loadTexture(const char * filename);

void setTexture(Texture *texture, int tfx);
void drawTextureSet(float x1, float y1, float u1, float v1, float x2, float y2, float u2, float v2, u32 colour);

void drawTexture(Texture * texture, float x1, float y1, float u1, float v1, float x2, float y2, float u2, float v2, u32 colour);
void drawTextureC(Texture * texture, float x, float y, float w, float h);
void drawTextureF(Texture * texture, float x, float y);

void drawTextureH(Texture * texture, float x, float y, uint32_t color);

void drawRect(float x, float y, float w, float h, uint32_t colour);

// end c++ guard
#ifdef __cplusplus
}
#endif

#endif	// SPRITES_H_
