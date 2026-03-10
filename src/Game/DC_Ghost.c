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

void njUnitMatrix(MTX* mtx) {
    if (mtx == NULL) {
        mtx = &cmtx;
    }

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            mtx->a[i][j] = (i == j);
        }
    }
}

void njGetMatrix(MTX* m) {
    *m = cmtx;
}

void njSetMatrix(MTX* md, MTX* ms) {
    if (md == NULL) {
        md = &cmtx;
    }

    *md = *ms;
}

void njScale(MTX* mtx, f32 x, f32 y, f32 z) {
    if (mtx == NULL) {
        mtx = &cmtx;
    }

    for (int i = 0; i < 4; i++) {
        mtx->a[0][i] *= x;
        mtx->a[1][i] *= y;
        mtx->a[2][i] *= z;
    }
}

void njTranslate(MTX* mtx, f32 x, f32 y, f32 z) {
    if (mtx == NULL) {
        mtx = &cmtx;
    }

    MTX translation_matrix;

    njUnitMatrix(&translation_matrix);
    translation_matrix.a[3][0] = x;
    translation_matrix.a[3][1] = y;
    translation_matrix.a[3][2] = z;

    matmul(mtx, &translation_matrix, mtx);
}

void njSetBackColor(u32 c0, u32 c1, u32 c2) {
    c0 = c0 | c1 | c2;
    flSetRenderState(FLRENDER_BACKCOLOR, NTH_BYTE(c0, 3) | NTH_BYTE(c0, 2) | NTH_BYTE(c0, 1) | NTH_BYTE(c0, 0));
    
}

void njColorBlendingMode(s32 target, s32 mode) {
    target = target;
    mode = mode;
    flSetRenderState(FLRENDER_ALPHABLENDMODE, 0x32);
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

void njDrawTexture(Polygon* polygon, s32 /* unused */, s32 tex, s32 /* unused */) {
    Vertex vtx[4];
    s32 i;

    for (i = 0; i < 4; i++) {
        vtx[i] = ((_Polygon*)polygon)[i].v;
    }

    ppgWriteQuadWithST_B(vtx, polygon[0].col, NULL, tex, -1);
}

void njDrawSprite(Polygon* polygon, s32 /* unused */, s32 tex, s32 /* unused */) {
    Vertex vtx[4];

    if ((polygon[0].x >= 384.0f) || (polygon[3].x < 0.0f) || (polygon[0].y >= 224.0f) || (polygon[3].y < 0.0f)) {
        return;
    }

    vtx[0] = ((_Polygon*)polygon)[0].v;
    vtx[3] = ((_Polygon*)polygon)[3].v;

    ppgWriteQuadWithST_B2(vtx, polygon[0].col, 0, tex, -1);
}

void njdp2d_init() {
    njdp2d_w.ix1st = -1;
    njdp2d_w.total = 0;
}

void njdp2d_draw() {
    ColorVertex* vertices = (ColorVertex*) sceGuGetMemory(6 * sizeof(ColorVertex));
    s32 i;
    s32 k;

    for (i = njdp2d_w.ix1st; i != -1; i = njdp2d_w.prim[i].next) {
        switch (njdp2d_w.prim[i].type) {
        case 0:
            //Vertex vertices[2];

            for(int j = 0; j < 3; j++){
                k = -j + 5;
                vertices[k].x = vertices[j].x = njdp2d_w.prim[i].v[j].x;
                vertices[k].y = vertices[j].y = njdp2d_w.prim[i].v[j].y;
                vertices[k].z = vertices[j].z = njdp2d_w.prim[i].v[j].z;
                vertices[k].colour = vertices[j].colour = njdp2d_w.prim[i].col;
                //vertices[j].colour = 0xFFFFFFFF;
                //drawRect(vertices[j].x, vertices[j].y, 8, 8, 0xFFFFFFFF);
            }
            vertices[5].x = njdp2d_w.prim[i].v[3].x;
            vertices[5].y = njdp2d_w.prim[i].v[3].y;
            vertices[5].z = njdp2d_w.prim[i].v[3].z;
            vertices[5].colour = njdp2d_w.prim[3].col;

            sceGuDisable(GU_TEXTURE_2D);
            //sceGuDrawArray(GU_TRIANGLE_FAN, GU_COLOR_5551 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0, vertices);
            //sceGuDrawArray(GU_TRIANGLE_FAN, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 4, 0, vertices);
            sceGuDrawArray(GU_TRIANGLES, GU_COLOR_8888 | GU_VERTEX_32BITF | GU_TRANSFORM_2D, 6, 0, vertices);
            sceGuEnable(GU_TEXTURE_2D);
            break;

        case 1:
            shadow_drawing((WORK*)njdp2d_w.prim[i].col, njdp2d_w.prim[i].v[0].y);
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
         flLogOut("The 2D polygon display request has exceeded the buffer\n");

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
    flLogOut("njdp2d_w.total %d\n", njdp2d_w.total);
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
