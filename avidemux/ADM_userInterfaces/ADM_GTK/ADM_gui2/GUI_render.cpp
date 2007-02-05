/***************************************************************************
    Hook to Gfx toolkit concerning display (RGB or accel)
                             -------------------
    
    copyright            : (C) 2007 by mean
    email                : Mean/fixounet@free.fr
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
#include <math.h>
#include <unistd.h>

#include <gtk/gtk.h>
#ifndef CYG_MANGLING
#include <gdk/gdkx.h>
#endif
#include <time.h>
#include <sys/time.h>


#include "../avi_vars.h"
#include <../ADM_assert.h>



#include "ADM_commonUI/GUI_render.h"

#include "ADM_commonUI/GUI_accelRender.h"
#ifdef USE_XV
#include "ADM_commonUI/GUI_xvRender.h"
#endif
#ifdef USE_SDL
#include "ADM_commonUI/GUI_sdlRender.h"
#endif

#include "ADM_toolkit_gtk/toolkit_gtk.h"

#include "../prefs.h"
#include "../../../ADM_colorspace/ADM_rgb.h"
#include "../../../ADM_libraries/ADM_libswscale/ADM_mp.h"
#include "../gtkgui.h"


extern GtkWidget *getDrawWidget( void );

/**
    \brief return pointer to the drawing widget that displays video
*/
void *UI_getDrawWidget(void)
{
  return (void *) getDrawWidget();
}
/**
    \brief Display to widget in RGB32
*/
void UI_rgbDraw(void *widg,uint32_t w, uint32_t h,uint8_t *ptr)
{
    GtkWidget *widget;
    widget = (GtkWidget *)widg; 


    gdk_draw_rgb_32_image(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], 0,    // X
                       0,       // y
                       w,       //width
                       h,       //h*2, // heigth
                       GDK_RGB_DITHER_NONE,
                       //GDK_RGB_DITHER_MAX,  // dithering
                       (guchar *) ptr,  // buffer
                       w * 4);
}
/**
      \brief Resize the window
*/
void  UI_updateDrawWindowSize(void *win,uint32_t w,uint32_t h)
{
    gtk_widget_set_usize((GtkWidget *)win, w, h);
}
/**
      \brief Retrieve info from window, needed for accel layer
*/
void UI_getWindowInfo(void *draw, GUI_WindowInfo *xinfo)
{
        GdkWindow *win;
        GtkWidget *widget=(GtkWidget *)draw;
          
        win = gtk_widget_get_parent_window(widget);
#ifndef CYG_MANGLING
        xinfo->window=	GDK_WINDOW_XWINDOW(widget->window);
        xinfo->display= GDK_WINDOW_XDISPLAY(win);
#endif
}
#if 0
#endif
// EOF
