/***************************************************************************
 *                                                                         *
 * Module  : palquant.h                                                        *
 *                                                                         *
 * Purpose :                                                               *
 *                                                                         *
 **************************************************************************/

#ifndef PALQUANT_H
#define PALQUANT_H

/****************************************************************************
 Includes
 */
#include "baimage.h"

/****************************************************************************
 Defines
 */

#define RWPALQUANT_MAXDEPTH 8
#define RWPALQUANT_MAXCOLOR (1<<RWPALQUANT_MAXDEPTH)

/****************************************************************************
 Global Types
 */

/**
 * \ingroup rwpalquant
 * \struct RwPalQuant
 * This opaque type is a structure that stores working data for the palette
 * quantization functions. This contains the color quantization tree, which
 * has a maximum depth set globally by \ref RwPalQuantSetMaxDepth.
 *
 * \see RwPalQuantInit
 * \see RwPalQuantAddImage
 * \see RwPalQuantResolvePalette
 * \see RwPalQuantMatchImage
 * \see RwPalQuantTerm
 */
typedef struct RwPalQuant RwPalQuant;

#if (!defined(DOXYGEN))

typedef struct _rwPalQuantRGBABox _rwPalQuantRGBABox;
typedef struct _rwPalQuantOctNode _rwPalQuantOctNode;
typedef struct _rwPalQuantLeafNode _rwPalQuantLeafNode;
typedef struct _rwPalQuantBranchNode _rwPalQuantBranchNode;

struct _rwPalQuantRGBABox
{
    RwInt32 col0[4];    /* min value, inclusive */
    RwInt32 col1[4];    /* max value, exclusive */
};

struct _rwPalQuantLeafNode
{
    /* Represents stats for both true leaf nodes and also branch nodes */
    RwReal      weight;     /* Accumulated weight */
    RwRGBAReal  ac;         /* Accumulated (weight * color) */
    RwReal      var;        /* Weight * variance */
    RwUInt8     palIndex;
};

struct _rwPalQuantBranchNode
{
    _rwPalQuantOctNode *dir[16];
};

/*
struct _rwPalQuantOctNode
{
    _rwPalQuantLeafNode   Leaf;
    _rwPalQuantBranchNode Branch;
};
*/

struct _rwPalQuantOctNode
{
    _rwPalQuantBranchNode Branch;
    _rwPalQuantLeafNode   Leaf;
};

struct RwPalQuant
{
    _rwPalQuantRGBABox  Mcube[RWPALQUANT_MAXCOLOR];
    RwReal              Mvv[RWPALQUANT_MAXCOLOR];
    _rwPalQuantLeafNode Mvol[RWPALQUANT_MAXCOLOR];
    _rwPalQuantOctNode  *root;                    
    RwFreeList          *cubefreelist;            
};

#endif /* (!defined(DOXYGEN)) */

/****************************************************************************
 Function prototypes
 */

/* RWPUBLIC */

#ifdef    __cplusplus
extern          "C"
{
#endif         /* __cplusplus */

extern void
RwPalQuantSetMaxDepth(RwUInt32 depth);

extern RwUInt32
RwPalQuantGetMaxDepth(void);

/* RWPUBLICEND */

extern void
_rwPalQuantAddImage(RwPalQuant *pq, RwImage *img, RwReal weight);


extern RwInt32
_rwPalQuantResolvePalette(RwRGBA *palette, RwInt32 maxcols, RwPalQuant *pq);


extern void
_rwPalQuantMatchImage(RwUInt8 *dstpixels, RwInt32 dststride, RwInt32 dstdepth,
                      RwBool dstPacked, RwPalQuant *pq, RwImage *img);

extern RwBool
_rwPalQuantInit(RwPalQuant *pq);

extern void
_rwPalQuantTerm(RwPalQuant *pq);

/* RWPUBLIC */
#ifdef    __cplusplus
}
#endif         /* __cplusplus */

/* RWPUBLICEND */

#endif /* PALQUANT_H */
