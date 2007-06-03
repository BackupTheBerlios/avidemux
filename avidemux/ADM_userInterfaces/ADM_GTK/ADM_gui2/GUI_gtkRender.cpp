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
#ifndef ADM_WIN32
#include <gdk/gdkx.h>
#else
#include <gdk/gdkwin32.h>
#endif
#include <time.h>
#include <sys/time.h>


//#include "../avi_vars.h"
#include <../ADM_assert.h>
#define WIN32_CLASH
#include "default.h"
#include "../ADM_osSupport/ADM_misc.h"

#include "ADM_commonUI/GUI_render.h"

#include "ADM_commonUI/GUI_accelRender.h"

#include "ADM_toolkit_gtk/toolkit_gtk.h"

#include "../prefs.h"
#include "../../../ADM_colorspace/ADM_rgb.h"
#include "../../../ADM_libraries/ADM_libswscale/ADM_mp.h"
#include "../gtkgui.h"

void GUI_gtk_grow_off(int onff);

extern GtkWidget *getDrawWidget( void );
 ColYuvRgb rgbConverter(640,480);
uint32_t lastW,lastH;
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

    if(lastW>w || lastH>h)
    {
      printf("[Gtk] Warning window bigger than display %u x %u vs %u x %u\n",w,h,lastW,lastH);
    }

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
    GUI_gtk_grow_off(0);
    gtk_widget_set_usize((GtkWidget *)win, w, h);
    UI_purge();
    GUI_gtk_grow_off(1);
    lastW=w;
    lastH=h;
    printf("[GTK] Changing size to %u %u\n",w,h);
    UI_purge();
}
/**
      \brief Retrieve info from window, needed for accel layer
*/
void UI_getWindowInfo(void *draw, GUI_WindowInfo *xinfo)
{
        GdkWindow *win;
        GtkWidget *widget=(GtkWidget *)draw;
          
        win = gtk_widget_get_parent_window(widget);
#ifndef ADM_WIN32
        xinfo->window=	GDK_WINDOW_XWINDOW(widget->window);
        xinfo->display= GDK_WINDOW_XDISPLAY(win);
#else
        xinfo->display=	(void*)GDK_WINDOW_HWND(widget->window);
#endif
}
#if 0
#endif
// EOF
