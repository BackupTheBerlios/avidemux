/***************************************************************************
                          ADM_rgb16.cpp  -  description
                             -------------------
    begin                : Mon May 27 2002
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>



#include "ADM_utilities/default.h"

#include <ADM_assert.h>
#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_rgb16.h"


decoderRGB16::decoderRGB16 (uint32_t w, uint32_t h,uint32_t rgb):decoders (w, h)
{
  isRgb=rgb;
}
decoderRGB16::~decoderRGB16 ()
{

}


uint8_t
  decoderRGB16::uncompress (uint8_t * in, ADMImage * out,
			    uint32_t len, uint32_t * flag)
{
  int xx;
  xx = _w * _h;

  if (flag)
    *flag = AVI_KEY_FRAME;

  // We dont do much here ...

  if (len == (3 * xx))  // rgb 24 ?
    {
      if(isRgb)
        out->_colorspace =(ADM_colorspace)( ADM_COLOR_RGB24 | ADM_COLOR_BACKWARD);
      else
        out->_colorspace = (ADM_colorspace)( ADM_COLOR_BGR24| ADM_COLOR_BACKWARD);
      out->_planeStride[0] = 3 * _w;
    }
  else if (len == (4 * xx))
    {
      if(isRgb)
        out->_colorspace = (ADM_colorspace)( ADM_COLOR_RGB32A | ADM_COLOR_BACKWARD );
      else
        out->_colorspace = (ADM_colorspace)( ADM_COLOR_BGR32A| ADM_COLOR_BACKWARD);
      out->_planeStride[0] = 4 * _w;
    }
/*        else if(len==2*xx)
                        color->changeColorSpace(ADM_COLOR_RGB16);*/
  else
    return 0;

  ADM_assert (out->_isRef);
  out->_planes[0] = in;
  out->_planes[1] = NULL;
  out->_planes[2] = NULL;
  out->_planeStride[1] = 0;
  out->_planeStride[2] = 0;
  return 1;
}
//EOF
