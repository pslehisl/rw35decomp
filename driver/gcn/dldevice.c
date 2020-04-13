/*
 * Copyright (C) 2000 Criterion Software Limited
 */

/*
 * Includes
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

/* Dolphin Libraries */
#include <dolphin.h>

/* RenderWare types */
#include "batypes.h"
#include "batype.h"
#include "balibtyp.h"
#include "barwtyp.h"
#include "bavector.h"

/* Abstraction of string functionality  -- for unicode support */
#include "rwstring.h"

#include "bamemory.h"
#include "badevice.h"
#include "batextur.h"
#include "baimmedi.h"
#include "baraster.h"

#include "drvfns.h"
#include "dldevice.h"
#include "dlraster.h"
#include "dlrendst.h"
#include "dl2drend.h"
#include "dlconvrt.h"
#include "dltexdic.h"
#include "dltextur.h"

#include "nodeDolphinSubmitNoLight.h"

/* This files header */
#include "dldevice.h"

/****************************************************************************
 Local defines
 */

/****************************************************************************
 Local types
 */

/****************************************************************************
 Local (Static) Prototypes
 */

/****************************************************************************
 Globals (across program)
 */

RwBool _RwDlFSAA = FALSE;
RwBool _RwDlFSAATop = TRUE;
GXRenderModeObj *_RwDlRenderMode = NULL;
RwInt32 _RwDlHalfHeight = 0;

/****************************************************************************
 Globals (prototypes)
 */