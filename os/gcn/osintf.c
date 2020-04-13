/**********************************************************************
 *
 * File:     dosintf.c
 *
 * Abstract: Code to provide a device independent interface between
 *           RenderWare and the host operating system. Currently,
 *           only a minimal set of functions are implemented here.
 *           These should be added to over time.
 *
 *           This is the DOS version.
 *
 * Modification history:
 * (13/05/93) (CMcC) Created this file.
 *
 **********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "osintf.h"

#include "balibtyp.h"


/**********************************************************************/

int
_rwpathisabsolute(const RwChar *path)
{
    if (path[1] == ':' && RwIsAlpha(*path))
    {
        return(TRUE);
    }

    if (*path == SLASH)
    {
        return(TRUE);
    }

    return(FALSE);
}

/**********************************************************************/

/* Get current time string */
RwChar *
_rwgetcurrenttime(RwChar *buffer, RwInt32 bufSiz)
{
    time_t      t;
    RwChar      *timeString;

    time(&t);
#ifdef RWUNICODE
    timeString = _wctime(&t);
#else
    timeString = ctime(&t);
#endif

    if (timeString)
    {
        /* Will it fit in the buffer (take account of \0) */
        if (rwstrlen(timeString) < (unsigned)(bufSiz-1))
        {
            rwstrcpy(buffer, timeString);
            return(buffer);
        }
    }

    /* Won't fit, or failure in ctime() */
    return(NULL);
}

/**********************************************************************
 *
 * End of file.
 *
 **********************************************************************/
