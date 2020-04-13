/***************************************************************************
 *                                                                         *
 * Module  : ostypes.h                                                     *
 *                                                                         *
 * Purpose : Types                                                         *
 *                                                                         *
 **************************************************************************/

/* RWPUBLIC */
#ifndef GCN_OSTYPES_H
#define GCN_OSTYPES_H
/* RWPUBLICEND */

/****************************************************************************
 Includes
 */

/****************************************************************************
 Library version number
 */

/* RWPUBLIC */

#define rwBIGENDIAN         /* This is a big endian machine */

typedef int RwFixed;
typedef int RwInt32;
typedef unsigned int RwUInt32;
typedef short RwInt16;
typedef unsigned short RwUInt16;
typedef unsigned char RwUInt8;
typedef signed char RwInt8;

#ifdef RWUNICODE
typedef wchar_t RwChar;
#else /* RWUNICODE */
typedef char RwChar;
#endif /* RWUNICODE */
typedef float RwReal;
typedef RwInt32 RwBool;

typedef long long RwInt64;
typedef unsigned long long RwUInt64;

#define RWZERO64 ((RwUInt64)0)

typedef struct _RwUInt128 RwUInt128;
typedef struct _RwInt128 RwInt128;

/* We'll do it with structures
 * (can't do maths on these, but OK for allocation): */
#if (!defined(DOXYGEN))
#ifdef rwBIGENDIAN 
struct _RwUInt128
{
    RwUInt64 top;
    RwUInt64 bottom;
};

struct _RwInt128
{
    RwInt64 top;
    RwUInt64 bottom;
};

#else /* rwBIGENDIAN */
#ifdef rwLITTLEENDIAN
struct _RwUInt128
{
    RwUInt64 bottom;
    RwUInt64 top;
};

struct _RwInt128
{
    RwUInt64 bottom;
    RwInt64 top;
};

#else /* rwLITTLEENDIAN */
#error "ENDIAN-ness undefined!"
#endif /* rwLITTLEENDIAN */
#endif /* rwBIGENDIAN */
#endif /* (!defined(DOXYGEN)) */

#define RWZERO128 { RWZERO64, RWZERO64 }

/* Limits of types */
#define RwInt32MAXVAL       0x7FFFFFFF
#define RwInt32MINVAL       0x80000000
#define RwUInt32MAXVAL      0xFFFFFFFF
#define RwUInt32MINVAL      0x00000000
#define RwRealMAXVAL        (RwReal)(3.40282347e+38)
#define RwRealMINVAL        (RwReal)(1.17549435e-38)
#define RwInt16MAXVAL       0x7FFF
#define RwInt16MINVAL       0x8000
#define RwUInt16MAXVAL      0xFFFF
#define RwUInt16MINVAL      0x0000

/* Structure alignment */
#define RWALIGN(type, x)   type /* nothing */

#if defined (__MWERKS__)

/* Use only the local extent RWFORCEENUMSIZEINT within enum definitions
 * to force 4 byte enums, rather than a global extent pragma.  A pragma
 * is unneccessary since ANSI C enums should be no wider than an int
 * (see section 3.1.3.4 in ANSI C rationale). A global pragma in rwcore.h
 * also applies to application as well as RW enums, which can be undesirable
 * e.g. if some application files includes rwcore.h but others do not
 * leading to a mismatch in enum sizes.
 */
/* #pragma enumsalwaysint on */

#if ((!defined(RWSUPPRESSOPTIMIZATIONPRAGMAS)) && __option(global_optimizer))

/* #pragma inline_instrinsics on */
/* #pragma always_inline on */ /* occassionally hangs  compilation */

#pragma auto_inline on
#pragma dont_inline off
#pragma inline_depth(smart)
#pragma opt_common_subs on
#pragma opt_dead_assignments on
#pragma opt_dead_code on
#pragma opt_lifetimes on
#pragma opt_loop_invariants on
#pragma opt_propagation on
#pragma opt_strength_reduction on
#pragma opt_unroll_loops on
#pragma opt_vectorize_loops on
#pragma peephole on

#if (0x2401 <= __MWERKS__)
/* #pragma fast_fptosi on */
/* #pragma irocseglobaladdresses on */

#if (0x2405 <= __MWERKS__)

/* #pragma opt_pointer_analysis on */ 
/* occassionally hangs  compilation */

/* #pragma fastmath on */ 
/* invokes a sqrtf() -> sqrt.s transformation
 * which may not always give sufficient precision -- see
 * CodeWarrior/CW_Release_Notes/PlayStation2 R3.01/CW PS2 Release Notes.txt 
 * Use rwSqrtMacro() when a lower precision sqrt result is acceptable
 */

#endif /* (0x2405 <= __MWERKS__) */

#endif /* (0x2401 <= __MWERKS__) */

#endif /* ((!defined(RWSUPPRESSOPTIMIZATIONPRAGMAS)) && __option(global_optimizer)) */

#if (defined(RW_USE_SPF))
#pragma float_constants on
#endif /* (defined(RW_USE_SPF)) */

#pragma extended_errorcheck on

#define rwMATRIXALIGNMENT 4
#define rwFRAMEALIGNMENT 4
#define rwV4DALIGNMENT 4

#endif /*  defined (__MWERKS__) */

#if (!defined(rwMATRIXALIGNMENT))
#define rwMATRIXALIGNMENT 32
#endif /* (!defined(rwMATRIXALIGNMENT)) */

#if (!defined(rwFRAMEALIGNMENT))
#define rwFRAMEALIGNMENT 32
#endif /* (!defined(rwFRAMEALIGNMENT)) */

#if (!defined(rwV4DALIGNMENT))
#define rwV4DALIGNMENT 32
#endif /* (!defined(rwV4DALIGNMENT)) */

#if (!defined(rwMALLOCALIGNMENT))
#define rwMALLOCALIGNMENT 32
#endif /* (!defined(rwMALLOCALIGNMENT) */

#define RwFastRealToUInt32(valIn) ((RwUInt32)((RwReal)(valIn)))

#endif /* GCN_OSTYPES_H */

/* RWPUBLICEND */
