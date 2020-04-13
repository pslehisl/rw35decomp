/***************************************************************************
 *                                                                         *
 * Module  : drvmodel.h (GCN)                                             *
 *                                                                         *
 * Purpose : Driver model description (device specific vertices/polys)     *
 *                                                                         *
 **************************************************************************/

/* RWPUBLIC */
#ifndef GCN_DRVMODEL_H
#define GCN_DRVMODEL_H

/* RWPUBLICEND */

/****************************************************************************
 Includes
 */

#include "batypes.h"
#include "bacolor.h"

/* RWPUBLIC */

/****************************************************************************
 Defines
 */

/* Set true depth information (for fogging, eg) */
#define RwIm2DVertexSetCameraX(vert, camx)      /* Nothing */
#define RwIm2DVertexSetCameraY(vert, camy)      /* Nothing */
#define RwIm2DVertexSetCameraZ(vert, camz)      /* Nothing */
#define RwIm2DVertexSetRecipCameraZ(vert, recipz)     /* Nothing */

#define RwIm2DVertexGetCameraX(vert)            (cause an error)
#define RwIm2DVertexGetCameraY(vert)            (cause an error)
#define RwIm2DVertexGetCameraZ(vert)            (cause an error)
#define RwIm2DVertexGetRecipCameraZ(vert)       (cause an error)

/* Set screen space coordinates in a device vertex */
#define RwIm2DVertexSetScreenX(vert, scrnx)     ((vert)->x = (scrnx))
#define RwIm2DVertexSetScreenY(vert, scrny)     ((vert)->y = (scrny))
#define RwIm2DVertexSetScreenZ(vert, scrnz)     ((vert)->z = (scrnz))
#define RwIm2DVertexGetScreenX(vert)            ((vert)->x)
#define RwIm2DVertexGetScreenY(vert)            ((vert)->y)
#define RwIm2DVertexGetScreenZ(vert)            ((vert)->z)

/* Set texture coordinates in a device vertex */
#define RwIm2DVertexSetU(vert, u, recipz)       ((vert)->u = (u))
#define RwIm2DVertexSetV(vert, v, recipz)       ((vert)->v = (v))
#define RwIm2DVertexGetU(vert)                  ((vert)->u)
#define RwIm2DVertexGetV(vert)                  ((vert)->v)

/* Modify the luminance stuff */
#define RwIm2DVertexSetRealRGBA(vert, r, g, b, a)   \
MACRO_START                                         \
{                                                   \
    ((vert)->col.red = (r));                        \
    ((vert)->col.green = (g));                      \
    ((vert)->col.blue = (b));                       \
    ((vert)->col.alpha = (a));                      \
}                                                   \
MACRO_STOP

#define RwIm2DVertexSetIntRGBA(vert, r, g, b, a)    \
MACRO_START                                         \
{                                                   \
    ((vert)->col.red = (RwReal)(r));                \
    ((vert)->col.green = (RwReal)(g));              \
    ((vert)->col.blue = (RwReal)(b));               \
    ((vert)->col.alpha = (RwReal)(a));              \
}                                                   \
MACRO_STOP

#define RwIm2DVertexGetRed(vert)    ((vert)->col.red)
#define RwIm2DVertexGetGreen(vert)  ((vert)->col.green)
#define RwIm2DVertexGetBlue(vert)   ((vert)->col.blue)
#define RwIm2DVertexGetAlpha(vert)  ((vert)->col.alpha)

#define RwIm2DVertexCopyRGBA(dst, src)      \
MACRO_START                                 \
{                                           \
    ((dst)->col.red   = (src)->col.red);    \
    ((dst)->col.green = (src)->col.green);  \
    ((dst)->col.blue  = (src)->col.blue);   \
    ((dst)->col.alpha = (src)->col.alpha);  \
}                                           \
MACRO_STOP

/* Clipper stuff */
#define RwIm2DVertexClipRGBA(o, i, n, f)                    \
MACRO_START                                                 \
{                                                           \
    const RwInt32        _factor =                          \
        (RwFastRealToUInt32(i * (RwReal)(255))) & 255;      \
                                                            \
    (o)->col.red =                                          \
        (n)->col.red +                                      \
        (((f)->col.red - (n)->col.red) * _factor) >> 8;     \
                                                            \
    (o)->col.blue =                                         \
        (n)->col.blue +                                     \
        (((f)->col.blue - (n)->col.blue) * _factor) >> 8;   \
                                                            \
    (o)->col.green =                                        \
        (n)->col.green +                                    \
        (((f)->col.green - (n)->col.green) * _factor) >> 8; \
                                                            \
    (o)->col.alpha =                                        \
        (n)->col.alpha +                                    \
        (((f)->col.alpha - (n)->col.alpha) * _factor) >> 8; \
}                                                           \
MACRO_STOP

#define RwV3dTransformPointsMacro(_pntsOut, _pntsIn, _count, _matrix)       \
MACRO_START                                                                 \
{                                                                           \
    register RwV3d *po = (_pntsOut);                                        \
    const register RwV3d *pi = (_pntsIn);                                   \
    register RwInt32 cnt = (_count);                                        \
    const register RwMatrix *m = (_matrix);                                 \
    register RwV3d *p;                                                      \
                                                                            \
    asm {                                                                   \
        mtctr       cnt                 ;                                   \
        psq_l       f0,  0x00(m), 0, 0  ;                                   \
        psq_l       f1,  0x08(m), 1, 0  ;                                   \
        addi        p, po, -0x04        ;                                   \
        psq_l       f6,  0x30(m), 0, 0  ;                                   \
        psq_l       f7,  0x38(m), 1, 0  ;                                   \
        psq_l       f8,  0x00(pi), 0, 0 ;                                   \
        psq_lu      f9,  0x08(pi), 1, 0 ;                                   \
        psq_l       f2,  0x10(m), 0, 0  ;                                   \
        psq_l       f3,  0x18(m), 1, 0  ;                                   \
        psq_l       f4,  0x20(m), 0, 0  ;                                   \
        psq_l       f5,  0x28(m), 1, 0  ;                                   \
                                                                            \
        loop:                                                               \
        ps_madds0   f10, f0, f8, f6     ;                                   \
        ps_madds0   f11, f1, f8, f7     ;                                   \
        ps_madds1   f10, f2, f8, f10    ;                                   \
        ps_madds1   f11, f3, f8, f11    ;                                   \
        psq_lu      f8,  0x04(pi), 0, 0 ;                                   \
        ps_madds0   f10, f4, f9, f10    ;                                   \
        ps_madds0   f11, f5, f9, f11    ;                                   \
        psq_lu      f9,  0x08(pi), 1, 0 ;                                   \
        psq_stu     f10, 0x04(p), 0, 0  ;                                   \
        psq_stu     f11, 0x08(p), 1, 0  ;                                   \
        bdnz        loop                                                    \
    }                                                                       \
}                                                                           \
MACRO_STOP

#define RwV3dTransformVectorsMacro(_vecsOut, _vecsIn, _count, _matrix)      \
MACRO_START                                                                 \
{                                                                           \
    register RwV3d *vo = (_vecsOut);                                        \
    const register RwV3d *vi = (_vecsIn);                                   \
    register RwInt32 cnt = (_count);                                        \
    const register RwMatrix *m = (_matrix);                                 \
    register RwV3d *v;                                                      \
                                                                            \
    asm {                                                                   \
        mtctr       cnt                 ;                                   \
        psq_l       f0,  0x00(m), 0, 0  ;                                   \
        psq_l       f1,  0x08(m), 1, 0  ;                                   \
        addi        v, vo, -4           ;                                   \
        psq_l       f6,  0x00(vi), 0, 0 ;                                   \
        psq_lu      f7,  0x08(vi), 1, 0 ;                                   \
        psq_l       f2,  0x10(m), 0, 0  ;                                   \
        psq_l       f3,  0x18(m), 1, 0  ;                                   \
        psq_l       f4,  0x20(m), 0, 0  ;                                   \
        psq_l       f5,  0x28(m), 1, 0  ;                                   \
                                                                            \
        loop:                                                               \
        ps_muls0    f10, f0, f6         ;                                   \
        ps_muls0    f11, f1, f6         ;                                   \
        ps_madds1   f10, f2, f6, f10    ;                                   \
        ps_madds1   f11, f3, f6, f11    ;                                   \
        psq_lu      f6,  0x04(vi), 0, 0 ;                                   \
        ps_madds0   f10, f4, f7, f10    ;                                   \
        ps_madds0   f11, f5, f7, f11    ;                                   \
        psq_lu      f7,  0x08(vi), 1, 0 ;                                   \
        psq_stu     f10, 0x04(v), 0, 0  ;                                   \
        psq_stu     f11, 0x08(v), 1, 0  ;                                   \
        bdnz        loop                                                    \
    }                                                                       \
}                                                                           \
MACRO_STOP

#define RwMatrixMultiplyPSMacro(_matrix, _matrixIn1, _matrixIn2)    \
MACRO_START                                                         \
{                                                                   \
    register int tmp1, tmp2;                                        \
    register RwMatrix *m = (_matrix);                               \
    const register RwMatrix *m1 = (_matrixIn1);                     \
    const register RwMatrix *m2 = (_matrixIn2);                     \
                                                                    \
    asm {                                                           \
        psq_l       f8,  0x00(m2), 0, 0 ;                           \
        psq_l       f0,  0x00(m1), 0, 0 ;                           \
        psq_l       f2,  0x10(m1), 0, 0 ;                           \
        ps_muls0    f12, f8, f0         ;                           \
        psq_l       f9,  0x10(m2), 0, 0 ;                           \
        ps_muls0    f13, f8, f2         ;                           \
        psq_l       f4,  0x20(m1), 0, 0 ;                           \
        psq_l       f6,  0x30(m1), 0, 0 ;                           \
        ps_muls0    f30, f8, f4         ;                           \
        psq_l       f11, 0x30(m2), 0, 0 ;                           \
        ps_madds1   f12, f9, f0, f12    ;                           \
        psq_l       f10, 0x20(m2), 0, 0 ;                           \
        ps_madds1   f13, f9, f2, f13    ;                           \
        ps_madds0   f31, f8, f6, f11    ;                           \
        psq_l       f1,  0x08(m1), 1, 0 ;                           \
        ps_madds1   f30, f9, f4, f30    ;                           \
        psq_l       f3,  0x18(m1), 1, 0 ;                           \
        ps_madds0   f12, f10, f1, f12   ;                           \
        psq_l       f5,  0x28(m1), 1, 0 ;                           \
        ps_madds1   f31, f9, f6, f31    ;                           \
        psq_l       f7,  0x38(m1), 1, 0 ;                           \
        ps_madds0   f13, f10, f3, f13   ;                           \
        psq_l       f8,  0x08(m2), 1, 0 ;                           \
        psq_st      f12, 0x00(m), 0, 0  ;                           \
        ps_madds0   f30, f10, f5, f30   ;                           \
        ps_madds0   f31, f10, f7, f31   ;                           \
        psq_l       f11, 0x38(m2), 1, 0 ;                           \
        psq_st      f13, 0x10(m), 0, 0  ;                           \
        ps_muls0    f12, f8, f0         ;                           \
        psq_l       f9,  0x18(m2), 1, 0 ;                           \
        psq_st      f30, 0x20(m), 0, 0  ;                           \
        ps_muls0    f13, f8, f2         ;                           \
        psq_l       f10, 0x28(m2), 1, 0 ;                           \
        ps_madds1   f12, f9, f0, f12    ;                           \
        lwz         tmp1, 0x0C(m1)      ;                           \
        lwz         tmp2, 0x0C(m2)      ;                           \
        ps_madds0   f11, f8, f6, f11    ;                           \
        ps_muls0    f30, f8, f4         ;                           \
        psq_st      f31, 0x30(m), 0, 0  ;                           \
        ps_madds1   f13, f9, f2, f13    ;                           \
        and         tmp2, tmp1, tmp2    ;                           \
        ps_madds1   f11, f9, f6, f11    ;                           \
        stw         tmp2, 0x0C(m)       ;                           \
        ps_madds0   f12, f10, f1, f12   ;                           \
        ps_madds1   f8, f9, f4, f30     ;                           \
        ps_madds0   f13, f10, f3, f13   ;                           \
        psq_st      f12, 0x08(m), 1, 0  ;                           \
        ps_madds0   f11, f10, f7, f11   ;                           \
        ps_madds0   f8, f10, f5, f8     ;                           \
        psq_st      f13, 0x18(m), 1, 0  ;                           \
        psq_st      f8,  0x28(m), 1, 0  ;                           \
        psq_st      f11, 0x38(m), 1, 0  ;                           \
    }                                                               \
}                                                                   \
MACRO_STOP

#if (!defined(_RW_MATMUL_CPU) && !defined(RwMatrixMultiplyMacro))
#define RwMatrixMultiplyMacro(_m, _m1, _m2) \
        RwMatrixMultiplyPSMacro(_m, _m1, _m2)
#endif /* (!defined(_RW_MATMUL_CPU) && !defined(RwMatrixMultiplyMacro)) */

/****************************************************************************
 Global Types
 */

/* Define types used */

typedef struct rwGameCube2DVertex RwGameCube2DVertex;
struct rwGameCube2DVertex
{
    RwReal  x;    /**< Screen X */
    RwReal  y;    /**< Screen Y */
    RwReal  z;    /**< Screen Z */

    RwRGBA  col;  /**< Vertex color */

    RwReal  u;    /**< Texture coordinate U */
    RwReal  v;    /**< Texture coordinate V */
};

/**
 * \ingroup rwim2dvertex
 * \ref RwIm2DVertex
 * Typedef for a RenderWare Graphics Immediate Mode 2D Vertex
 */
typedef RwGameCube2DVertex  RwIm2DVertex;

/**
 * \ingroup rwcoregeneric
 * \ref RxVertexIndex
 * Typedef for a RenderWare Graphics PowerPipe Immediate Mode Vertex
 */
typedef RwUInt16        RxVertexIndex;

/**
 * \ingroup rwcoregeneric
 * \ref RwImVertexIndex
 * Typedef for a RenderWare Graphics Immediate Mode Vertex
 */
typedef RxVertexIndex   RwImVertexIndex;

#endif /* GCN_DRVMODEL_H */
/* RWPUBLICEND */
