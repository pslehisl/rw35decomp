/***************************************************************************
 *                                                                         *
 * Module  : bafsys.h                                                      *
 *                                                                         *
 * Purpose :                                                               *
 *                                                                         *
 **************************************************************************/

#ifndef RWFSYS_H
#define RWFSYS_H

/****************************************************************************
 Includes
 */

#include "batypes.h"

/****************************************************************************
 Defines
 */

#define RwFexist  RWSRCGLOBAL(fileFuncs).rwfexist
#define RwFopen   RWSRCGLOBAL(fileFuncs).rwfopen
#define RwFclose  RWSRCGLOBAL(fileFuncs).rwfclose
#define RwFread   RWSRCGLOBAL(fileFuncs).rwfread
#define RwFwrite  RWSRCGLOBAL(fileFuncs).rwfwrite
#define RwFgets   RWSRCGLOBAL(fileFuncs).rwfgets
#define RwFputs   RWSRCGLOBAL(fileFuncs).rwfputs
#define RwFeof    RWSRCGLOBAL(fileFuncs).rwfeof
#define RwFseek   RWSRCGLOBAL(fileFuncs).rwfseek
#define RwFflush  RWSRCGLOBAL(fileFuncs).rwfflush
#define RwFtell   RWSRCGLOBAL(fileFuncs).rwftell

/* RWPUBLIC */

/****************************************************************************
 Global Types
 */

/*
 * rwFnFexist
 * Returns TRUE if file with given name exists, FALSE if it doesn't.
 */
typedef RwBool  (*rwFnFexist)(const RwChar *name);

/*
 * rwFnFopen
 * Mimics ANSI C Standard Library fopen.
 */
typedef void   *(*rwFnFopen)(const RwChar *name, const RwChar *mode);

/*
 * rwFnFclose
 * Mimics ANSI C Standard Library fclose.
 */
typedef int     (*rwFnFclose)(void *fptr);

/*
 * rwFnFread
 * Mimics ANSI C Standard Library fread.
 */
typedef size_t  (*rwFnFread)(void *addr, size_t size, size_t count, void *fptr);

/*
 * rwFnFwrite
 * Mimics ANSI C Standard Library fwrite.
 */
typedef size_t  (*rwFnFwrite)(const void *addr, size_t size, size_t count, void *fptr);

/*
 * rwFnFgets
 * Mimics ANSI C Standard Library fgets.
 */
typedef RwChar *(*rwFnFgets)(RwChar *buffer, int maxLen, void *fptr);

/*
 * rwFnFputs
 * Mimics ANSI C Standard Library fputs.
 */
typedef int     (*rwFnFputs)(const RwChar *buffer, void *fptr);

/*
 * rwFnFeof
 * Mimics ANSI C Standard Library feof.
 */
typedef int     (*rwFnFeof)(void *fptr);

/*
 * rwFnFseek
 * Mimics ANSI C Standard Library fseek.
 */
typedef int     (*rwFnFseek)(void *fptr, long offset, int origin);

/*
 * rwFnFflush
 * Mimics ANSI C Standard Library fflush.
 */
typedef int     (*rwFnFflush)(void *fptr);

/*
 * rwFnFtell
 * Mimics ANSI C Standard Library ftell.
 */
typedef int     (*rwFnFtell)(void *fptr);


typedef struct RwFileFunctions RwFileFunctions;
/**
 * \ingroup memoryfileinterface
 * \struct RwFileFunctions
 * This type is used to specify the file access
 * functions used by RenderWare. The default file system uses the standard
 * ANSI functions. The application may install an alternative file system
 * providing it complies with the ANSI interface -- see API 
 * function \ref RwOsGetFileInterface.
 *
 * The function types associated with this type are defined as follows 
 *
 * \verbatim
   RwBool (*rwFnFexist)(const RwChar *name)
       void *(*rwFnFopen)(const RwChar *name, const RwChar *access) 
       int (*rwFnFclose)(void *fptr)
       size_t (*rwFnFread)(void *addr, size_t size, size_t count, void *fptr)                        
       size_t (*rwFnFwrite)(const void *addr, size_t size, size_t count,
                 void *fptr)
   RwChar *(*rwFnFgets)(RwChar *buffer, int maxLen, void *fptr)
       int (*rwFnFputs)(const RwChar *buffer, void *fptr)
       int (*rwFnFeof)(void *fptr)
       int (*rwFnFseek)(void *fptr, long offset, int origin)
       int (*rwFnFflush)(void *fptr)
       int (*rwFnFtell)(void *fptr)
 \endverbatim
 * Note the function argument lists are consistent with the ANSI
 * standard file access interface:
 */
struct RwFileFunctions
{
    rwFnFexist  rwfexist; /**< Pointer to fexist function */
    rwFnFopen   rwfopen;  /**< Pointer to fopen function */
    rwFnFclose  rwfclose; /**< Pointer to fclose function */
    rwFnFread   rwfread;  /**< Pointer to fread function */
    rwFnFwrite  rwfwrite; /**< Pointer to fwrite function */
    rwFnFgets   rwfgets;  /**< Pointer to fgets function */
    rwFnFputs   rwfputs;  /**< Pointer to puts function */
    rwFnFeof    rwfeof;   /**< Pointer to feof function */
    rwFnFseek   rwfseek;  /**< Pointer to fseek function */
    rwFnFflush  rwfflush; /**< Pointer to fflush function */
    rwFnFtell   rwftell;  /**< Pointer to ftell function */  
};


#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

extern RwFileFunctions *RwOsGetFileInterface(void);

/* RWPUBLICEND */

extern RwBool           _rwFileSystemOpen(void);
extern void             _rwFileSystemClose(void);

/* RWPUBLIC */
#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

/* RWPUBLICEND */

#endif /* RWFSYS_H */
