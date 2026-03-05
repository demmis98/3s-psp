#include "Game/DC_Ghost.h"
#include "common.h"
//#include "sf33rd/AcrSDK/ps2/flps2render.h"
//#include "sf33rd/AcrSDK/ps2/foundaps2.h"
#include "psp/PPGFile.h"
#include "Game/AcrUtil.h"
#include "Game/aboutspr.h"
#include "Game/color3rd.h"
//#include "PS2/ps2Quad.h"
#include "structs.h"
//#include <libvu0.h>

#include "common/sprites.h"
#include "common/graphics.h"
#include "fl.h"

#if !defined(TARGET_PS2)
#include <string.h>
#endif

#define NTH_BYTE(value, n) ((((value >> n * 8) & 0xFF) << n * 8))

typedef struct {
    Vertex v;
    u32 col;
} _Polygon;

// `col` needs to be `uintptr_t` because it sometimes stores a pointer to `WORK`
typedef struct {
    // total size: 0x3C
    Vec3 v[4];     // offset 0x0, size 0x30
    uintptr_t col; // offset 0x30, size 0x4
    u32 type;      // offset 0x34, size 0x4
    s32 next;      // offset 0x38, size 0x4
} NJDP2D_PRIM;

typedef struct {
    // total size: 0x1774
    s16 ix1st;             // offset 0x0, size 0x2
    s16 total;             // offset 0x2, size 0x2
    NJDP2D_PRIM prim[100]; // offset 0x4, size 0x1770
} NJDP2D_W;

NJDP2D_W njdp2d_w;
MTX cmtx;

#if !defined(TARGET_PS2)
static void matmul(MTX* dst, const MTX* a, const MTX* b) {
    MTX result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.a[i][j] =
                a->a[i][0] * b->a[0][j] + a->a[i][1] * b->a[1][j] + a->a[i][2] * b->a[2][j] + a->a[i][3] * b->a[3][j];
        }
    }

    memcpy(dst, &result, sizeof(MTX));
}
#endif

s32 currentTexture = -1;

void njUnitMatrix(MTX* mtx) {
    /*
    if (mtx == NULL) {
        mtx = &cmtx;
    }

    sceVu0UnitMatrix(mtx->a);
    */
}

void njGetMatrix(MTX* m) {
    //*m = cmtx;
}

void njSetMatrix(MTX* md, MTX* ms) {
    /*
    if (md == NULL) {
        md = &cmtx;
    }

    *md = *ms;
    */
}

void njScale(MTX* mtx, f32 x, f32 y, f32 z) {
    /*
    f32 v0[4];

    v0[0] = x;
    v0[1] = y;
    v0[2] = z;
    v0[3] = 1.0f;

    if (mtx == NULL) {
        mtx = &cmtx;
    }

#if defined(TARGET_PS2)
    __asm__ __volatile__("lqc2       vf8,  0x0(%0) \n"
                         "lqc2       $vf4, 0x0(%1) \n"
                         "lqc2       $vf5, 0x10(%1) \n"
                         "lqc2       $vf6, 0x20(%1) \n"
                         "lqc2       $vf7, 0x30(%1) \n"
                         "vmulx.xyzw $vf4, $vf4, $vf8x \n"
                         "vmuly.xyzw $vf5, $vf5, $vf8y \n"
                         "vmulz.xyzw $vf6, $vf6, $vf8z \n"
                         "sqc2       $vf4, 0x0(%1) \n"
                         "sqc2       $vf5, 0x10(%1) \n"
                         "sqc2       $vf6, 0x20(%1) \n"
                         "sqc2       $vf7, 0x30(%1) \n"
                         :
                         : "r"(v0), "r"(mtx)
                         : "memory");
#else
    for (int i = 0; i < 4; i++) {
        mtx->a[0][i] *= v0[0];
        mtx->a[1][i] *= v0[1];
        mtx->a[2][i] *= v0[2];
    }
#endif
    */
}

void njTranslate(MTX* mtx, f32 x, f32 y, f32 z) {
    /*
    if (mtx == NULL) {
        mtx = &cmtx;
    }

#if defined(TARGET_PS2)
    __asm__ __volatile__("mfc1       $t0, %1 \n"
                         "mfc1       $t1, %3 \n"
                         "pextlw     $t0, $t1, $t0 \n"

                         "mfc1       $t1, %2 \n"
                         "pextlw     $t0, $t1, $t0 \n"

                         "qmtc2      $t0, $vf8 \n"
                         "vmove.w    $vf8, $vf0 \n"

                         "lqc2       $vf4, 0x0(%0) \n"
                         "lqc2       $vf5, 0x10(%0) \n"
                         "lqc2       $vf6, 0x20(%0) \n"
                         "lqc2       $vf7, 0x30(%0) \n"
                         "vmulax.xyzw $ACC, $vf4, $vf8x \n"
                         "vmadday.xyzw $ACC, $vf5, $vf8y \n"
                         "vmaddaz.xyzw $ACC, $vf6, $vf8z \n"
                         "vmaddw.xyzw $vf9, $vf7, $vf8w \n"
                         "sqc2       $vf9, 0x30(%0) \n"
                         :
                         : "r"(mtx), "f"(x), "f"(y), "f"(z)
                         : "t0", "t1", "memory");
#else
    MTX translation_matrix;

    njUnitMatrix(&translation_matrix);
    translation_matrix.a[3][0] = x;
    translation_matrix.a[3][1] = y;
    translation_matrix.a[3][2] = z;

    matmul(mtx, &translation_matrix, mtx);
#endif
    */
}

void njSetBackColor(u32 c0, u32 c1, u32 c2) {
    /*
    c0 = c0 | c1 | c2;
    flSetRenderState(FLRENDER_BACKCOLOR, NTH_BYTE(c0, 3) | NTH_BYTE(c0, 2) | NTH_BYTE(c0, 1) | NTH_BYTE(c0, 0));
    */
}

void njColorBlendingMode(s32 target, s32 mode) {
    target = target;
    mode = mode;
    //flSetRenderState(FLRENDER_ALPHABLENDMODE, 0x32);
}

void njCalcPoint(MTX* mtx, Vec3* ps, Vec3* pd) {
    f32 v0[4];

    if (mtx == NULL) {
        mtx = &cmtx;
    }

#if defined(TARGET_PS2)
    v0[0] = ps->x;
    v0[1] = ps->y;
    v0[2] = ps->z;
    v0[3] = 1.0f;

    __asm__ __volatile__("lqc2    $vf8, 0(%1) \n"
                         "lqc2    $vf4, 0(%0) \n"
                         "lqc2    $vf5, 0x10(%0) \n"
                         "lqc2    $vf6, 0x20(%0) \n"
                         "lqc2    $vf7, 0x30(%0) \n"
                         "vmulax.xyz $ACC, $vf4, $vf8x \n"
                         "vmadday.xyz $ACC, $vf5, $vf8y \n"
                         "vmaddaz.xyz $ACC, $vf6, $vf8z \n"
                         "vmaddw.xyz $vf9, $vf7, $vf8w \n"
                         "sqc2 $vf9, 0(%1) \n"
                         :
                         : "r"(mtx), "r"(v0), "f"(pd)
                         : "memory");

    pd->x = v0[0];
    pd->y = v0[1];
    pd->z = v0[2];
#else
    f32 x = ps->x;
    f32 y = ps->y;
    f32 z = ps->z;
    f32 w = 1.0f;

    pd->x = x * mtx->a[0][0] + y * mtx->a[1][0] + z * mtx->a[2][0] + w * mtx->a[3][0];
    pd->y = x * mtx->a[0][1] + y * mtx->a[1][1] + z * mtx->a[2][1] + w * mtx->a[3][1];
    pd->z = x * mtx->a[0][2] + y * mtx->a[1][2] + z * mtx->a[2][2] + w * mtx->a[3][2];
#endif
}

void njCalcPoints(MTX* mtx, Vec3* ps, Vec3* pd, s32 num) {
    s32 i;

    if (mtx == NULL) {
        mtx = &cmtx;
    }

    for (i = 0; i < num; i++) {
        njCalcPoint(mtx, ps++, pd++);
    }
}

void njRotateZ(s32 /* unused */, s32 /* unused */) {
    // Do nothing
}

void njDrawTexture(Polygon* polygon, s32 unused0, s32 texture, s32 unused1) {
    /*
    float x0, y0, x1, y1;
    float u0, v0, u1, v1;
    int i_pol;
    */

    if(DEMMA_DEBUG){
        flLogOut("njDrawTexture %d %x\n", texture, texturesPSP[texture]);
        flLogOut("mode: %d data: %x width: %d height: %d\n", texturesPSP[texture].mode, texturesPSP[texture].data, texturesPSP[texture].width, texturesPSP[texture].height);
        
        //flLogOut("palette %d\n", currentPaletteIndex);

        int loop = 0;
        for(int i = 0; i < 64; i++){
            //flLogOut("%x ", ColorRAM[currentPaletteIndex][i]);
        }

        for(int i = 0; i < texturesPSP[texture].width * 0; i++){
            //flLogOut("%x ", texturesPSP[texture].data[i]);
        }
    }
    
    /*
    for(i_pol = 0; i_pol < 4; i_pol++){
        if(polygon[i_pol].x > x1)
            x1 = polygon[i_pol].x;

        if(polygon[i_pol].y > y1)
            y1 = polygon[i_pol].y;

        if(polygon[i_pol].u > u1)
            u1 = polygon[i_pol].u;

        if(polygon[i_pol].v > v1)
            v1 = polygon[i_pol].v;

        if(polygon[i_pol].x < x0 || x0 == -999.0f)
            x0 = polygon[i_pol].x;

        if(polygon[i_pol].y < y0 || y0 == -999.0f)
            y0 = polygon[i_pol].y;

        if(polygon[i_pol].u < u0 || u0 == -999.0f)
            u0 = polygon[i_pol].u;

        if(polygon[i_pol].v < v0 || v0 == -999.0f)
            v0 = polygon[i_pol].v;
    }

    u0 *= texturesPSP[texture].width;
    v0 *= texturesPSP[texture].height;
    u1 *= texturesPSP[texture].width;
    v1 *= texturesPSP[texture].height;

    if(DEMMA_DEBUG){
        flLogOut("x0: %f y0: %f x1: %f y1: %f\n", x0, y0, x1, y1);
        flLogOut("u0: %f v0: %f u1: %f v1: %f\n", u0, v0, u1, v1);
    }

    //while(1);
    */
    //drawRect(x0, y0, x1 - x0, y1 - y0, 0xFF00FFFF);
    //drawRect(x0, y0, x1 - x0, y1 - y0, njdp2d_w.prim[i].col);

    //drawRect(polygon[0].x, polygon[0].y, 10, 10, 0xFFFFFFFF);
    //drawRect(polygon[1].x, polygon[1].y, 10, 10, 0xFFFF00FF);
    //drawRect(polygon[2].x, polygon[2].y, 10, 10, 0xFFFF00FF);
    //drawRect(polygon[3].x, polygon[3].y, 10, 10, 0xFFFF00FF);

    //sceGuClutMode(GU_PSM_5551, 0, 0x3F, 0);
    sceGuClutMode(GU_PSM_5551, 0, 0xFF, 0);
    sceGuClutLoad(8, ColorRAM[currentPaletteIndex]);

    //drawTextureF(&texturesPSP[texture], x0, y0);
    //drawTexture(&texturesPSP[texture], x0, y0, u0, v0, x1, y1, u1, v1, 0xFFFFFFFF);
    if(currentTexture != texture){
        currentTexture = texture;
        setTexture(&texturesPSP[texture], GU_TFX_REPLACE);
    }
    //drawTextureSet(x0, y0, u0, v0, x1, y1, u1, v1, 0xFFFFFFFF);
    // draw directly bc what the hell
    TextureVertex *vertices = (TextureVertex*)sceGuGetMemory(4 * sizeof(TextureVertex));
    for(int i = 0; i < 4; i++){
        vertices[i].u = polygon[i].u * texturesPSP[texture].width;
        vertices[i].v = polygon[i].v * texturesPSP[texture].height;
        vertices[i].colour = 0xFFFFFFFF;
        vertices[i].x = polygon[i].x;
        vertices[i].y = polygon[i].y;
        vertices[i].z = 0.0f;
    }
    //sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);
    sceGuDrawArray( GU_TRIANGLE_FAN, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0, vertices);
}

void njDrawSprite(Polygon* polygon, s32 unused0, s32 texture, s32 unused1){
    TextureVertex *vertices = (TextureVertex*)sceGuGetMemory(2 * sizeof(TextureVertex));

    if(DEMMA_DEBUG)
        flLogOut("njDrawSprite %d %x\n", texture, texturesPSP[texture]);

    //if ((getCP3toFullScreenDrawFlag() != 0) &&
    if (
        ((polygon[0].x >= 384.0f) || (polygon[3].x < 0.0f) || (polygon[0].y >= 224.0f) || (polygon[3].y < 0.0f))) {
        return;
    }

    if(currentTexture != texture){
        currentTexture = texture;
        setTexture(&texturesPSP[texture], GU_TFX_REPLACE);
    }

    for(int i = 0; i < 2; i++){
        vertices[i].x = polygon[i*3].x;
        vertices[i].u = polygon[i*3].u * texturesPSP[texture].width;
        vertices[i].v = polygon[i*3].v * texturesPSP[texture].height;
        vertices[i].colour = 0xFFFFFFFF;
        vertices[i].x = polygon[i*3].x;
        vertices[i].y = polygon[i*3].y;
        vertices[i].z = 0.0f;
    }

    sceGuDrawArray(GU_SPRITES, GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 2, 0, vertices);

    //njDrawTexture(polygon, unused0, texture, unused1);
}

void njdp2d_init() {
    njdp2d_w.ix1st = -1;
    njdp2d_w.total = 0;
}

void njdp2d_draw() {
    Quad prm;
    s32 i;
    int x0=-1, y0=-1, x1=-1, y1=-1, i_prm;
    /*

    ps2SeqsRenderQuadInit_B();
    setZ_Operation(1);
    */
    for (i = njdp2d_w.ix1st; i != -1; i = njdp2d_w.prim[i].next) {
        switch (njdp2d_w.prim[i].type) {
        case 0:
            prm.v[0] = njdp2d_w.prim[i].v[0];
            prm.v[1] = njdp2d_w.prim[i].v[1];
            prm.v[2] = njdp2d_w.prim[i].v[2];
            prm.v[3] = njdp2d_w.prim[i].v[3];

            if(!DEMMA_DEBUG){
                for(i_prm = 0; i_prm < 4; i_prm++){
                    if(prm.v[i_prm].x > x1)
                        x1 = prm.v[i_prm].x;
                    if(prm.v[i_prm].y > y1)
                        y1 = prm.v[i_prm].y;

                    if(prm.v[i_prm].x < x0 || x0 == -1)
                        x0 = prm.v[i_prm].x;
                    if(prm.v[i_prm].y < y0 || y0 == -1)
                        y0 = prm.v[i_prm].y;
                }
                //drawRect(x0, y0, x1 - x0, y1 - y0, 0xFFFFFFFF);
                //drawRect(x0, y0, x1 - x0, y1 - y0, njdp2d_w.prim[i].col);

                drawRect(prm.v[0].x, prm.v[0].y, 10, 10, 0xFFFFFFFF);
                drawRect(prm.v[1].x, prm.v[1].y, 10, 10, 0xFFFF0000);
                drawRect(prm.v[2].x, prm.v[2].y, 10, 10, 0xFF00FF00);
                drawRect(prm.v[3].x, prm.v[3].y, 10, 10, 0xFF0000FF);
                //drawRect(prm.v[0].x, prm.v[0].y, prm.v[3].x - prm.v[0].x , prm.v[3].y - prm.v[0].y, 0xFFFFFFFF);
            }
            else{
                flLogOut("drawrect\n");
            }

            //ps2SeqsRenderQuad_B(&prm, njdp2d_w.prim[i].col);
            break;

        case 1:
            //shadow_drawing((WORK*)njdp2d_w.prim[i].col, njdp2d_w.prim[i].v[0].y);
            break;
        }
    }

    njdp2d_init();
    //ps2SeqsRenderQuadEnd();
}

// `col` needs to be `uintptr_t` because it sometimes stores a pointer to `WORK`
void njdp2d_sort(f32* pos, f32 pri, uintptr_t col, s32 flag) {
    s32 i;
    s32 ix = njdp2d_w.total;
    s32 prev;

    if (ix >= 100) {
        // Have to write the string as raw bytes here.
        // Otherwise MWCC removes a single byte for some reason
        //
        // Original:
        // flLogOut("２Ｄポリゴンの表示要求がバッファをオーバーしました\n");

        // The 2D polygon display request has exceeded the buffer\n
        /*
        flLogOut(
            "\x82\x51\x82\x63\x83\x7C\x83\x8A\x83\x53\x83\x93\x82\xCC\x95\x5C\x8E\xA6\x97\x76\x8B\x81\x82\xAA\x83\x6F"
            "\x83\x62\x83\x74\x83\x40\x82\xF0\x83\x49\x81\x5B\x83\x6F\x81\x5B\x82\xB5\x82\xDC\x82\xB5\x82\xBD\xA");
        */
        return;
    }

    if (flag == 0) {
        njdp2d_w.prim[ix].v[0].z = njdp2d_w.prim[ix].v[1].z = njdp2d_w.prim[ix].v[2].z = njdp2d_w.prim[ix].v[3].z = pri;
        njdp2d_w.prim[ix].v[0].x = pos[0];
        njdp2d_w.prim[ix].v[0].y = pos[1];
        njdp2d_w.prim[ix].v[1].x = pos[2];
        njdp2d_w.prim[ix].v[1].y = pos[3];
        njdp2d_w.prim[ix].v[2].x = pos[4];
        njdp2d_w.prim[ix].v[2].y = pos[5];
        njdp2d_w.prim[ix].v[3].x = pos[6];
        njdp2d_w.prim[ix].v[3].y = pos[7];
        njdp2d_w.prim[ix].type = 0;
        njdp2d_w.prim[ix].col = col;
    }

    if (flag == 1) {
        njdp2d_w.prim[ix].v[0].z = pri;
        njdp2d_w.prim[ix].v[0].y = pos[0];
        njdp2d_w.prim[ix].type = 1;
        njdp2d_w.prim[ix].col = col;
    }

    njdp2d_w.prim[ix].next = -1;

    if (njdp2d_w.ix1st == -1) {
        njdp2d_w.ix1st = njdp2d_w.total;
    } else {
        i = njdp2d_w.ix1st;
        prev = -1;

        while (1) {
            if (pri > njdp2d_w.prim[i].v[0].z) {
                if (prev == -1) {
                    njdp2d_w.ix1st = ix;
                    njdp2d_w.prim[ix].next = i;
                } else {
                    njdp2d_w.prim[prev].next = ix;
                    njdp2d_w.prim[ix].next = i;
                }

                break;
            }

            if (njdp2d_w.prim[i].next == -1) {
                njdp2d_w.prim[i].next = ix;
                break;
            }

            prev = i;
            i = njdp2d_w.prim[i].next;
        }
    }

    njdp2d_w.total += 1;
}

void njDrawPolygon2D(PAL_CURSOR* p, s32 /* unused */, f32 pri, u32 attr) {
    if (attr & 0x20) {
        njdp2d_sort((f32*)p->p, pri, p->col->color, 0);
    }
}

void njSetPaletteBankNumG(u32 globalIndex, s32 bank) {
    globalIndex = globalIndex;
    ppgSetupCurrentPaletteNumber(0, bank);
}

void njSetPaletteMode(u32 mode) {
    mode = mode;
}

void njSetPaletteData(s32 offset, s32 count, void* data) {
    palCopyGhostDC(offset, count, data);
    palUpdateGhostDC();
}

s32 njReLoadTexturePartNumG(u32 gix, s8* srcAdrs, u32 ofs, u32 size) {
    ppgRenewDotDataSeqs(0, gix, (u32*)srcAdrs, ofs, size);
    return 1;
}
