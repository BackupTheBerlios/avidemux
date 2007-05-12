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

#include <config.h>

#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>


#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
# include <math.h>

#include "default.h"

#include "ADM_toolkit_gtk/ADM_gladeSupport.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_toolkit/toolkit.hxx"
#include "default.h"

#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "DIA_flyDialog.h"
#include "ADM_assert.h"
extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);
uint8_t  ADM_flyDialog::display(void)
{
  ADM_assert(_canvas);
  ADM_assert(_rgbBufferOut);
  GUI_RGBDisplay(_rgbBufferOut,_w,_h,_canvas);
  return 1; 
}
uint32_t ADM_flyDialog::sliderGet(void)
{
  ADM_assert(_slider);
  GtkAdjustment *adj=gtk_range_get_adjustment (GTK_RANGE(_slider));
  return (uint32_t)GTK_ADJUSTMENT(adj)->value;
}
uint8_t     ADM_flyDialog::sliderSet(uint32_t value)
{
  ADM_assert(_slider);
  return 1; 
}


//EOF
