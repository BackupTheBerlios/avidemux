/***************************************************************************
                          colorspace.h  -  description
                           -------------------
  Select here the colorspace used by application



    begin                : Fri Feb 22 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#define COLORSPACE_YV12
//#define  COLORSPACE_YUY2

#if !defined(COLORSPACE_YV12) && !defined(COLORSPACE_YUY2)
#error Define at least one colorspace !
#endif
uint8_t  COL_yv12rgbBMP(uint32_t ww, uint32_t hh, uint8_t* in, uint8_t *out);
uint8_t  COL_yv12rgb(uint32_t w, uint32_t h,uint8_t * ptr, uint8_t * out );

uint8_t COL_422_YV12( uint8_t *in[3], uint32_t stride[3],  uint8_t *out,uint32_t w, uint32_t h);
uint8_t COL_411_YV12( uint8_t *in[3], uint32_t stride[3],  uint8_t *out,uint32_t w, uint32_t h);

uint8_t COL_RgbToYuv(uint8_t R,uint8_t G,uint8_t B, uint8_t *y,int8_t *u,int8_t  *v);
uint8_t COL_YuvToRgb( uint8_t y,int8_t u,int8_t v,uint8_t *r,uint8_t *g,uint8_t *b);


