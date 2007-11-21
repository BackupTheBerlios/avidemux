/***************************************************************************
    copyright            : (C) 2006 by mean
    email                : fixounet@free.fr
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************///

#include "config.h"

#include <string.h>
#include <stdio.h>

#include <math.h>

#include "default.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "DIA_flyDialog.h"
#include "ADM_assert.h"

uint8_t  ADM_flyDialog::display(void)
{
  return 1; 
}
uint32_t ADM_flyDialog::sliderGet(void)
{
  return 1; 
}
uint8_t     ADM_flyDialog::sliderSet(uint32_t value)
{
  return 1; 
}


//EOF
