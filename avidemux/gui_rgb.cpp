/***************************************************************************
                          gui_rgb.cpp  -  description
                             -------------------
    begin                : Thu Jan 3 2002
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

 #include <gtk/gtk.h>
 #include "config.h"
#include "avi_vars.h"
#include "prototype.h"
//#include "ADM_gui/GUI_vars.h"

//_______________________________________________
// Quick hack to have display that works..
//_______________________________________________
void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg)
{
    GtkWidget *widget;
    widget = (GtkWidget *)widg; 
    //return;


    gdk_draw_rgb_image(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], 0,	// X
		       0,	// y
		       w,	//width
		       h,	//h*2, // heigth
		       GDK_RGB_DITHER_NONE,
		       //GDK_RGB_DITHER_MAX,  // dithering
		       (guchar *) dis,	// buffer
		       w * 3);


}

// EOF
