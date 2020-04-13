/*****************************************************************************
 *                                                                           *
 * Module  : dlconvrt.c                                                      *
 *                                                                           *
 * Purpose : Converting to and from images to rasters                        *
 *                                                                           *
 ****************************************************************************/

/*****************************************************************************
 Includes
 */

#include <stdlib.h>
#include <string.h>

#include "batypes.h"
#include "batype.h"
#include "balibtyp.h"
#include "barwtyp.h"
#include "baresour.h"
#include "baimage.h"
#include "badebug.h"
#include "baresamp.h"

#include "palquant.h"

#include "drvfns.h"

#include "dlconvrt.h"
#include "dlraster.h"
#include "dldevice.h"

/*****************************************************************************
 Global prototypes
 */

/*****************************************************************************
 Local Types
 */

typedef RwUInt32 (*DlPixConvertFn)(RwRGBA *colIn);
typedef void (*DlPixUnconvertFn)(RwRGBA *pixOut, RwUInt32 pixIn);

/*****************************************************************************
 Local (Static) Prototypes
 */

/*****************************************************************************
 Globals (across program)
 */

/*****************************************************************************
 Local (static) Globals
 */

/****************************************************************************
 _rwDlFindMSB

 On entry   : LONG
 On exit    : Bit number of MSB (-1 if 0)
 */
RwInt32
_rwDlFindMSB(RwInt32 num)
{
    RwInt32 pos = -1;

    RWFUNCTION(RWSTRING("_rwDlFindMSB"));

    while (num)
    {
        pos++;
        num >>= 1;
    }

    RWRETURN(pos);
}

/*****************************************************************************
 _rwDlConv8888To555

 On entry : A pointer to a RwRGBA to pack.
 On exit  : A RwUInt32 representing the packed RwRGBA.
 */
static RwUInt32 
_rwDlConv8888To555(RwRGBA *pixIn)
{
    RwUInt32    pixOut;

    RWFUNCTION(RWSTRING("_rwDlConv8888To555"));

    pixOut = (0x00008000) |
             (((RwUInt32)pixIn->red << 7) & 0x7C00) |
             (((RwUInt32)pixIn->green << 2) & 0x03E0) |
             (((RwUInt32)pixIn->blue >> 3) & 0x001F);

    RWRETURN(pixOut);
}

/*****************************************************************************
 _rwDlConv8888To565

 On entry : A pointer to a RwRGBA to pack.
 On exit  : A RwUInt32 representing the packed RwRGBA.
 */
static RwUInt32 
_rwDlConv8888To565(RwRGBA *pixIn)
{
    RwUInt32    pixOut;

    RWFUNCTION(RWSTRING("_rwDlConv8888To565"));

    pixOut = (((RwUInt32)pixIn->red << 8) & 0xF800) |
             (((RwUInt32)pixIn->green << 3) & 0x07E0) |
             (((RwUInt32)pixIn->blue >> 3) & 0x001F);

    RWRETURN(pixOut);
}

/*****************************************************************************
 _rwDlConv8888To555or3444

 On entry : A pointer to a RwRGBA to pack.
 On exit  : A RwUInt32 representing the packed RwRGBA.
 */
static RwUInt32 
_rwDlConv8888To555or3444(RwRGBA *pixIn)
{
    RwUInt32    pixOut;

    RWFUNCTION(RWSTRING("_rwDlConv8888To555or3444"));

    if (pixIn->alpha != 0xff)
    {
        /* 3444 */
        pixOut = (((RwUInt32)pixIn->alpha << 7) & 0x7000) |
                 (((RwUInt32)pixIn->red << 4) & 0x0F00) |
                 ((RwUInt32)pixIn->green & 0x00F0) |
                 (((RwUInt32)pixIn->blue >> 4) & 0x000F);
    }
    else
    {
        /* 555 */
        pixOut = (0x00008000) |
                 (((RwUInt32)pixIn->red << 7) & 0x7C00) |
                 (((RwUInt32)pixIn->green << 2) & 0x03E0) |
                 (((RwUInt32)pixIn->blue >> 3) & 0x001F);
    }

    RWRETURN(pixOut);
}

/*****************************************************************************
 _rwDlConv8888ToDl888

 On entry : A pointer to a RwRGBA to pack.
 On exit  : A RwUInt32 representing the packed RwRGBA.
 */
static RwUInt32 
_rwDlConv8888ToDl888(RwRGBA *pixIn)
{
    RwUInt32    pixOut;

    RWFUNCTION(RWSTRING("_rwDlConv8888ToDl888"));

    pixOut = (0xFF000000) |
             ((RwUInt32)pixIn->red << 16) |
             ((RwUInt32)pixIn->green << 8) |
             (RwUInt32)pixIn->blue;

    RWRETURN(pixOut);
}

/*****************************************************************************
 _rwDlConv8888ToDl8888

 On entry : A pointer to a RwRGBA to pack.
 On exit  : A RwUInt32 representing the packed RwRGBA.
 */
static RwUInt32 
_rwDlConv8888ToDl8888(RwRGBA *pixIn)
{
    RwUInt32    pixOut;

    RWFUNCTION(RWSTRING("_rwDlConv8888ToDl8888"));

    pixOut = ((RwUInt32)pixIn->alpha << 24) |
             ((RwUInt32)pixIn->red << 16) |
             ((RwUInt32)pixIn->green << 8) |
             (RwUInt32)pixIn->blue;

    RWRETURN(pixOut);
}

/*****************************************************************************
 _rwDlRGBToPixel

 Convert RwRGBA to device specific format

 On entry   : pixel - device specific colour out
            : col - colour to convert
            : format - pixel format
 On exit    : TRUE on success else FALSE.
 */
RwBool
_rwDlRGBToPixel(void *pixelOut, void *colIn, RwInt32 format)
{
    RwRGBA  *rgba = (RwRGBA *)colIn;
    RwInt32 pixVal;

    RWFUNCTION(RWSTRING("_rwDlRGBToPixel"));

    switch (format & rwRASTERFORMATPIXELFORMATMASK)
    {
        case rwRASTERFORMATDEFAULT:
            pixVal = _rwDlConv8888To565(rgba);
            break;
        
        case rwRASTERFORMATLUM8:
            RWERROR((E_RW_DEVICEERROR, "rwRASTERFORMATLUM8 not yet supported"));
            break;
        
        case rwRASTERFORMAT1555:
        case rwRASTERFORMAT4444:
            pixVal = _rwDlConv8888To555or3444(rgba);
            break;
        
        case rwRASTERFORMAT555:
            pixVal = _rwDlConv8888To555(rgba);
            break;
        
        case rwRASTERFORMAT565:
            pixVal = _rwDlConv8888To565(rgba);
            break;
        
        case rwRASTERFORMAT8888:
            pixVal = _rwDlConv8888ToDl8888(rgba);
            break;
        
        case rwRASTERFORMAT888:
            pixVal = _rwDlConv8888ToDl888(rgba);
            break;
        
        default:
            RWERROR((E_RW_INVRASTERFORMAT));
            break;
    }

    *(RwInt32 *)pixelOut = pixVal;

    RWRETURN(TRUE);
}

/****************************************************************************
 _rwDlConv555To8888

 On entry : A pointer to a RwRGBA that is the unpacked pixel value.
 On exit  : A RwUInt32 representing the packed pixel value.
 */
static void
_rwDlConv555To8888(RwRGBA *pixOut, RwUInt32 pixIn)
{
    RWFUNCTION(RWSTRING("_rwDlConv555To8888"));

    pixOut->red   = (RwUInt8)((pixIn >> 7) & 0xF8);
    pixOut->green = (RwUInt8)((pixIn >> 2) & 0xF8);
    pixOut->blue  = (RwUInt8)((pixIn << 3) & 0xF8);
    pixOut->alpha = 0xFF;

    RWRETURNVOID();
}

/****************************************************************************
 _rwDlConv565To8888

 On entry : A pointer to a RwRGBA that is the unpacked pixel value.
 On exit  : A RwUInt32 representing the packed pixel value.
 */
static void
_rwDlConv565To8888(RwRGBA *pixOut, RwUInt32 pixIn)
{
    RWFUNCTION(RWSTRING("_rwDlConv565To8888"));

    pixOut->red   = (RwUInt8)((pixIn >> 8) & 0xF8);
    pixOut->green = (RwUInt8)((pixIn >> 3) & 0xFC);
    pixOut->blue  = (RwUInt8)((pixIn << 3) & 0xF8);
    pixOut->alpha = 0xFF;

    RWRETURNVOID();
}

/****************************************************************************
 _rwDlConv1555To8888

 On entry : A pointer to a RwRGBA that is the unpacked pixel value.
 On exit  : A RwUInt32 representing the packed pixel value.
 */
static void
_rwDlConv1555To8888(RwRGBA *pixOut, RwUInt32 pixIn)
{
    RWFUNCTION(RWSTRING("_rwDlConv1555To8888"));

    if (pixIn & 0x8000)
    {
        pixOut->red   = (RwUInt8)((pixIn >> 7) & 0xF8);
        pixOut->green = (RwUInt8)((pixIn >> 2) & 0xF8);
        pixOut->blue  = (RwUInt8)((pixIn << 3) & 0xF8);
        pixOut->alpha = 0xFF;
    }
    else
    {
        pixOut->red   = (RwUInt8)((pixIn >> 4) & 0xF0);
        pixOut->green = (RwUInt8)(pixIn & 0xF0);
        pixOut->blue  = (RwUInt8)((pixIn << 4) & 0xF0);
        pixOut->alpha = 0x00;
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwDlConv4444To8888

 On entry : A pointer to a RwRGBA that is the unpacked pixel value.
 On exit  : A RwUInt32 representing the packed pixel value.
 */
static void
_rwDlConv4444To8888(RwRGBA *pixOut, RwUInt32 pixIn)
{
    RWFUNCTION(RWSTRING("_rwDlConv4444To8888"));

    if (pixIn & 0x8000)
    {
        pixOut->red   = (RwUInt8)((pixIn >> 7) & 0xF8);
        pixOut->green = (RwUInt8)((pixIn >> 2) & 0xF8);
        pixOut->blue  = (RwUInt8)((pixIn << 3) & 0xF8);
        pixOut->alpha = 0xFF;
    }
    else
    {
        pixOut->red   = (RwUInt8)((pixIn >> 4) & 0xF0);
        pixOut->green = (RwUInt8)(pixIn & 0xF0);
        pixOut->blue  = (RwUInt8)((pixIn << 4) & 0xF0);
        pixOut->alpha = (RwUInt8)((pixIn >> 7) & 0xE0);
    }

    RWRETURNVOID();
}

/****************************************************************************
 _rwDlConvDl888To8888

 On entry : A pointer to a RwRGBA that is the unpacked pixel value.
 On exit  : A RwUInt32 representing the packed pixel value.
 */
static void
_rwDlConvDl888To8888(RwRGBA *pixOut, RwUInt32 pixIn)
{
    RWFUNCTION(RWSTRING("_rwDlConvDl888To8888"));

    pixOut->alpha = 0xFF;
    pixOut->red   = (RwUInt8)((pixIn >> 16) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue  = (RwUInt8)(pixIn & 0xFF);

    RWRETURNVOID();
}

/****************************************************************************
 _rwDlConvDl8888To8888

 On entry : A pointer to a RwRGBA that is the unpacked pixel value.
 On exit  : A RwUInt32 representing the packed pixel value.
 */
static void
_rwDlConvDl8888To8888(RwRGBA *pixOut, RwUInt32 pixIn)
{
    RWFUNCTION(RWSTRING("_rwDlConvDl8888To8888"));

    pixOut->alpha = (RwUInt8)((pixIn >> 24) & 0xFF);
    pixOut->red   = (RwUInt8)((pixIn >> 16) & 0xFF);
    pixOut->green = (RwUInt8)((pixIn >> 8) & 0xFF);
    pixOut->blue  = (RwUInt8)(pixIn & 0xFF);

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlPixelToRGB

 Convert device specific colour to RwRGBA

 On entry   : rgbOut - RwRGBA out
            : pixel - Pixel in
            : Pixel format
 On exit    : TRUE on success
 */
RwBool
_rwDlPixelToRGB(void *rgbOut, void *pixel, RwInt32 format)
{
    RwRGBA  *rgba = (RwRGBA *)rgbOut;

    RWFUNCTION(RWSTRING("_rwD3D8PixelToRGB"));

    switch (format & rwRASTERFORMATPIXELFORMATMASK)
    {
        case rwRASTERFORMATDEFAULT:
            _rwDlConv565To8888(rgba, *(RwInt32 *)pixel);
            break;
        
        case rwRASTERFORMATLUM8:
            RWERROR((E_RW_DEVICEERROR, "rwRASTERFORMATLUM8 not yet supported"));
            break;
        
        case rwRASTERFORMAT555:
            _rwDlConv555To8888(rgba, *(RwInt32 *)pixel);
            break;
        
        case rwRASTERFORMAT1555:
            _rwDlConv1555To8888(rgba, *(RwInt32 *)pixel);
            break;

        case rwRASTERFORMAT565:
            _rwDlConv565To8888(rgba, *(RwInt32 *)pixel);
            break;
        
        case rwRASTERFORMAT4444:
            _rwDlConv4444To8888(rgba, *(RwInt32 *)pixel);
            break;
        
        case rwRASTERFORMAT888:
            _rwDlConvDl888To8888(rgba, *(RwInt32 *)pixel);
            break;
        
        case rwRASTERFORMAT8888:
            _rwDlConvDl8888To8888(rgba, *(RwInt32 *)pixel);
            break;
        
        default:
            RWERROR((E_RW_INVRASTERFORMAT));
            break;
    }

    RWRETURN(TRUE);
}

/*****************************************************************************
 rwDlSelectUnconvertFn

 On entry :
 On exit  :
 */
static DlPixUnconvertFn
rwDlSelectUnconvertFn(RwRasterFormat format)
{
    DlPixUnconvertFn  result = NULL;

    RWFUNCTION(RWSTRING("rwDlSelectUnconvertFn"));

    switch (format & rwRASTERFORMATPIXELFORMATMASK)
    {
        case rwRASTERFORMATLUM8:
            RWERROR((E_RW_DEVICEERROR, "rwRASTERFORMATLUM8 not yet supported"));
            break;
        
        case rwRASTERFORMAT555:
            result = _rwDlConv555To8888;
            break;
        
        case rwRASTERFORMAT565:
            result = _rwDlConv565To8888;
            break;
        
        case rwRASTERFORMAT1555:
            result = _rwDlConv1555To8888;
            break;
        
        case rwRASTERFORMAT4444:
            result = _rwDlConv4444To8888;
            break;
        
        case rwRASTERFORMAT8888:
            result = _rwDlConvDl8888To8888;
            break;
        
        case rwRASTERFORMAT888:
            result = _rwDlConvDl888To8888;
            break;
        
        default:
            RWERROR((E_RW_INVRASTERFORMAT));
            break;
    }

    RWRETURN(result);
}

/*****************************************************************************
 _rwDlImage4GetFromRaster

 Convert raster into a palettized image

 On entry   : image - destination image to receive the raster's image
            : raster - source raster to convert

 On exit    :
 */
static void
_rwDlImage4GetFromRaster(RwImage *image, RwRaster *raster)
{
    DlPixUnconvertFn  unconvFn;

    RWFUNCTION(RWSTRING("_rwDlImage4GetFromRaster"));

    unconvFn = rwDlSelectUnconvertFn(RwRasterGetFormat(raster));

    switch (RwRasterGetDepth(raster))
    {
    case 0:
        {
            const RwUInt8   *srcPixel;
            RwInt32         j;

            for (j = 0; j < 16; j++)
            {
                unconvFn(&image->palette[j], ((RwUInt16 *)raster->palette)[j]);
            }

            for (j = 0; j < raster->height; j++)
            {
                RwInt32 i;
                RwUInt8 *dstPixel;

                dstPixel = (RwUInt8 *)(image->cpPixels + (image->stride * j));
                srcPixel = (const RwUInt8 *)(raster->cpPixels + (raster->stride * j));

                for (i = 0; i < raster->width; i += 2)
                {
                    dstPixel[0] = (RwUInt8)((*srcPixel >> 4) & 0x0F);
                    dstPixel[1] = (RwUInt8)(*srcPixel & 0x0F);

                    srcPixel++;
                    dstPixel += 2;
                }
            }

            break;
        }
    
    case 4:
    case 12:
    case 28:
        {
            RWERROR((E_RW_DEVICEERROR,
                "Conversion from 8/16/32bit rasters to 4bit images is not supported"));
            break;
        }
    
    /* Dunno if these are real cases, just need enough to force a jumptable :) */
    case 8:
    case 16:
    case 24:
    case 32:
    default:
        RWERROR((E_RW_INVRASTERDEPTH));
        break;
    }

    RWRETURNVOID();
}