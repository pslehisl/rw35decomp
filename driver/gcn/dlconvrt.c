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

    RWFUNCTION(RWSTRING("_rwDlPixelToRGB"));

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
    case 4:
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
    
    case 8:
    case 16:
    case 32:
        {
            RWERROR((E_RW_DEVICEERROR,
                "Conversion from 8/16/32bit rasters to 4bit images is not supported"));
            break;
        }
    
    case 24:
    default:
        RWERROR((E_RW_INVRASTERDEPTH));
        break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlImage8GetFromRaster

 Convert raster into a palettized image

 On entry   : image - destination image to receive the raster's image
            : raster - source raster to convert

 On exit    :
 */
static void
_rwDlImage8GetFromRaster(RwImage *image, RwRaster *raster)
{
    DlPixUnconvertFn  unconvFn;

    RWFUNCTION(RWSTRING("_rwDlImage8GetFromRaster"));

    unconvFn = rwDlSelectUnconvertFn(RwRasterGetFormat(raster));

    switch (RwRasterGetDepth(raster))
    {
    case 4:
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
    
    case 8:
        {
            const RwUInt8   *srcPixel;
            RwInt32         j;

            for (j = 0; j < 256; j++)
            {
                unconvFn(&image->palette[j], ((RwUInt16 *)raster->palette)[j]);
            }

            for (j = 0; j < raster->height; j++)
            {
                RwInt32 i;
                RwUInt8 *dstPixel;

                srcPixel = (const RwUInt8 *)(raster->cpPixels + (raster->stride * j));
                dstPixel = (RwUInt8 *)(image->cpPixels + (image->stride * j));

                for (i = 0; i < raster->width; i++)
                {
                    *dstPixel = *srcPixel;

                    srcPixel++;
                    dstPixel++;
                }
            }

            break;
        }

    case 16:
    case 32:
        {
            RWERROR((E_RW_DEVICEERROR,
                "Conversion from 16/32bit rasters to 8bit images is not supported"));
            break;
        }
    
    case 24:
    default:
        RWERROR((E_RW_INVRASTERDEPTH));
        break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlImage32GetFromRaster

 Convert raster into a palettized image

 On entry   : image - destination image to receive the raster's image
            : raster - source raster to convert

 On exit    :
 */
static void
_rwDlImage32GetFromRaster(RwImage *image, RwRaster *raster)
{
    DlPixUnconvertFn  unconvFn;

    RWFUNCTION(RWSTRING("_rwDlImage32GetFromRaster"));

    unconvFn = rwDlSelectUnconvertFn(RwRasterGetFormat(raster));

    switch (RwRasterGetDepth(raster))
    {
    case 4:
        {
            RwUInt32        palette[16];
            const RwUInt8   *srcPixel;
            RwInt32         j;

            for (j = 0; j < 16; j++)
            {
                unconvFn((RwRGBA *)&palette[j], ((RwUInt16 *)raster->palette)[j]);
            }

            for (j = 0; j < raster->height; j++)
            {
                RwInt32     i;
                RwUInt32    *dstPixel;

                dstPixel = (RwUInt32 *)(image->cpPixels + (image->stride * j));
                srcPixel = (const RwUInt8 *)(raster->cpPixels + (raster->stride * j));

                for (i = 0; i < raster->width; i += 2)
                {
                    dstPixel[0] = palette[(*srcPixel >> 4) & 0x0F];
                    dstPixel[1] = palette[*srcPixel & 0x0F];

                    srcPixel++;
                    dstPixel += 2;
                }
            }

            break;
        }
    
    case 8:
        {
            RwUInt32        palette[256];
            const RwUInt8   *srcPixel;
            RwInt32         j;

            for (j = 0; j < 256; j++)
            {
                unconvFn((RwRGBA *)&palette[j], ((RwUInt16 *)raster->palette)[j]);
            }

            for (j = 0; j < raster->height; j++)
            {
                RwInt32 i;
                RwUInt32 *dstPixel;

                srcPixel = (const RwUInt8 *)(raster->cpPixels + (raster->stride * j));
                dstPixel = (RwUInt32 *)(image->cpPixels + (image->stride * j));

                for (i = 0; i < raster->width; i++)
                {
                    *dstPixel = palette[*srcPixel];

                    srcPixel++;
                    dstPixel++;
                }
            }

            break;
        }

    case 16:
        {
            RwInt32         j;

            for (j = 0; j < raster->height; j++)
            {
                RwInt32 i;
                const RwUInt16  *srcPixel;
                RwRGBA *dstPixel;

                srcPixel = (const RwUInt16 *)(raster->cpPixels + (raster->stride * j));
                dstPixel = (RwRGBA *)(image->cpPixels + (image->stride * j));

                for (i = 0; i < raster->width; i++)
                {
                    unconvFn(dstPixel, *srcPixel);

                    dstPixel++;
                    srcPixel++;
                }
            }

            break;
        }
    
    case 24:
        {
            RWERROR((E_RW_INVRASTERDEPTH));
            break;
        }
    
    case 32:
        {
            RwInt32         j;

            for (j = 0; j < raster->height; j++)
            {
                RwInt32 i;
                const RwUInt32  *srcPixel;
                RwRGBA *dstPixel;

                srcPixel = (const RwUInt32 *)(raster->cpPixels + (raster->stride * j));
                dstPixel = (RwRGBA *)(image->cpPixels + (image->stride * j));

                for (i = 0; i < raster->width; i++)
                {
                    unconvFn(dstPixel, *srcPixel);

                    dstPixel++;
                    srcPixel++;
                }
            }

            break;
        }
    
    default:
        RWERROR((E_RW_INVRASTERDEPTH));
        break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlImageGetFromRaster

 Convert raster to image

 On entry   : image - destination image to receive the raster's image
            : raster - source raster to convert
 On exit    : TRUE on success
 */
RwBool
_rwDlImageGetFromRaster(void *imageIn, 
                        void *rasterIn, 
                        RwInt32 unused3 __RWUNUSED__)
{
    RwImage     *image = (RwImage *)imageIn;
    RwRaster    *raster = (RwRaster *)rasterIn;
    RwBool      rasterLocked = FALSE;
    RwBool      paletteLocked = FALSE;

    RWFUNCTION(RWSTRING("_rwDlImageGetFromRaster"));

    if (!(raster->privateFlags & rwRASTERPIXELLOCKEDREAD))
    {
        RwRasterLock(raster, 0, rwRASTERLOCKREAD);
        rasterLocked = TRUE;
    }

    if (RwRasterGetFormat(raster) & (rwRASTERFORMATPAL4 | rwRASTERFORMATPAL8))
    {
        if (!(raster->privateFlags & rwRASTERPALETTELOCKEDREAD))
        {
            RwRasterLockPalette(raster, rwRASTERLOCKREAD);
            paletteLocked = TRUE;
        }
    }

    switch (RwImageGetDepth(image))
    {
        case 4:
            {
                _rwDlImage4GetFromRaster(image, raster);
                break;
            }
        
        case 8:
            {
                _rwDlImage8GetFromRaster(image, raster);
                break;
            }
        
        case 32:
            {
                _rwDlImage32GetFromRaster(image, raster);
                break;
            }
        
        default:
            {
                RWERROR((E_RW_INVIMAGEDEPTH));
                break;
            }
    }

    if (paletteLocked == TRUE)
    {
        RwRasterUnlockPalette(raster);
    }

    if (rasterLocked == TRUE)
    {
        RwRasterUnlock(raster);
    }

    RWRETURN(TRUE);
}

/*****************************************************************************
 rwDlSelectConvertFn

 On entry :
 On exit  :
 */
static DlPixConvertFn
rwDlSelectConvertFn(RwRasterFormat format)
{
    DlPixConvertFn  result = NULL;

    RWFUNCTION(RWSTRING("rwDlSelectConvertFn"));

    switch (format & rwRASTERFORMATPIXELFORMATMASK)
    {
        case rwRASTERFORMATLUM8:
            RWERROR((E_RW_DEVICEERROR, "rwRASTERFORMATLUM8 not yet supported"));
            break;
        
        case rwRASTERFORMAT555:
            result = _rwDlConv8888To555;
            break;
        
        case rwRASTERFORMAT565:
            result = _rwDlConv8888To565;
            break;
        
        case rwRASTERFORMAT1555:
        case rwRASTERFORMAT4444:
            result = _rwDlConv8888To555or3444;
            break;
        
        case rwRASTERFORMAT888:
            result = _rwDlConv8888ToDl888;
            break;
        
        case rwRASTERFORMAT8888:
            result = _rwDlConv8888ToDl8888;
            break;
        
        default:
            RWERROR((E_RW_INVRASTERFORMAT));
            break;
    }

    RWRETURN(result);
}

/*****************************************************************************
 DlPalettizeImage

 On entry   : srcImage - The image to palettize, must be 32Bit in depth.
            : depth - The depth of the palletized image.

 On exit    : An RwImage pointer to a new palletized image else NULL.
 */
static RwImage *
DlPalettizeImage(RwImage *srcImage, RwInt32 depth)
{
    RwPalQuant  palQuant;
    RwImage     *palImage;

    RWFUNCTION(RWSTRING("DlPalettizeImage"));
    RWASSERT(32 == RwImageGetDepth(srcImage));
    RWASSERT((4 == depth) | (8 == depth));

    /* Create the new image */
    palImage = RwImageCreate(RwImageGetWidth(srcImage),
                             RwImageGetHeight(srcImage),
                             depth);
    if (!palImage)
    {
        RWRETURN(NULL);
    }

    /* Allocate the pixels & palette */
    RwImageAllocatePixels(palImage);

    if (!_rwPalQuantInit(&palQuant))
    {
        RWRETURN(NULL);
    }

    /* add all pixels from the image */
    _rwPalQuantAddImage(&palQuant, srcImage, 1.0f);
    _rwPalQuantResolvePalette(RwImageGetPalette(palImage),
        (1UL << depth), &palQuant);

    /* match images */
    _rwPalQuantMatchImage(RwImageGetPixels(palImage),
                         RwImageGetStride(palImage),
                         RwImageGetDepth(palImage),
                         FALSE,
                         &palQuant,
                         srcImage);

    _rwPalQuantTerm(&palQuant);

    RWRETURN(palImage);
}

/*****************************************************************************
 _rwDlRasterPalletized4SetFromImage

 On entry   : Raster
            : Image

            NOTE - CURRENTLY WE DON'T SUPPORT SETTING OF A PARTIAL RASTER
            HERE - IE, IF THE RASTER TO BE SET IS A SUB-RASTER

 On exit    :
 */
static void
_rwDlRasterPalletized4SetFromImage(RwRaster *raster, RwImage *image)
{
    _rwGameCubeRasterExt    *rasExt;
    DlPixConvertFn          convFn;

    RWFUNCTION(RWSTRING("_rwDlRasterPalletized4SetFromImage"));
    RWASSERT(raster);
    RWASSERT(image);

    rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(raster));
    convFn = rwDlSelectConvertFn(RwRasterGetFormat(raster));

    switch (RwImageGetDepth(image))
    {
        case 4:
            {
                RwInt32 y, x;

                for (y = 0; y < raster->height; y++)
                {
                    const RwUInt8   *srcPixel;
                    RwUInt8         *dstPixel;
                
                    srcPixel = (const RwUInt8 *)(image->cpPixels + (image->stride * y));
                    dstPixel = raster->cpPixels + (raster->stride * y);

                    for (x = 0; x < raster->width; x += 2)
                    {
                        *dstPixel = (RwUInt8)(((srcPixel[0] << 4) & 0xF0) |
                                    (srcPixel[1] & 0x0F));
                        
                        srcPixel += 2;
                        dstPixel++;
                    }
                }

                if (!rasExt->unk33)
                {
                    RwInt32     i;
                    RwUInt16    *palette = (RwUInt16 *)raster->palette;

                    for (i = 0; i < 16; i++)
                    {
                        palette[i] = convFn(&image->palette[i]);
                    }
                }

                break;
            }
        
        case 8:
        case 32:
            {
                RwImage     *palImage;

                palImage = DlPalettizeImage(image, RwRasterGetDepth(raster));
                RWASSERT(palImage);

                if (!palImage)
                {
                    RWRETURNVOID();
                }

                _rwDlRasterPalletized4SetFromImage(raster, palImage);

                RwImageDestroy(palImage);

                break;
            }
        
        default:
            RWERROR((E_RW_INVIMAGEDEPTH));
            break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlRasterPalletized8SetFromImage

 On entry   : Raster
            : Image

            NOTE - CURRENTLY WE DON'T SUPPORT SETTING OF A PARTIAL RASTER
            HERE - IE, IF THE RASTER TO BE SET IS A SUB-RASTER

 On exit    :
 */
static void
_rwDlRasterPalletized8SetFromImage(RwRaster *raster, RwImage *image)
{
    DlPixConvertFn          convFn;

    RWFUNCTION(RWSTRING("_rwDlRasterPalletized8SetFromImage"));
    RWASSERT(raster);
    RWASSERT(image);

    convFn = rwDlSelectConvertFn(RwRasterGetFormat(raster));

    switch (RwImageGetDepth(image))
    {
        case 4:
        case 8:
            {
                RwInt32                 y;
                _rwGameCubeRasterExt    *rasExt;

                rasExt = RASTEREXTFROMRASTER(RwRasterGetParent(raster));

                for (y = 0; y < raster->height; y++)
                {
                    RwInt32         x;
                    const RwUInt8   *srcPixel;
                    RwUInt8         *dstPixel;
                
                    srcPixel = (const RwUInt8 *)(image->cpPixels + (image->stride * y));
                    dstPixel = raster->cpPixels + (raster->stride * y);

                    for (x = 0; x < raster->width; x++)
                    {
                        *dstPixel = *srcPixel;
                        
                        srcPixel++;
                        dstPixel++;
                    }
                }

                if (!rasExt->unk33)
                {
                    RwUInt16    *palette = (RwUInt16 *)raster->palette;

                    for (y = 0; y < (1 << image->depth); y++)
                    {
                        palette[y] = convFn(&image->palette[y]);
                    }
                }

                break;
            }
        
        case 32:
            {
                RwImage     *palImage;

                palImage = DlPalettizeImage(image, RwRasterGetDepth(raster));
                RWASSERT(palImage);

                if (!palImage)
                {
                    RWRETURNVOID();
                }

                _rwDlRasterPalletized8SetFromImage(raster, palImage);

                RwImageDestroy(palImage);

                break;
            }
        
        default:
            RWERROR((E_RW_INVIMAGEDEPTH));
            break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlRaster16SetFromImage

 On entry   : Raster
            : Image

            NOTE - CURRENTLY WE DON'T SUPPORT SETTING OF A PARTIAL RASTER
            HERE - IE, IF THE RASTER TO BE SET IS A SUB-RASTER

 On exit    : TRUE on success
 */
static void
_rwDlRaster16SetFromImage(RwRaster *ras, RwImage *image)
{
    DlPixConvertFn  convFn;
    RwInt32         y, x;

    RWFUNCTION(RWSTRING("_rwDlRaster16SetFromImage"));
    RWASSERT(ras);
    RWASSERT(image);

    convFn = rwDlSelectConvertFn(RwRasterGetFormat(ras));

    switch (RwImageGetDepth(image))
    {
        case 4:
        case 8:
            {
                RwUInt16 pal16[256];

                for (x = 0; x < (1 << image->depth); x++)
                {
                    pal16[x] = (RwUInt16)convFn(&image->palette[x]);
                }

                for (y = 0; y < ras->height; y++)
                {
                    const RwUInt8   *srcPixel;
                    RwUInt16        *dstPixel;

                    srcPixel = image->cpPixels + (image->stride * y);
                    dstPixel = ((RwUInt16 *)(((RwUInt8 *)(ras->cpPixels)) +
                                            (ras->stride * y)));

                    for (x = 0; x < ras->width; x++)
                    {
                        *dstPixel = pal16[*srcPixel];

                        srcPixel++;
                        dstPixel++;
                    }
                }

                break;
            }
        
        case 32:
            {
                RwRGBA      *srcPixel;
                RwUInt16    *dstPixel;

                for (y = 0; y < ras->height; y++)
                {
                    srcPixel = (RwRGBA *)(image->cpPixels + image->stride * y);
                    dstPixel = ((RwUInt16 *)(((RwUInt8 *)(ras->cpPixels)) +
                                        (ras->stride * y)));
                    
                    for (x = 0; x < ras->width; x++)
                    {
                        *dstPixel = convFn(srcPixel);

                        dstPixel++;
                        srcPixel++;
                    }
                }

                break;
            }
        
        default:
            RWERROR((E_RW_INVIMAGEDEPTH));
            break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlRaster32SetFromImage

 On entry   : Raster
            : Image

            NOTE - CURRENTLY WE DON'T SUPPORT SETTING OF A PARTIAL RASTER
            HERE - IE, IF THE RASTER TO BE SET IS A SUB-RASTER

 On exit    : TRUE on success
 */
static void
_rwDlRaster32SetFromImage(RwRaster *ras, RwImage *image)
{
    DlPixConvertFn  convFn;
    RwInt32         y, x;

    RWFUNCTION(RWSTRING("_rwDlRaster32SetFromImage"));
    RWASSERT(ras);
    RWASSERT(image);

    convFn = rwDlSelectConvertFn(RwRasterGetFormat(ras));

    switch (RwImageGetDepth(image))
    {
        case 4:
        case 8:
            {
                RwUInt32 pal32[256];

                for (x = 0; x < (1 << image->depth); x++)
                {
                    pal32[x] = (RwUInt32)convFn(&image->palette[x]);
                }

                for (y = 0; y < ras->height; y++)
                {
                    const RwUInt8   *srcPixel;
                    RwUInt32        *dstPixel;

                    srcPixel = image->cpPixels + (image->stride * y);
                    dstPixel = ((RwUInt32 *)(((RwUInt8 *)(ras->cpPixels)) +
                                            (ras->stride * y)));

                    for (x = 0; x < ras->width; x++)
                    {
                        *dstPixel = pal32[*srcPixel];

                        srcPixel++;
                        dstPixel++;
                    }
                }

                break;
            }
        
        case 32:
            {
                RwRGBA      *srcPixel;
                RwUInt32    *dstPixel;

                for (y = 0; y < ras->height; y++)
                {
                    srcPixel = (RwRGBA *)(image->cpPixels + image->stride * y);
                    dstPixel = ((RwUInt32 *)(((RwUInt8 *)(ras->cpPixels)) +
                                        (ras->stride * y)));
                    
                    for (x = 0; x < ras->width; x++)
                    {
                        *dstPixel = convFn(srcPixel);

                        dstPixel++;
                        srcPixel++;
                    }
                }

                break;
            }
        
        default:
            RWERROR((E_RW_INVIMAGEDEPTH));
            break;
    }

    RWRETURNVOID();
}

/*****************************************************************************
 _rwDlRasterSetFromImage

 The raster is only set if it has a valid pixel pointer

 On entry   : Raster (MODIFY)
            : Image
            : Flags
 On exit    : TRUE on success
 */
RwBool
_rwDlRasterSetFromImage(void *rasterIn,
                        void *image,
                        RwInt32 unused3 __RWUNUSED__)
{
    RwImage     *img = (RwImage *)image;
    RwRaster    *raster = (RwRaster *)rasterIn;
    RwInt32     format;
    RwBool      rasterLocked = FALSE;
    RwBool      paletteLocked = FALSE;

    RWFUNCTION(RWSTRING("_rwDlRasterSetFromImage"));

    /* What kind of raster do we have */
    format = RwRasterGetFormat(raster);
    RWASSERT(rwRASTERFORMAT16 != format);
    RWASSERT(rwRASTERFORMAT24 != format);
    RWASSERT(rwRASTERFORMAT32 != format);

    /* Check for an already existing lock */
    if (raster->privateFlags & rwRASTERPIXELLOCKEDWRITE)
    {
        rasterLocked = TRUE;
    }

    /* Lock for write */
    if (!rasterLocked)
    {
        if (!RwRasterLock(raster, 0, rwRASTERLOCKWRITE | rwRASTERLOCKNOFETCH))
        {
            RWRETURN(FALSE);
        }
    }

    if (format & (rwRASTERFORMATPAL4 | rwRASTERFORMATPAL8))
    {
        if (raster->privateFlags & rwRASTERPALETTELOCKEDWRITE)
        {
            paletteLocked = TRUE;
        }

        if (!paletteLocked)
        {
            if (!RwRasterLockPalette(raster, rwRASTERLOCKWRITE | rwRASTERLOCKNOFETCH))
            {
                RWRETURN(FALSE);
            }
        }
    }

    switch (RwRasterGetDepth(raster))
    {
    case 4:
        _rwDlRasterPalletized4SetFromImage(raster, img);
        break;
    
    case 8:
        _rwDlRasterPalletized8SetFromImage(raster, img);
        break;
    
    case 16:
        _rwDlRaster16SetFromImage(raster, img);
        break;
    
    case 24:
        RWERROR((E_RW_INVRASTERDEPTH));
        break;
    
    case 32:
        _rwDlRaster32SetFromImage(raster, img);
        break;
    
    default:
        RWERROR((E_RW_INVRASTERDEPTH));
        break;
    }

    if (format & (rwRASTERFORMATPAL4 | rwRASTERFORMATPAL8))
    {
        if (!paletteLocked)
        {
            RwRasterUnlockPalette(raster);
        }
    }

    if (!rasterLocked)
    {
        RwRasterUnlock(raster);
    }

    RWRETURN(TRUE);
}

/*****************************************************************************
 _rwDlImageFindFormat

 Finds a raster format from analyzing the images pixels

 On entry   : Image - The image to analyze.
            : format - The variable to hold the returned format.
 On exit    :
 */
static RwUInt32
_rwDlImageFindFormat(RwImage *image)
{
    RwInt32     depth;
    RwUInt32    format;
    RwBool      paletteHasAlpha = FALSE;

    RWFUNCTION(RWSTRING("_rwDlImageFindFormat"));

    depth = RwImageGetDepth(image);

    if ((4 == depth) || (8 == depth))
    {
        const RwInt32   width = RwImageGetWidth(image);
        const RwInt32   height = RwImageGetHeight(image);
        RwInt32         y;
        const RwUInt8   *cpIn = image->cpPixels;
        const RwRGBA    *rpPal = image->palette;

        for (y = 0; y < height; y++)
        {
            const RwUInt8   *cpInCur = cpIn;
            RwInt32         x;

            for (x = 0; x < width; x++)
            {
                /* Is there any alpha */
                if (0xFF != rpPal[*cpInCur].alpha)
                {
                    paletteHasAlpha = TRUE;

                    if (0xF < rpPal[*cpInCur].alpha)
                    {
                        format = rwRASTERFORMAT4444;

                        if (4 == depth)
                        {
                            format |= rwRASTERFORMATPAL4;
                        }
                        else
                        {
                            format |= rwRASTERFORMATPAL8;
                        }

                        RWRETURN(format);
                    }
                }

                /* Next pixel */
                cpInCur++;
            }

            cpIn += RwImageGetStride(image);
        }
    }
    else
    {
        const RwInt32   width = RwImageGetWidth(image);
        const RwInt32   height = RwImageGetHeight(image);
        RwInt32         y;
        const RwUInt8   *cpIn = image->cpPixels;

        for (y = 0; y < height; y++)
        {
            const RwRGBA    *rpInCur = (const RwRGBA *)cpIn;
            RwInt32         x;

            for (x = 0; x < width; x++)
            {
                /* Is there any alpha */
                if (0xFF != rpInCur->alpha)
                {
                    paletteHasAlpha = TRUE;

                    if (0xF < rpInCur->alpha)
                    {
                        format = rwRASTERFORMAT4444;
                        RWRETURN(format);
                    }
                }

                /* Next pixel */
                rpInCur++;
            }

            cpIn += RwImageGetStride(image);
        }
    }

    format = (paletteHasAlpha) ? rwRASTERFORMAT1555 : rwRASTERFORMAT565;

    if (depth == 4)
    {
        format |= rwRASTERFORMATPAL4;
    }
    else if (depth == 8)
    {
        format |= rwRASTERFORMATPAL8;
    }

    RWRETURN(format);
}

/*****************************************************************************
 _rwDlImageFindRasterFormat

 On entry   : Raster (OUT)
            : Image
            : Flags
 On exit    : TRUE on success
 */
RwBool
_rwDlImageFindRasterFormat(void *raster, void *image, RwInt32 flags)
{
    RwRaster    *ras = (RwRaster *)raster;
    RwImage     *im = (RwImage *)image;

    RWFUNCTION(RWSTRING("_rwDlImageFindRasterFormat"));

    ras->cType = flags & rwRASTERTYPEMASK;
    ras->depth = 0;

    switch (flags & rwRASTERTYPEMASK)
    {
        case rwRASTERTYPENORMAL:
        case rwRASTERTYPETEXTURE:
        case rwRASTERTYPECAMERATEXTURE:
        {
            RwInt32 format;

            ras->width = (im->width <= 1024) ? im->width : 1024;
            ras->height = (im->height <= 1024) ? im->height : 1024;

            if (flags & rwRASTERFORMATMIPMAP)
            {
                ras->width = 1 << _rwDlFindMSB(ras->width);
                ras->height = 1 << _rwDlFindMSB(ras->height);
            }

            format = _rwDlImageFindFormat(im);

            format |= flags & (rwRASTERFORMATMIPMAP | rwRASTERFORMATAUTOMIPMAP);

            ras->cFormat = (RwUInt8)(format >> 8);

            RWRETURN(TRUE);
        }

        case rwRASTERTYPECAMERA:
        case rwRASTERTYPEZBUFFER:
        {
            ras->cFormat = rwRASTERFORMATDEFAULT;
            ras->width = im->width;
            ras->height = im->height;
            
            RWRETURN(TRUE);
        }

        default:
        {
            RWERROR((E_RW_INVRASTERFORMAT));
        }
    }

    RWRETURN(FALSE);
}