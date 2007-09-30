/***************************************************************************
                          DIA_flyParticle.cpp  -  configuration dialog for
						   particle filter
                              -------------------
                         Chris MacGregor, September 2007
                         chris-avidemux@bouncingdog.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "default.h"

#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_videoFilter/ADM_vidParticle.h"
#include "DIA_flyDialog.h"
#include "DIA_flyParticle.h"

/************* COMMON PART *********************/

uint8_t flyParticle::update ()
{
    download();
    process();
    copyYuvFinalToRgb();
    display();
    return 1;
}

static inline void draw_hline (uint8_t * fb, uint32_t width, uint32_t height,
                               uint32_t y)
{
    for (uint8_t * pixp = fb + width * y; width--; pixp++)
        *pixp ^= 0xff;
}

static inline void draw_vline (uint8_t * fb, uint32_t width, uint32_t height,
                               uint32_t x)
{
    for (uint8_t * pixp = fb + x; height--; pixp += width)
        *pixp ^= 0xff;
}

uint8_t flyParticle::process ()
{
    uint8_t ret = ADMVideoParticle::doParticle (_yuvBuffer, _yuvBufferOut,
                                                _in, 0, 0, &param, _w, _h);
    input_buffer_valid = false;

    uint8_t * fb = YPLANE(_yuvBufferOut);

    if (param.left_crop)
        draw_vline (fb, _w, _h, param.left_crop - 1);
    if (param.right_crop)
        draw_vline (fb, _w, _h, _w - param.right_crop);
    if (param.top_crop)
        draw_hline (fb, _w, _h, param.top_crop - 1);
    if (param.bottom_crop)
        draw_hline (fb, _w, _h, _h - param.bottom_crop);

    return ret;
}

/************* COMMON PART *********************/
