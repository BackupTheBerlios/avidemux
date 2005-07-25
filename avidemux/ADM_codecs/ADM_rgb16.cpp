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



#include "ADM_library/default.h"

#include <ADM_assert.h>
#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_rgb16.h"


decoderRGB16::decoderRGB16(uint32_t w, uint32_t h):decoders(w, h)
{
    planar = new uint8_t[_w * _h * 2];
    color=new ColRgbToYV12(w,h,ADM_COLOR_RGB24);
    color->reset(w,h);
    color->setBmpMode();
}
decoderRGB16::~decoderRGB16()
{
    delete[]planar;
    delete color;
}

static inline void SwapMe(uint8_t *tgt,uint8_t *src,int nb);
void SwapMe(uint8_t *tgt,uint8_t *src,int nb)
{
    uint8_t r,g,b;
   nb=nb;
   while(nb--)
   {
       r=*src++;
       g=*src++;
       b=*src++;
       *tgt++=b;
       *tgt++=g;
       *tgt++=r;
       
   }
   return;
    
}

uint8_t decoderRGB16::uncompress(uint8_t * in, ADMImage * out,
				 uint32_t len, uint32_t * flag)
{
    uint32_t xx,mul;
    
        xx=_w*_h;

        if (flag)
                *flag = AVI_KEY_FRAME;

    
    if (len == (3 * xx))	// rgb 24 ?
        color->changeColorSpace(ADM_COLOR_RGB24);
    else if(len==(4*xx))
                color->changeColorSpace(ADM_COLOR_RGB32A);
        else if(len==2*xx)
                        color->changeColorSpace(ADM_COLOR_RGB16);
                                else return 0;

    mul=len/xx; // Byte/pixel in rgb colorspace

    if(! color->scale(in,out->data)) return 0;

        return 1;
}
//EOF
