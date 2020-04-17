/***************************************************************************
 *                                                                         *
 * Module  : dlraster.h                                                    *
 *                                                                         *
 * Purpose : Dolphin raster header                                         *
 *                                                                         *
 **************************************************************************/

#ifndef DLRASTER_H
#define DLRASTER_H

/****************************************************************************
 Includes
 */

#include <dolphin.h>

#include "batypes.h"
#include "baimmedi.h"

/****************************************************************************
 Global Types
 */

typedef struct _rwGameCubeRasterExt _rwGameCubeRasterExt;
struct _rwGameCubeRasterExt
{
    RwUInt8 unknown[0x30];
    RwUInt8 unk30;
    RwUInt8 unk31;
    RwUInt8 unk32;
    RwUInt8 unk33;
};

/****************************************************************************
 Defines
 */
#define RASTEREXTFROMRASTER(raster) \
    ((_rwGameCubeRasterExt *)(((RwUInt8 *)(raster)) + _RwGameCubeRasterExtOffset))

#define RASTEREXTFROMCONSTRASTER(raster)                            \
    ((const _rwGameCubeRasterExt *)                                 \
     (((const RwUInt8 *)(raster)) + _RwGameCubeRasterExtOffset))

/****************************************************************************
 Global variables (across program)
 */

extern RwInt32 _RwGameCubeRasterExtOffset;    /* Raster extension offset */

/****************************************************************************
 Function prototypes
 */

#endif /* DLRASTER_H */
