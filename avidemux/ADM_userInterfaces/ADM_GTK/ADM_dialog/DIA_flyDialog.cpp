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

#include <gtk/gtk.h>

#include "default.h"
#include "ADM_toolkit_gtk/toolkit_gtk.h"
#include "ADM_toolkit_gtk/toolkit_gtk_include.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "DIA_flyDialog.h"
#include "ADM_assert.h"

extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);

void ADM_flyDialog::postInit(uint32_t width, uint32_t height, AVDMGenericVideoStream *in,
							 void *canvas, void *slider, int yuv)
{
	if (slider != NULL)
	{
		GtkAdjustment *adj = (GtkAdjustment*)gtk_adjustment_new(0, 0, in->getInfo()->nb_frames - 1, 0, 0, 0);

		gtk_range_set_adjustment(GTK_RANGE(_slider), adj);
		gtk_scale_set_digits(GTK_SCALE(_slider), 0);
	}
}

uint8_t  ADM_flyDialog::display(void)
{
	ADM_assert(_canvas);
	ADM_assert(_rgbBufferOut);

	if (_resizer)
	{
		_resizer->resize(_rgbBufferOut, _rgbBufferDisplay);
		GUI_RGBDisplay(_rgbBufferDisplay, _zoomW, _zoomH, _canvas);
	}
	else
		GUI_RGBDisplay(_rgbBufferOut, _w, _h, _canvas);

	return 1; 
}

uint32_t ADM_flyDialog::sliderGet(void)
{
	ADM_assert(_slider);
	GtkAdjustment *adj=gtk_range_get_adjustment (GTK_RANGE(_slider));
	return (uint32_t)GTK_ADJUSTMENT(adj)->value;
}

uint8_t ADM_flyDialog::sliderSet(uint32_t value)
{
	ADM_assert(_slider);

	GtkAdjustment *adj = gtk_range_get_adjustment(GTK_RANGE(_slider));

	adj->value = value;

	gtk_range_set_adjustment(GTK_RANGE(_slider), adj);

	return 1; 
}

uint8_t ADM_flyDialog::isRgbInverted(void)
{
	return 0; 
}

//EOF
