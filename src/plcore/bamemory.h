/***************************************************************************
 *                                                                         *
 * Module  : bamemory.h                                                    *
 *                                                                         *
 * Purpose : Memory handling                                               *
 *                                                                         *
 **************************************************************************/

#ifndef RWMEMORY_H
#define RWMEMORY_H

/****************************************************************************
 Includes
 */

#include <stdlib.h>
#include "batypes.h"

/* RWPUBLIC */
#if (defined(RWMEMDEBUG))
#ifdef _XBOX
/* Need OutputDebugString macros */
#include <xtl.h>
#endif
#endif

/****************************************************************************
 Defines
 */

/*
 * Debug fill bytes for compatibility with MSVC/C++ debug heap
 * See
 * \Program Files\Microsoft Visual Studio\VC98\CRT\SRC\DBGHEAP.C:
 * static unsigned char _bNoMansLandFill = 0xFD;
 *   // fill no-man's land with this
 * static unsigned char _bDeadLandFill   = 0xDD;
 *   // fill free objects with this
 * static unsigned char _bCleanLandFill  = 0xCD;
 *   // fill new objects with this
 */

#if (!defined(rwFREELISTNOMANSLANDFILL))
#define rwFREELISTNOMANSLANDFILL    0xFD
#endif /* (!defined(rwFREELISTNOMANSLANDFILL)) */

#if (!defined(rwFREELISTDEADLANDFILL))
#define rwFREELISTDEADLANDFILL      0xDD
#endif /* (!defined(rwFREELISTDEADLANDFILL)) */

#if (!defined(rwFREELISTCLEANLANDFILL))
#define rwFREELISTCLEANLANDFILL     0xCD
#endif /* (!defined(rwFREELISTCLEANLANDFILL)) */

#define RWFREELISTALIGNED(_pData, _freelist) \
  (! (((RwUInt32)(_pData)) & ((_freelist)->alignment - 1)) )

/*****************************
 * REGULAR MEMORY ALLOCATION *
 *****************************/

/**
 * \ingroup memoryfileinterface
 * \def RwMalloc
 * RwMalloc(_s) is a macro for malloc(_s).
 */

/**
 * \ingroup memoryfileinterface
 * \def RwFree
 * RwFree(_p) is a macro for free(_p).
 */

/**
 * \ingroup memoryfileinterface
 * \def RwCalloc
 * RwCalloc(_n, _s) is a macro for calloc(_n, _s).
 */

/**
 * \ingroup memoryfileinterface
 * \def RwRealloc
 * RwRealloc(_p, _s) is a macro for realloc(_p, _s).
 */

#if ( (defined(RWMEMDEBUG)) && defined(RWDEBUG) )

#if (!defined(RWNOFREELISTS))
#define RWNOFREELISTS
#endif /* (!defined(RWNOFREELISTS)) */

#if (defined(rwPLUGIN_ID))
#define _CLIENT_TAG \
    ( 0xFFFF & (rwPLUGIN_ID) )
#endif /* (defined(rwPLUGIN_ID)) */

#if (!defined(_CLIENT_TAG))
#define _CLIENT_TAG \
    ( 0xFFFF & (MAKECHUNKID(rwVENDORID_CRITERIONTK, 0x00) ) )
#endif /* (!defined(_CLIENT_TAG)) */

# if (defined(_MSC_VER))
#  if ((_MSC_VER>=1000) && defined(_DEBUG))

#if (defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC))
#define _CRTDBG_MAP_ALLOC
#endif /* defined(RWMEMDEBUG) && !defined(_CRTDBG_MAP_ALLOC)) */
#include <crtdbg.h>

#define RwMalloc(_s)                                    \
    _malloc_dbg((_s),                                   \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),     \
               __FILE__,                                \
               __LINE__)

#define RwFree(_p)                                      \
   _free_dbg((_p),                                      \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16))

#define RwCalloc(_n, _s)                                \
   _calloc_dbg((_n), (_s),                              \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),     \
               __FILE__,                                \
               __LINE__)

#define RwRealloc(_p, _s)                               \
   _realloc_dbg((_p),                                   \
                (_s),                                   \
               _CLIENT_BLOCK | ((_CLIENT_TAG)<<16),     \
               __FILE__,                                \
               __LINE__)

#define     RWCRTORDBGFLAG(_flag)                       \
    do                                                  \
    {                                                   \
        int            _DbgFlag;                        \
                                                        \
        _DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG); \
        _DbgFlag |= (_flag);                            \
        _CrtSetDbgFlag(_DbgFlag);                       \
    }   while(0)

#define VALID_HEAP_STR \
  __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): valid heap\n"

#define     RWCRTCHECKMEMORY()                          \
    do                                                  \
    {                                                   \
        int             valid_heap;                     \
                                                        \
        valid_heap = _CrtCheckMemory();                 \
        _ASSERTE(valid_heap);                           \
    }   while(0)

/*
 *      if (valid_heap)                                 \
 *          OutputDebugString(VALID_HEAP_STR);          \
 */

#define NO_LEAKS_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): no heap leaks found\n"

#define     RWCRTDUMPMEMORYLEAKS()                      \
    do                                                  \
    {                                                   \
        int             leaks_found;                    \
                                                        \
        leaks_found = _CrtDumpMemoryLeaks();            \
        _ASSERTE(!leaks_found);                         \
        if (!leaks_found)                               \
            OutputDebugString(NO_LEAKS_FOUND_STR);      \
    }   while(0)

#define HEAP_DIFFERENCES_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): heap differences found\n"

#define NO_DIFFERENCES_FOUND_STR \
   __FILE__##"("##RW_STRINGIFY_EXPANDED(__LINE__)##"): no heap differences found\n"

#define RWCRTHEAPDIFFERENCESINCE(_Then)                                   \
    do                                                                    \
    {                                                                     \
        /* only dump differences when                                     \
         * there are in fact differences */                               \
        _CrtMemState _Now;                                                \
        _CrtMemState _Delta;                                              \
        const int _DbgFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);         \
        int Differences;                                                  \
                                                                          \
        _CrtMemCheckpoint(&_Now);                                         \
        _CrtMemDifference(&_Delta, _Then, &_Now);                         \
                                                                          \
        (Differences) = ( ( 0 != _Delta.lCounts[_CLIENT_BLOCK] ) ||       \
                          ( 0 != _Delta.lCounts[_NORMAL_BLOCK] ) ||       \
                          ( (_DbgFlag & _CRTDBG_CHECK_CRT_DF) &&          \
                            ( 0 != _Delta.lCounts[_CRT_BLOCK]) ) );       \
                                                                          \
        if ( (Differences) )                                              \
        {                                                                 \
            /* difference detected: dump objects since _Then. */          \
            OutputDebugString(HEAP_DIFFERENCES_FOUND_STR);                \
            _CrtMemDumpAllObjectsSince(_Then);                            \
            _CrtMemDumpStatistics(&_Delta);                               \
        }                                                                 \
        else                                                              \
        {                                                                 \
            OutputDebugString(NO_DIFFERENCES_FOUND_STR);                  \
        }                                                                 \
    }   while (0)

#define RWCRTDBGBREAK() \
    _CrtDbgBreak()

#define RWCRTDOFORALLCLIENTOBJECTS(_f, _c) \
    _CrtDoForAllClientObjects(_f, _c)

#define RWCRTISMEMORYBLOCK(_p, _t, _r, _f, _l) \
    _CrtIsMemoryBlock(_p, _t, _r, _f, _l)

#define RWCRTISVALIDHEAPPOINTER(_p) \
    _CrtIsValidHeapPointer(_p)

#define RWCRTISVALIDPOINTER(_p, _n, _r) \
    _CrtIsValidPointer(_p, _n, _r)

#define RWCRTMEMCHECKPOINT(_s) \
    _CrtMemCheckpoint(_s)

#define RWCRTMEMDIFFERENCE(_s1, _s2, _s3) \
    _CrtMemDifference(_s1, _s2, _s3)

#define RWCRTMEMDUMPALLOBJECTSSINCE(_s) \
    _CrtMemDumpAllObjectsSince(_s)

#define RWCRTMEMDUMPSTATISTICS(_s) \
    _CrtMemDumpStatistics(_s)

#define RWCRTSETALLOCHOOK(_f) \
    _CrtSetAllocHook(_f)

#define RWCRTSETBREAKALLOC(_a) \
    _CrtSetBreakAlloc(_a)

#define RWCRTSETDBGFLAG(_f) \
    _CrtSetDbgFlag(_f)

#define RWCRTSETDUMPCLIENT(_f) \
    _CrtSetDumpClient(_f)

#define RWCRTSETREPORTFILE(_t, _f) \
    _CrtSetReportFile(_t, _f)

#define RWCRTSETREPORTHOOK(_f) \
    _CrtSetReportHook(_f)

#define RWCRTSETREPORTMODE(_t, _f) \
    _CrtSetReportMode(_t, _f)

#if (!defined(_CRTDBG_FLAGS))
#define _CRTDBG_FLAGS                                           \
    ( (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_DELAY_FREE_MEM_DF |       \
       _CRTDBG_CHECK_CRT_DF | _CRTDBG_LEAK_CHECK_DF) &          \
     ~(_CRTDBG_CHECK_ALWAYS_DF |_CRTDBG_RESERVED_DF) )
#endif /* (!defined(_CRTDBG_FLAGS)) */

#  endif /* ((_MSC_VER>=1000) && defined(_DEBUG)) */
# endif /* (defined(_MSC_VER)) */



#if (!defined(rwDEADPTRFILL))
#define rwDEADPTRFILL      ((void *)0xDDDDDDDD)
#endif /* (!defined(rwDEADPTRFILL)) */

#endif /* (defined(RWDEBUG) && (defined(RWMEMDEBUG))) */

#if (!defined(rwDEADPTRFILL))
#define rwDEADPTRFILL      (NULL)
#endif /* (!defined(rwDEADPTRFILL)) */

#if (!defined(RwMalloc))
#define RwMalloc(_s)   ((RWSRCGLOBAL(memoryFuncs).rwmalloc)((_s)))
#endif /* (!defined(RwMalloc)) */

#if (!defined(RwFree))
#define RwFree(_p)     ((RWSRCGLOBAL(memoryFuncs).rwfree)((_p)))
#endif /* (!defined(RwFree)) */

#if (!defined(RwCalloc))
#define RwCalloc(_n, _s)   ((RWSRCGLOBAL(memoryFuncs).rwcalloc)((_n), (_s)))
#endif /* (!defined(RwCalloc)) */

#if (!defined(RwRealloc))
#define RwRealloc(_p, _s)  ((RWSRCGLOBAL(memoryFuncs).rwrealloc)((_p),(_s)))
#endif /* (!defined(RwRealloc)) */

#if (!defined(RWCRTORDBGFLAG))
#define     RWCRTORDBGFLAG(_flag) /* No op */
#endif /* (!defined(RWCRTORDBGFLAG)) */

#if (!defined(RWCRTCHECKMEMORY))
#define RWCRTCHECKMEMORY()     /* No Op */
#endif /* (!defined(RWCRTCHECKMEMORY)) */

#if (!defined(RWCRTDBGBREAK))
#define RWCRTDBGBREAK()        /* No Op */
#endif /* (!defined(RWCRTDBGBREAK)) */

#if (!defined(RWCRTDOFORALLCLIENTOBJECTS))
#define RWCRTDOFORALLCLIENTOBJECTS(_f, _c) /* No Op */
#endif /* (!defined(RWCRTDOFORALLCLIENTOBJECTS)) */

#if (!defined(RWCRTDUMPMEMORYLEAKS))
#define RWCRTDUMPMEMORYLEAKS() /* No Op */
#endif /* (!defined(RWCRTDUMPMEMORYLEAKS)) */

#if (!defined(RWCRTHEAPDIFFERENCESINCE))
#define RWCRTHEAPDIFFERENCESINCE(_Then)           /* No Op */
#endif /* (!defined(RWCRTHEAPDIFFERENCESINCE)) */

#if (!defined(RWCRTISMEMORYBLOCK))
#define RWCRTISMEMORYBLOCK(_p, _t, _r, _f, _l)    (NULL != (_p))
#endif /* (!defined(RWCRTISMEMORYBLOCK)) */

#if (!defined(RWCRTISVALIDHEAPPOINTER))
#define RWCRTISVALIDHEAPPOINTER(_p)               (NULL != (_p))
#endif /* (!defined(RWCRTISVALIDHEAPPOINTER)) */

#if (!defined(RWCRTISVALIDPOINTER))
#define RWCRTISVALIDPOINTER(_p, _n, _r)           (NULL != (_p))
#endif /* (!defined(RWCRTISVALIDPOINTER)) */

#if (!defined(RWCRTMEMCHECKPOINT))
#define RWCRTMEMCHECKPOINT(_s) /* No Op */
#endif /* (!defined(RWCRTMEMCHECKPOINT)) */

#if (!defined(RWCRTMEMDIFFERENCE))
#define RWCRTMEMDIFFERENCE(_s1, _s2, _s3) /* No Op */
#endif /* (!defined(RWCRTMEMDIFFERENCE)) */

#if (!defined(RWCRTMEMDUMPALLOBJECTSSINCE))
#define RWCRTMEMDUMPALLOBJECTSSINCE(_s) /* No Op */
#endif /* (!defined(RWCRTMEMDUMPALLOBJECTSSINCE)) */

#if (!defined(RWCRTMEMDUMPSTATISTICS))
#define RWCRTMEMDUMPSTATISTICS(_s)                (NULL)
#endif /* (!defined(RWCRTMEMDUMPSTATISTICS)) */

#if (!defined(RWCRTSETALLOCHOOK))
#define RWCRTSETALLOCHOOK(_f)                     (NULL)
#endif /* (!defined(RWCRTSETALLOCHOOK)) */

#if (!defined(RWCRTSETBREAKALLOC))
#define RWCRTSETBREAKALLOC(_a)                    (0)
#endif /* (!defined(RWCRTSETBREAKALLOC)) */

#if (!defined(RWCRTSETDBGFLAG))
#define RWCRTSETDBGFLAG(_f)                       (0)
#endif /* (!defined(RWCRTSETDBGFLAG)) */

#if (!defined(RWCRTSETDUMPCLIENT))
#define RWCRTSETDUMPCLIENT(_f)                    (NULL)
#endif /* (!defined(RWCRTSETDUMPCLIENT)) */

#if (!defined(RWCRTSETREPORTFILE))
#define RWCRTSETREPORTFILE(_t, _f)                (NULL)
#endif /* (!defined(RWCRTSETREPORTFILE)) */

#if (!defined(RWCRTSETREPORTHOOK))
#define RWCRTSETREPORTHOOK(_f)                    (NULL)
#endif /* (!defined(RWCRTSETREPORTHOOK)) */

#if (!defined(RWCRTSETREPORTMODE))
#define RWCRTSETREPORTMODE(_t, _f)                (0)
#endif /* (!defined(RWCRTSETREPORTMODE)) */

#if (!defined(RWREGSETBREAKALLOC))
#define RWREGSETBREAKALLOC(_name) /* No op */
#endif /* (!defined(RWREGSETBREAKALLOC)) */

#if (!defined(RWREGSETASSERTPRINT))
#define RWREGSETASSERTPRINT(_name) /* No op */
#endif /* (!defined(RWREGSETASSERTPRINT)) */

#if (!defined(RWGETWINREGDWORD))
#define  RWGETWINREGDWORD(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGDWORD)) */

#if (!defined(RWGETWINREGBINARY))
#define  RWGETWINREGBINARY(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGBINARY)) */

#if (!defined(RWGETWINREGSTRING))
#define  RWGETWINREGSTRING(_env_var, _match) /* No op */
#endif /* (!defined(RWGETWINREGSTRING)) */

#if (!defined(_CRTDBG_FLAGS))
#define  _CRTDBG_FLAGS 0x33
#endif /* (!defined(_CRTDBG_FLAGS)) */

/****************************************************************************
 Global Types
 */

typedef struct RwMemoryFunctions RwMemoryFunctions;
/**
 * \ingroup memoryfileinterface
 * \struct RwMemoryFunctions
 * This type represents the memory functions used
 * by RenderWare. By default, the standard ANSI functions are used. The
 * application may install an alternative interface providing that it is ANSI
 * compliant (In RenderWare Graphics see API function RwEngineInit):
 */
struct RwMemoryFunctions
{
    /* c.f.
     * Program Files/Microsoft Visual Studio/VC98/Include/MALLOC.H
     */
    void *(*rwmalloc)(size_t size);                   /**< rwmalloc   malloc */
    void  (*rwfree)(void *mem);                       /**< rwfree     free */
    void *(*rwrealloc)(void *mem, size_t newSize);    /**< rwrealloc  realloc */
    void *(*rwcalloc)(size_t numObj, size_t sizeObj); /**< calloc     calloc */
};

 /**
 * \ingroup rwfreelist
 * The free list was statically allocated
 *
 * \see RwFreeListSetFlags
 */
#define rwFREELISTFLAG_STATIC 0x00000001

/**
 * \ingroup rwfreelist
 * \hideinitializer
 * Free blocks as soon as they are empty
 *
 * \see RwFreeListSetFlags
 */
#define rwFREELISTFLAG_FREEBLOCKS 0x00000002


typedef struct RwFreeList RwFreeList;

/**
 * \ingroup rwfreelist
 * Holds free list info, should be considered opaque. Use API functions to access.
 */
struct RwFreeList
{
    RwUInt32   entrySize;       /**<size of an entry in the free list */
#if (defined(RWDEBUG) && !defined(DOXYGEN))
    RwUInt32   nonAlignedEntrySize;
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */
    RwUInt32   entriesPerBlock; /**<number of entries per free list block */
    RwUInt32   heapSize;        /**<size of the heap */
    RwUInt32   alignment;       /**<alignment of a free list entry */
    RwLinkList blockList;       /**<list of data blocks */
    RwUInt32   flags;           /**<flags which affect the behavior of the
                                    free list <BR>
                                    rwFREELISTFLAG_FREEBLOCKS */
    RwLLLink   link;            /**<link to the free list linked list */

#if (defined(RWDEBUG) && !defined(DOXYGEN))
    const RwChar       *fileCreate;
    RwUInt32            lineCreate;
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */
};
#ifndef RWADOXYGENEXTERNAL
/**
 * \ingroup rwfreelist
 * \ref RwFreeListCallBack represents
 * the function called from \ref RwFreeListForAllUsed for all used entries in a
 * given free list.
 *
 * \param  pMem   Pointer to the start of the current entries.
 *
 * \param  pData   User-defined data pointer.
 *
 * \see RwFreeListForAllUsed
 *
 */
#endif /* RWADOXYGENEXTERNAL */
typedef void        (*RwFreeListCallBack) (void *pMem, void *pData);
typedef void       *(*RwMemoryAllocFn)    (RwFreeList * fl);
typedef RwFreeList *(*RwMemoryFreeFn)     (RwFreeList * fl, void *pData);

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern              "C"
{
#endif                          /* __cplusplus */

extern RwMemoryFunctions *RwOsGetMemoryInterface(void);

/*************
 * FREELISTS *
 *************/

/* Allocation and freeing */
#if (defined(RWDEBUG) && !defined(DOXYGEN))

extern RwFreeList  *_rwFreeListCreate(RwInt32 entrySize,
                                      RwInt32 entriesPerBlock,
                                      RwInt32 alignment,
                                      const RwChar *fileCreate,
                                      RwUInt32 lineCreate );

#define RwFreeListCreate(entrySize, entriesPerBlock, alignment) \
                    _rwFreeListCreate(entrySize,                \
                                      entriesPerBlock,          \
                                      alignment,                \
                                      __FILE__,                 \
                                      __LINE__)
#else /* (defined(RWDEBUG) && !defined(DOXYGEN)) */

/* legacy freelist create */

extern RwFreeList  *RwFreeListCreate(RwInt32 entrySize,
                                     RwInt32 entriesPerBlock,
                                     RwInt32 alignment);
#endif /* (defined(RWDEBUG) && !defined(DOXYGEN)) */

extern RwFreeList* 
RwFreeListCreateAndPreallocateSpace(RwInt32 entrySize,
                                    RwInt32 entriesPerBlock,
                                    RwInt32 alignment,
                                    RwInt32 numBlocksToPreallocate,
                                    RwFreeList *inPlaceSpaceForFreeListStruct );

extern RwBool       RwFreeListDestroy(RwFreeList * freelist);

extern void RwFreeListSetFlags( RwFreeList *freeList, RwUInt32 flags );
extern RwUInt32 RwFreeListGetFlags( RwFreeList *freeList );

/* RWPUBLICEND */

extern void        *_rwFreeListAllocReal(RwFreeList * freelist);
extern RwFreeList  *_rwFreeListFreeReal(RwFreeList * freelist,
                                        void *pData);

/* RWPUBLIC */
/* Garbage collection/enumeration */
extern RwInt32      RwFreeListPurge(RwFreeList * freelist);
extern RwFreeList  *RwFreeListForAllUsed(RwFreeList * freelist,
                                         RwFreeListCallBack
                                         fpCallBack, void *pData);
extern RwInt32      RwFreeListPurgeAllFreeLists(void);

/* RWPUBLICEND */

/* Opening/Closing */
extern void         _rwMemoryClose(void);
extern RwBool       _rwMemoryOpen(const RwMemoryFunctions * memFuncs);
extern void         _rwFreeListEnable(RwBool enabled);

/* RWPUBLIC */
#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#if (defined(RWDEBUG) && defined(RWNOFREELISTS) && !defined(RWKEEPFREELISTS))

#if ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__))

/*
 * for more on memalign, see
 * http://www.gnu.org/manual/glibc-2.0.6/html_chapter/libc_3.html#SEC28
 */
#include <rtdbmalloc.h>

#define RwFreeListAlloc(_f)     \
    memalign(((_f)->alignment),  (_f)->entrySize)

#else /* ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__)) */

#define RwFreeListAlloc(_f)     \
    RwMalloc((_f)->entrySize)

#endif /* ((defined(__MWERKS__) || defined(__GNUC__)) && defined(__R5900__)) */

#define RwFreeListFree(_f, _p)  \
    RwFree((_p))

#endif /* (defined(RWDEBUG) && defined(RWNOFREELISTS) && !defined(RWKEEPFREELISTS)) */

#if (!defined(RwFreeListAlloc))
#define RwFreeListAlloc(_f)     \
    RWSRCGLOBAL(memoryAlloc)(_f)
#endif /* (!defined(RwFreeListAlloc)) */

#if (!defined(RwFreeListFree))
#define RwFreeListFree(_f, _p)  \
    RWSRCGLOBAL(memoryFree)(_f, _p)
#endif /* (!defined(RwFreeListFree)) */

/* RWPUBLICEND */

#endif                          /* RWMEMORY_H */

