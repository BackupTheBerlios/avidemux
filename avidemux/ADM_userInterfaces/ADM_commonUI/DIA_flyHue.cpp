/**/
/***************************************************************************
                          DIA_hue
                             -------------------

			   Ui for hue & sat

    begin                : 08 Apr 2005
    copyright            : (C) 2004/7 by mean
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



#include <config.h>


#include <string.h>
#include <stdio.h>

#include "default.h"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidHue.h"
#include "DIA_flyDialog.h"
#include "DIA_flyHue.h"

#include "ADM_assert.h"
// FIXME
#ifndef M_PI
#define M_PI    3.14159265358979323846
#endif

/************* COMMON PART *********************/
uint8_t  flyHue::update(void)
{
    download();
    process();
    _rgb->scale(_yuvBufferOut->data,_rgbBufferOut);
    display();
    return 1;
}        

uint8_t  flyHue::process(void )
{
uint8_t *src,*dst;
uint32_t stride;
float hue,sat;
    hue=param.hue*M_PI/180.;
    sat=(100+param.saturation)/100.;
    memcpy(YPLANE(_yuvBufferOut),YPLANE(_yuvBuffer),_w*_h); // copy luma
    // Do it!
    HueProcess_C(VPLANE(_yuvBufferOut), UPLANE(_yuvBufferOut),
        VPLANE(_yuvBuffer), UPLANE(_yuvBuffer),
        _w>>1,_w>>1,
        _w>>1,_h>>1, 
        hue, sat);
    // Copy half source to display
    dst=_yuvBufferOut->data+_w*_h;
    src=_yuvBuffer->data+_w*_h;
    stride=_w>>1;
    for(uint32_t y=0;y<_h;y++)   // We do both u & v!
    {
        memcpy(dst,src,stride>>1);
        dst+=stride;
        src+=stride;
    }
    return 1;
}
//EOF
