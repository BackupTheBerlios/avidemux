/***************************************************************************
                          ADM_rgb16.h  -  description
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
#ifndef ADM_RGB16_H
#define ADM_RGB16_H

#include "ADM_colorspace/ADM_rgb.h"
 class decoderRGB16 : decoders
{
     protected:
                                                uint8_t         *planar;
                                                ColRgbToYV12    *color;

     public:
                                                decoderRGB16(uint32_t w,uint32_t h);
                        virtual	                ~decoderRGB16();
                        virtual uint8_t         uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flag=NULL);	
                        
}   ;

#endif
