/***************************************************************************
 *                                                                         *
 * Module  : dl2drend.c                                                    *
 *                                                                         *
 * Purpose : 2D primitive rendering functions.                             *
 *                                                                         *
 **************************************************************************/

/****************************************************************************
 Includes
 */
#include "batypes.h"
#include "badebug.h"
#include "batextur.h"
#include "baimage.h"
#include "baresour.h"
#include "bamatrix.h"
#include "baimmedi.h"
#include "barwtyp.h"
#include "drvmodel.h"
#include "drvfns.h"

#include "dldevice.h"
#include "dlrendst.h"

#include "dl2drend.h"

/****************************************************************************
 Globals (across program)
 */

/****************************************************************************
 Local (static) Globals
 */

static GXPrimitive _rwDlPrimConvTbl[] =
{
    0,                  /* rwPRIMTYPENAPRIMTYPE */
    GX_LINES,           /* rwPRIMTYPELINELIST */
    GX_LINESTRIP,       /* rwPRIMTYPEPOLYLINE */
    GX_TRIANGLES,       /* rwPRIMTYPETRILIST */
    GX_TRIANGLESTRIP,   /* rwPRIMTYPETRISTRIP */
    GX_TRIANGLEFAN,     /* rwPRIMTYPETRIFAN */
    GX_POINTS           /* rwPRIMTYPEPOINTLIST */
};

static f32 _rwDlProjectionMatrix[GX_PROJECTION_SZ];

/****************************************************************************
 _rw2DRenderPrimitiveInit

 On exit    : 
 */
void
_rw2DRenderPrimitiveInit(void)
{
    static f32      projVector[GX_PROJECTION_SZ] =
    {
        GX_ORTHOGRAPHIC,
        1.0f,  -1.0f,
        1.0f,   1.0f,
       -1.0f,  -1.0f
    };

    static f32      posMatrix[3][4] =
    {
        1.0f, 0.0f,  0.0f, 0.0f,
        0.0f, 1.0f,  0.0f, 0.0f,
        0.0f, 0.0f, -1.0f, 0.0f
    };

    const RwCamera  *camera;
    const RwRaster  *raster;

    RWFUNCTION(RWSTRING("_rw2DRenderPrimitiveInit"));
    RWASSERT(_RwDlTexture);

    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetNumTevStages(1);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, GX_DISABLE, GX_SRC_REG, GX_SRC_VTX,
        GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, GX_DISABLE, GX_SRC_REG, GX_SRC_REG,
        GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    
    if (RwTextureGetRaster(_RwDlTexture))
    {
        GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
        GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
        GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
        GXSetNumTexGens(1);
        GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_IDENTITY,
            GX_FALSE, GX_PTIDENTITY);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
        _rwDlTextureRasterFlush();
    }
    else
    {
        GXSetNumTexGens(0);
        GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD_NULL, GX_TEXMAP_NULL,
            GX_COLOR0A0);
        GXSetTevOp(GX_TEVSTAGE0, GX_PASSCLR);
    }
    
    if (_RwDlRenderMode->field_rendering)
    {
        GXSetViewportJitter(0.0f, 0.0f,
            _RwDlRenderMode->fbWidth, _RwDlRenderMode->xfbHeight,
            0.0f, 1.0f,
            VIGetNextField() ^ 1);
    }
    else
    {
        GXSetViewport(0.0f, 0.0f,
            _RwDlRenderMode->fbWidth, _RwDlRenderMode->xfbHeight,
            0.0f, 1.0f);
    }

    camera = RwCameraGetCurrentCamera();
    RWASSERT(camera);

    raster = RwCameraGetRaster(camera);
    RWASSERT(raster);
    
    projVector[1] = 2.0f / _RwDlRenderMode->fbWidth;
    projVector[3] = -2.0f / _RwDlRenderMode->xfbHeight;

    GXGetProjectionv(_rwDlProjectionMatrix);
    GXSetProjectionv(projVector);

    posMatrix[0][3] = 0.5f + raster->nOffsetX;
    posMatrix[1][3] = 0.5f + raster->nOffsetY;

    GXLoadPosMtxImm(posMatrix, GX_PNMTX0);
    GXSetCurrentMtx(GX_PNMTX0);

    RWRETURNVOID();
}

/****************************************************************************
 _rw2DRenderPrimativeTerm

 On exit    : 
 */
static void
_rw2DRenderPrimativeTerm(void)
{
    const RwCamera  *camera;
    const RwRaster  *raster;

    RWFUNCTION(RWSTRING("_rw2DRenderPrimitiveInit"));
    RWASSERT(_RwDlRenderMode);

    camera = RwCameraGetCurrentCamera();
    RWASSERT(camera);

    raster = RwCameraGetRaster(camera);
    RWASSERT(raster);

    if (raster != RwRasterGetParent(raster))
    {
        if (!_RwDlFSAA)
        {
            if (_RwDlRenderMode->field_rendering)
            {
                GXSetViewportJitter(raster->nOffsetX, raster->nOffsetY,
                    raster->width, raster->height,
                    0.0f, 1.0f, VIGetNextField() ^ 1);
            }
            else
            {
                GXSetViewport(raster->nOffsetX, raster->nOffsetY,
                    raster->width, raster->height,
                    0.0f, 1.0f);
            }

            GXSetScissor(raster->nOffsetX, raster->nOffsetY,
                raster->width, raster->height);
        }
        else
        {
            if (_RwDlRenderMode->field_rendering)
            {
                GXSetViewportJitter(raster->nOffsetX, raster->nOffsetY,
                    raster->width, raster->height,
                    0.0f, 1.0f, VIGetNextField() ^ 1);
            }
            else
            {
                GXSetViewport(raster->nOffsetX, raster->nOffsetY * 2,
                    raster->width, raster->height * 2,
                    0.0f, 1.0f);
            }
            
            if (_RwDlFSAATop)
            {
                if (((raster->nOffsetY + raster->height) * 2) <=
                    (_RwDlHalfHeight + 2))
                {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2,
                        _RwDlRenderMode->fbWidth, raster->height * 2);
                }
                else if ((raster->nOffsetY * 2) > (_RwDlHalfHeight + 2))
                {
                    GXSetScissor(0, 0,
                        _RwDlRenderMode->fbWidth, _RwDlHalfHeight + 2);
                }
                else
                {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2,
                        _RwDlRenderMode->fbWidth, _RwDlHalfHeight + 2);
                }

                GXSetScissorBoxOffset(0, 0);
            }
            else
            {
                if ((raster->nOffsetY * 2) <= (_RwDlHalfHeight - 2))
                {
                    GXSetScissor(raster->nOffsetX, raster->nOffsetY * 2,
                        _RwDlRenderMode->fbWidth, raster->height * 2);
                }
                else if (((raster->nOffsetY + raster->height) * 2) <
                         (_RwDlHalfHeight + 2))
                {
                    GXSetScissor(0, _RwDlHalfHeight - 2,
                        _RwDlRenderMode->fbWidth, _RwDlHalfHeight + 2);
                }
                else
                {
                    GXSetScissor(raster->nOffsetX, _RwDlHalfHeight - 2,
                        _RwDlRenderMode->fbWidth, raster->height * 2);
                }
                
                GXSetScissorBoxOffset(0, _RwDlHalfHeight - 2);
            }
        }
    }

    GXSetProjectionv(_rwDlProjectionMatrix);

    RWRETURNVOID();
}

/****************************************************************************
 _rwDlIm2DRenderTriangle

 On entry   : vertices, num vertices in array
            : index of triangle vertices
 On exit    :
 */
RwBool
_rwDlIm2DRenderTriangle(RwIm2DVertex *verts,
                        RwInt32 unused2 __RWUNUSED__,
                        RwInt32 vert1,
                        RwInt32 vert2,
                        RwInt32 vert3)
{
    RwIm2DVertex *v1, *v2, *v3;

    RWFUNCTION(RWSTRING("_rwDlIm2DRenderTriangle"));
    RWASSERT(verts);
    RWASSERT(_RwDlTexture);

    v1 = &verts[vert1];
    v2 = &verts[vert2];
    v3 = &verts[vert3];

    _rw2DRenderPrimitiveInit();

    GXBegin(GX_TRIANGLES, GX_VTXFMT0, 3);

    if (RwTextureGetRaster(_RwDlTexture))
    {
        GXPosition3f32(v1->x, v1->y, v1->z);
        GXColor4u8(v1->col.red, v1->col.green, v1->col.blue, v1->col.alpha);
        GXTexCoord2f32(v1->u, v1->v);

        GXPosition3f32(v2->x, v2->y, v2->z);
        GXColor4u8(v2->col.red, v2->col.green, v2->col.blue, v2->col.alpha);
        GXTexCoord2f32(v2->u, v2->v);

        GXPosition3f32(v3->x, v3->y, v3->z);
        GXColor4u8(v3->col.red, v3->col.green, v3->col.blue, v3->col.alpha);
        GXTexCoord2f32(v3->u, v3->v);
    }
    else
    {
        GXPosition3f32(v1->x, v1->y, v1->z);
        GXColor4u8(v1->col.red, v1->col.green, v1->col.blue, v1->col.alpha);

        GXPosition3f32(v2->x, v2->y, v2->z);
        GXColor4u8(v2->col.red, v2->col.green, v2->col.blue, v2->col.alpha);

        GXPosition3f32(v3->x, v3->y, v3->z);
        GXColor4u8(v3->col.red, v3->col.green, v3->col.blue, v3->col.alpha);
    }

    GXEnd();

    _rw2DRenderPrimativeTerm();

    RWRETURN(TRUE);
}

/****************************************************************************
 _rwDlIm2DRenderLine

 On entry   : vertices, num vertices in array
            : index of line ends
 On exit    :
 */
RwBool
_rwDlIm2DRenderLine(RwIm2DVertex *verts,
                    RwInt32 numVerts __RWUNUSED__,
                    RwInt32 vert1,
                    RwInt32 vert2)
{
    RwIm2DVertex *v1, *v2;

    RWFUNCTION(RWSTRING("_rwDlIm2DRenderLine"));
    RWASSERT(verts);
    RWASSERT(_RwDlTexture);

    v1 = &verts[vert1];
    v2 = &verts[vert2];

    _rw2DRenderPrimitiveInit();

    GXBegin(GX_LINES, GX_VTXFMT0, 2);

    if (RwTextureGetRaster(_RwDlTexture))
    {
        GXPosition3f32(v1->x, v1->y, v1->z);
        GXColor4u8(v1->col.red, v1->col.green, v1->col.blue, v1->col.alpha);
        GXTexCoord2f32(v1->u, v1->v);

        GXPosition3f32(v2->x, v2->y, v2->z);
        GXColor4u8(v2->col.red, v2->col.green, v2->col.blue, v2->col.alpha);
        GXTexCoord2f32(v2->u, v2->v);
    }
    else
    {
        GXPosition3f32(v1->x, v1->y, v1->z);
        GXColor4u8(v1->col.red, v1->col.green, v1->col.blue, v1->col.alpha);

        GXPosition3f32(v2->x, v2->y, v2->z);
        GXColor4u8(v2->col.red, v2->col.green, v2->col.blue, v2->col.alpha);
    }

    GXEnd();

    _rw2DRenderPrimativeTerm();

    RWRETURN(TRUE);
}

/****************************************************************************
 _rwDlIm2DRenderPrimitive

 On entry   : Primitive type
            : Vertices
            : Num Vertices
 On exit    :
 */
RwBool
_rwDlIm2DRenderPrimitive(RwPrimitiveType primType,
                         RwIm2DVertex *verts,
                         RwInt32 numVerts)
{
    RwIm2DVertex *v;
    RwInt32 i;

    RWFUNCTION(RWSTRING("_rwDlIm2DRenderPrimitive"));
    RWASSERT(primType >= rwPRIMTYPENAPRIMTYPE &&
             primType <= rwPRIMTYPEPOINTLIST);
    RWASSERT(verts);
    RWASSERT(_RwDlTexture);

    _rw2DRenderPrimitiveInit();

    GXBegin(_rwDlPrimConvTbl[primType], GX_VTXFMT0, numVerts);

    switch (primType)
    {
        case rwPRIMTYPEPOLYLINE:
        case rwPRIMTYPETRISTRIP:
        case rwPRIMTYPETRIFAN:
        {
            RWASSERT(numVerts > 1);

            if (RwTextureGetRaster(_RwDlTexture))
            {
                for (i = 0; i < numVerts; i++, verts++)
                {
                    v = &verts[0];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);
                }
            }
            else
            {
                for (i = 0; i < numVerts; i++, verts++)
                {
                    v = &verts[0];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                }
            }

            break;
        }

        case rwPRIMTYPELINELIST:
        {
            const RwInt32 numLines = numVerts >> 1;
            RWASSERT(numVerts > 1);

            if (RwTextureGetRaster(_RwDlTexture))
            {
                for (i = 0; i < numLines; i++, verts += 2)
                {
                    v = &verts[0];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);

                    v = &verts[1];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);
                }
            }
            else
            {
                for (i = 0; i < numLines; i++, verts += 2)
                {
                    v = &verts[0];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);

                    v = &verts[1];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                }
            }

            break;
        }

        case rwPRIMTYPETRILIST:
        {
            const RwInt32 numTriangles = numVerts / 3;
            RWASSERT(numVerts > 2);

            if (RwTextureGetRaster(_RwDlTexture))
            {
                for (i = 0; i < numTriangles; i++, verts += 3)
                {
                    v = &verts[0];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);

                    v = &verts[1];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);

                    v = &verts[2];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);
                }
            }
            else
            {
                for (i = 0; i < numTriangles; i++, verts += 3)
                {
                    v = &verts[0];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);

                    v = &verts[1];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);

                    v = &verts[2];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                }
            }

            break;
        }

        default:
            RWASSERT(FALSE);
            RWERROR((E_RX_INVALIDPRIMTYPE));
            break;
    }

    GXEnd();

    _rw2DRenderPrimativeTerm();

    RWRETURN(TRUE);
}

/****************************************************************************
 _rwDlIm2DRenderIndexedPrimitive

 On entry   : Primitive type
            : Vertices
            : Num Vertices
 On exit    :
 */
RwBool
_rwDlIm2DRenderIndexedPrimitive(RwPrimitiveType primType,
                                RwIm2DVertex *verts,
                                RwInt32 unused3 __RWUNUSED__,
                                RwImVertexIndex *indices,
                                RwInt32 numIndices)
{
    RwIm2DVertex *v;
    RwInt32 i;

    RWFUNCTION(RWSTRING("_rwDlIm2DRenderIndexedPrimitive"));
    RWASSERT(primType >= rwPRIMTYPENAPRIMTYPE &&
             primType <= rwPRIMTYPEPOINTLIST);
    RWASSERT(verts);
    RWASSERT(_RwDlTexture);

    _rw2DRenderPrimitiveInit();

    GXBegin(_rwDlPrimConvTbl[primType], GX_VTXFMT0, numIndices);

    switch (primType)
    {
        case rwPRIMTYPEPOLYLINE:
        case rwPRIMTYPETRISTRIP:
        case rwPRIMTYPETRIFAN:
        {
            RWASSERT(numIndices > 1);

            if (RwTextureGetRaster(_RwDlTexture))
            {
                for (i = 0; i < numIndices; i++, indices++)
                {
                    v = &verts[indices[0]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);
                }
            }
            else
            {
                for (i = 0; i < numIndices; i++, indices++)
                {
                    v = &verts[indices[0]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                }
            }

            break;
        }

        case rwPRIMTYPELINELIST:
        {
            const RwInt32 numLines = numIndices >> 1;
            RWASSERT(numIndices > 1);

            if (RwTextureGetRaster(_RwDlTexture))
            {
                for (i = 0; i < numLines; i++, indices += 2)
                {
                    v = &verts[indices[0]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);

                    v = &verts[indices[1]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);
                }
            }
            else
            {
                for (i = 0; i < numLines; i++, indices += 2)
                {
                    v = &verts[indices[0]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);

                    v = &verts[indices[1]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                }
            }

            break;
        }

        case rwPRIMTYPETRILIST:
        {
            const RwInt32 numTriangles = numIndices / 3;
            RWASSERT(numIndices > 2);

            if (RwTextureGetRaster(_RwDlTexture))
            {
                for (i = 0; i < numTriangles; i++, indices += 3)
                {
                    v = &verts[indices[0]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);

                    v = &verts[indices[1]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);

                    v = &verts[indices[2]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                    GXTexCoord2f32(v->u, v->v);
                }
            }
            else
            {
                for (i = 0; i < numTriangles; i++, indices += 3)
                {
                    v = &verts[indices[0]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);

                    v = &verts[indices[1]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);

                    v = &verts[indices[2]];

                    GXPosition3f32(v->x, v->y, v->z);
                    GXColor4u8(v->col.red, v->col.green, v->col.blue, v->col.alpha);
                }
            }

            break;
        }

        default:
            RWASSERT(FALSE);
            RWERROR((E_RX_INVALIDPRIMTYPE));
            break;
    }

    GXEnd();

    _rw2DRenderPrimativeTerm();

    RWRETURN(TRUE);
}