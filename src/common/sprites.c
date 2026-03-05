#include "sprites.h"
#include "graphics.h"

#include <pspuser.h>
#include <pspdisplay.h>
#include <pspgu.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

TexturePSP * loadTexture(const char * filename, bool inVram) {
    TexturePSP * texture = (TexturePSP *) calloc(1, sizeof(TexturePSP));
    void *temp_tex;
    //TexturePSP *texture = (Texture*)sceGuGetMemory(1,  sizeof(Texture));

    texture->data = (uint32_t *) stbi_load(filename, &(texture->width), &(texture->height), NULL, STBI_rgb_alpha);
    texture->wRender = nextPowTwo(texture->width);
    texture->hRender = nextPowTwo(texture->height);
    texture->mode = GU_PSM_8888;

    sceKernelDcacheWritebackInvalidateAll();
    
    if(inVram){
        temp_tex = guGetStaticVramTexture(texture->wRender, texture->hRender, texture->mode);
        sceGuCopyImage(texture->mode, 0, 0, texture->width, texture->height, texture->wRender, texture->data, 0, 0, texture->wRender, temp_tex);
        free(texture->data);
        texture->data = temp_tex;
        sceKernelDcacheWritebackInvalidateAll();
    }
    // Make sure the texture cache is reloaded

    return texture;
}

void setTexture(TexturePSP *texture, int tfx){
    sceGuTexMode(texture->mode, 0, 0, GU_FALSE);
    sceGuTexFunc(tfx, GU_TCC_RGBA);
    sceGuTexImage(0, texture->wRender, texture->hRender, texture->width, texture->data);
}

void drawTextureSet(float x1, float y1, float u1, float v1, float x2, float y2, float u2, float v2, u32 colour) {
    TextureVertex *vertices = (TextureVertex*)sceGuGetMemory(2 * sizeof(TextureVertex));
    //static TextureVertex vertices[2];

    vertices[0].u = u1;
    vertices[0].v = v1;
    vertices[0].colour = colour;
    vertices[0].x = x1;
    vertices[0].y = y1;
    vertices[0].z = 0.0f;

    vertices[1].u = u2;
    vertices[1].v = v2;
    vertices[1].colour = colour;
    vertices[1].x = x2;
    vertices[1].y = y2;
    vertices[1].z = 0.0f;

    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
}

void drawTexture(TexturePSP * texture, float x1, float y1, float u1, float v1, float x2, float y2, float u2, float v2, u32 colour) {
    TextureVertex *vertices = (TextureVertex*)sceGuGetMemory(2 * sizeof(TextureVertex));
    //static TextureVertex vertices[2];

    vertices[0].u = u1;
    vertices[0].v = v1;
    vertices[0].colour = colour;
    vertices[0].x = x1;
    vertices[0].y = y1;
    vertices[0].z = 0.0f;

    vertices[1].u = u2;
    vertices[1].v = v2;
    vertices[1].colour = colour;
    vertices[1].x = x2;
    vertices[1].y = y2;
    vertices[1].z = 0.0f;

    sceGuTexMode(texture->mode, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexImage(0, texture->wRender, texture->hRender, texture->width, texture->data);

    //sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    //sceGumDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D,32,0,vertices);
    //sceGuDisable(GU_TEXTURE_2D);
}

void drawTextureC(TexturePSP * texture, float x, float y, float w, float h){
    drawTexture(texture, x, y, 0.0f, 0.0f, x + w, y + h, texture->width, texture->height, 0xFFFFFFFF);
}

void drawTextureF(TexturePSP * texture, float x, float y) {
	drawTextureC(texture, x, y, texture -> width, texture -> height);
}

void drawTextureH(TexturePSP * texture, float x, float y, uint32_t colour) {
    TextureVertex *vertices = (TextureVertex*)sceGuGetMemory(2 * sizeof(TextureVertex));
    //static TextureVertex vertices[2];

    vertices[0].u = -x;
    vertices[0].v = 0.0f;
    vertices[0].colour = colour;
    vertices[0].x = 0.0f;
    vertices[0].y = y;
    vertices[0].z = 0.0f;

    vertices[1].u = SCREEN_WIDTH - x;
    vertices[1].v = texture->height;
    vertices[1].colour = colour;
    vertices[1].x = SCREEN_WIDTH;
    vertices[1].y = y + texture->height;
    vertices[1].z = 0.0f;

    sceGuTexMode(texture->mode, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexWrap(GU_REPEAT, GU_CLAMP);
    sceGuTexImage(0, texture->wRender, texture->hRender, texture->width, texture->data);

    //sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    //sceGumDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D,32,0,vertices);
    //sceGuDisable(GU_TEXTURE_2D);
}

void drawTextureR(TexturePSP * texture, float x, float y, float angle, u32 colour) {
    TextureVertex *vertices = (TextureVertex*)sceGuGetMemory(3 * sizeof(TextureVertex));
    //static TextureVertex vertices[2];

    vertices[0].colour = colour;
    vertices[1].colour = colour;
    vertices[2].colour = colour;
    vertices[0].z = 0.0f;
    vertices[1].z = 0.0f;
    vertices[2].z = 0.0f;

    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = x + texture->width;
    vertices[1].y = y;
    vertices[1].u = texture->width;
    vertices[1].v = 0.0f;

    vertices[2].x = x + texture->width;
    vertices[2].y = y + texture->height;
    vertices[2].u = texture->width;
    vertices[2].v = texture->height;
    
    sceGuTexMode(texture->mode, 0, 0, GU_FALSE);
    sceGuTexFunc(GU_TFX_REPLACE, GU_TCC_RGBA);
    sceGuTexImage(0, texture->wRender, texture->hRender, texture->width, texture->data);

    //sceGuEnable(GU_TEXTURE_2D); 
    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    //sceGumDrawArray(GU_SPRITES,GU_TEXTURE_32BITF|GU_COLOR_8888|GU_VERTEX_32BITF|GU_TRANSFORM_2D,32,0,vertices);
    //sceGuDisable(GU_TEXTURE_2D);

    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].u = 0.0f;
    vertices[0].v = 0.0f;

    vertices[1].x = x;
    vertices[1].y = y + texture->height;
    vertices[1].u = 0.0f;
    vertices[1].v = texture->height;

    vertices[2].x = x + texture->width;
    vertices[2].y = y + texture->height;
    vertices[2].u = texture->width;
    vertices[2].v = texture->height;

    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
}

void drawRect(float x, float y, float w, float h, uint32_t color) {

    ColorVertex* vertices = (ColorVertex*) sceGuGetMemory(2 * sizeof(ColorVertex));
    //Vertex vertices[2];

    vertices[0].x = x;
    vertices[0].y = y;
    vertices[0].z = 0.0f;
    vertices[0].colour = color;    

    vertices[1].x = x + w;
    vertices[1].y = y + h;
    vertices[1].z = 0.0f;
    vertices[1].colour = color;

    sceGuColor(color); // colors are ABGR
    sceGuDisable(GU_TEXTURE_2D);
    sceGuDrawArray(GU_SPRITES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuEnable(GU_TEXTURE_2D);
}
