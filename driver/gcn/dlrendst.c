/***************************************************************************
 *                                                                         *
 * Module  : D3D8rendst.c                                                    *
 *                                                                         *
 * Purpose : Renderstates.                                                 *
 *                                                                         *
 **************************************************************************/

 /**************************************************************************
 Includes
 */
#include <dolphin.h>

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
#include "dlraster.h"
#include "dl2drend.h"

/****************************************************************************
 Local Defines
 */

/****************************************************************************
 Global Types
 */

/****************************************************************************
 Globals (across program)
 */

RwTexture *_RwDlTexture = NULL;

/****************************************************************************
 Local (static) Globals
 */