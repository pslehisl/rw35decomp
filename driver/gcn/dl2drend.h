/***************************************************************************
 *                                                                         *
 * Module  : dl2drend.h                                                    *
 *                                                                         *
 * Purpose : 2D primitive rendering functions.                             *
 *                                                                         *
 **************************************************************************/

#ifndef DL2DREND_H
#define DL2DREND_H

extern void _rw2DRenderPrimitiveInit(void);

extern RwBool _rwDlIm2DRenderLine(RwIm2DVertex *verts, RwInt32 numVerts,
                                  RwInt32 vert1, RwInt32 vert2);
extern RwBool _rwDlIm2DRenderTriangle(RwIm2DVertex *verts, RwInt32 numVerts,
                               RwInt32 vert1, RwInt32 vert2, RwInt32 vert3);
extern RwBool _rwDlIm2DRenderPrimitive(RwPrimitiveType primType,
                                       RwIm2DVertex *verts,
                                       RwInt32 numVerts);
extern RwBool _rwDlIm2DRenderIndexedPrimitive(RwPrimitiveType primType,
                                              RwIm2DVertex *verts,
                                              RwInt32 numVerts,
                                              RwImVertexIndex *indices,
                                              RwInt32 numIndices);

#endif /* DL2DREND_H */
