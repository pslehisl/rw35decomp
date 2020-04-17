/***************************************************************************
 *                                                                         *
 * Module  : D3D8raster.c                                                    *
 *                                                                         *
 * Purpose :                                                               *
 *                                                                         *
 **************************************************************************/

/****************************************************************************
 Includes
 */

#include <dolphin.h>

#include "batypes.h"
#include "batype.h"
#include "balibtyp.h"
#include "barwtyp.h"
#include "batextur.h"

#include "dldevice.h"
#include "drvfns.h"
#include "dlconvrt.h"
#include "dlrendst.h"
#include "dlraster.h"
#include "dl2drend.h"

/****************************************************************************
 Defines
 */

/****************************************************************************
 Globals (across program)
 */

RwInt32 _RwGameCubeRasterExtOffset;    /* Raster extension offset */

/****************************************************************************
 Global Types
 */

/****************************************************************************
 Local Types
 */

/****************************************************************************
 Local (static)
 */