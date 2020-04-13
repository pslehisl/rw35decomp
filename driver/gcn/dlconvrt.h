/***************************************************************************
 *                                                                         *
 * Module  : dlconvrt.h                                                    *
 *                                                                         *
 * Purpose : Image <-> raster conversions                                  *
 *                                                                         *
 **************************************************************************/

#ifndef DLCONVRT_H
#define DLCONVRT_H

/****************************************************************************
 Includes
 */

/****************************************************************************
 Defines
 */

 /****************************************************************************
 Global Types
 */

/****************************************************************************
 Globals
 */

/****************************************************************************
 Function prototypes
 */

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

/* Finding the MSB */
extern RwInt32 _rwDlFindMSB(RwInt32 num);

/* Pixel conversions */
extern RwBool _rwDlRGBToPixel(void *pixel, void *col, RwInt32 format);
extern RwBool _rwDlPixelToRGB(void *rgb, void *pixel, RwInt32 format);

/* Raster <--> Image conversions */
extern RwBool _rwDlImageGetFromRaster(void *imageIn, void *rasterIn, RwInt32 num);
extern RwBool _rwDlRasterSetFromImage(void *rasterIn, void *image, RwInt32 flags);

/* Finding a suitable format for a raster based on an image */
extern RwBool _rwDlImageFindRasterFormat(void *raster, void *image, RwInt32 flags);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif /* DLCONVRT_H */
