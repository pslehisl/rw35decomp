#include <stdio.h>

#include "batypes.h"
#include "balibtyp.h"
#include "osintf.h"

#include "bafsys.h"

static RwBool
rwfexist(const RwChar *name)
{
    void* fptr;
    RwBool result;

    RWFUNCTION(RWSTRING("rwfexist"));
    RWASSERT(name);

    fptr = RwFopen(name, "rb");
    result = (fptr != NULL);

    if (fptr)
    {
        RwFclose(fptr);
    }

    RWRETURN(result);
}

RwBool
_rwFileSystemOpen(void)
{
    RWFUNCTION(RWSTRING("_rwFileSystemOpen"));
    
    RWSRCGLOBAL(fileFuncs).rwfexist = (rwFnFexist)rwfexist;
    RWSRCGLOBAL(fileFuncs).rwfopen = (rwFnFopen)fopen;
    RWSRCGLOBAL(fileFuncs).rwfclose = (rwFnFclose)fclose;
    RWSRCGLOBAL(fileFuncs).rwfread = (rwFnFread)fread;
    RWSRCGLOBAL(fileFuncs).rwfwrite = (rwFnFwrite)fwrite;
    RWSRCGLOBAL(fileFuncs).rwfgets = (rwFnFgets)fgets;
    RWSRCGLOBAL(fileFuncs).rwfputs = (rwFnFputs)fputs;
    RWSRCGLOBAL(fileFuncs).rwfeof = (rwFnFeof)feof;
    RWSRCGLOBAL(fileFuncs).rwfseek = (rwFnFseek)fseek;
    RWSRCGLOBAL(fileFuncs).rwfflush = (rwFnFflush)fflush;
    RWSRCGLOBAL(fileFuncs).rwftell = (rwFnFtell)ftell;

    RWRETURN(TRUE);
}

void
_rwFileSystemClose(void)
{
    RWFUNCTION(RWSTRING("_rwFileSystemClose"));
    RWRETURNVOID();
}