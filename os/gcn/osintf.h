#ifndef    _OSINTF_H
#define    _OSINTF_H

/**********************************************************************

 * File:     osintf.h
 *
 * Abstract: Code to provide a device independent interface between
 *           RenderWare and the host operating system. Currently,
 *           only a minimal set of functions are implemented here.
 *           These should be added to over time.
 *
 **********************************************************************/

#include "batypes.h"

#ifdef UNDER_CE
#include <stdarg.h>
#else /* UNDER_CE */
#include <stdio.h>
#endif /* UNDER_CE */

/**********************************************************************/

/* OS independent filename handling */

#define SLASH '\\'

/* Do the TRUE/FALSE thing */
#ifdef FALSE
#undef FALSE
#endif
#define FALSE 0

#ifdef TRUE
#undef TRUE
#endif
#define TRUE !FALSE

#define RwIsAlpha(_c) \
   (  ( ((_c)>='A') && ((_c)<='Z') ) || \
      ( ((_c)>='a') && ((_c)<='z') ) )

/**********************************************************************/

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/* Test whether a filepath is absolute or relative. */
extern int _rwpathisabsolute (const RwChar *path);

/* Get current time string */
extern RwChar *_rwgetcurrenttime(RwChar *buffer, RwInt32 bufSiz);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/**********************************************************************

 * End of file.
 *
 **********************************************************************/

#endif /* _OSINTF_H */
