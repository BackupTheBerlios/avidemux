/***************************************************************************
                          DIA_flyThreshold.cpp  -  configuration dialog for
						   threshold filter
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

#include "config.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "default.h"
#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_videoFilter/ADM_vidThreshold.h"
#include "DIA_flyDialog.h"
#include "DIA_flyThreshold.h"

/************* COMMON PART *********************/

uint8_t  flyThreshold::update ()
{
    download();
    process();
    copyYuvFinalToRgb();
    display();
    return 1;
}

uint8_t flyThreshold::process ()
{
    uint8_t lookup_table [256];
    if (ADMVideoThreshold::computeLookupTable (&param, lookup_table))
        upload();

    ADMVideoThreshold::doThreshold (_yuvBuffer, _yuvBufferOut,
                                    lookup_table, (_h * _w * 3) >> 1);
    return 1;
}

/************* COMMON PART *********************/
