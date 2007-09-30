/***************************************************************************
                          DIA_flySwissArmyKnife.cpp  -  configuration dialog for
						   Swiss Army Knife filter
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
#include "ADM_videoFilter/ADM_vidSwissArmyKnife.h"
#include "DIA_flyDialog.h"
#include "DIA_flySwissArmyKnife.h"

/************* COMMON PART *********************/

uint8_t flySwissArmyKnife::sliderChanged ()
{
    if (!param.enable_preview)
        return 0;

    return ADM_flyDialog::sliderChanged();
}

uint8_t  flySwissArmyKnife::update ()
{
    download();
    if (!param.enable_preview)
        return 0;

    process();
    copyYuvFinalToRgb();
    display();
    return 1;
}

uint8_t flySwissArmyKnife::process ()
{
    if (!param.enable_preview)
        return 0;

    uint8_t ret =
        ADMVideoSwissArmyKnife::doSwissArmyKnife (_yuvBuffer, _yuvBufferOut,
                                                  _in, sakp, &param, _w, _h);
    return ret;
}

/************* COMMON PART *********************/
