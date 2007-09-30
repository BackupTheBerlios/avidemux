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

#include "default.h"

#include <gtk/gtk.h>
#ifdef ADM_WIN32
#define WIN32_CLASH
#include <gdk/gdkwin32.h>
#else
#include <gdk/gdkx.h>
#endif

#include "../ADM_assert.h"
#include "../ADM_osSupport/ADM_misc.h"

#include "ADM_commonUI/GUI_render.h"
#include "ADM_commonUI/GUI_accelRender.h"

#include "ADM_toolkit_gtk/toolkit_gtk.h"

#include "../../../ADM_colorspace/ADM_rgb.h"
#include "../../../ADM_libraries/ADM_libswscale/ADM_mp.h"

void GUI_gtk_grow_off(int onff);

extern GtkWidget *getDrawWidget(void);
extern uint8_t UI_getPhysicalScreenSize(uint32_t *w, uint32_t *h);

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
// This makes a lot of noise if you use the Crop filter...  perhaps it should
// be output only once?  Or maybe there is a real bug - I'm not sure.  [Chris MacGregor]
//      printf("[GTK] Warning window bigger than display %u x %u vs %u x %u\n",w,h,lastW,lastH);
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

// Calculate the zoom ratio required to fit the whole image on the screen.
float UI_calcZoomToFitScreen(GtkWindow* window, GtkWidget* drawingArea, uint32_t imageWidth, uint32_t imageHeight)
{
	int windowWidth, windowHeight;
	int drawingWidth, drawingHeight;
	uint32_t screenWidth, screenHeight;
	
	gtk_window_get_size(window, &windowWidth, &windowHeight);
	gtk_widget_get_size_request(drawingArea, &drawingWidth, &drawingHeight);

	UI_getPhysicalScreenSize(&screenWidth, &screenHeight);

	// Take drawing area out of the equation, how much extra do we need for additional controls?
	windowWidth -= drawingWidth;
	windowHeight -= drawingHeight;

	// Take borders and captions into consideration (GTK doesn't seem to support this so we'll have to guess)
	windowWidth += 10;
	windowHeight += 40;

	// This is the true amount of screen real estate we can work with
	screenWidth -= windowWidth;
	screenHeight -= windowHeight;

	// Calculate zoom ratio
	if (imageWidth > screenWidth || imageHeight > screenHeight)
	{
		if ((int)(imageWidth - screenWidth) > (int)(imageHeight - screenHeight))
			return (float)screenWidth / (float)imageWidth;
		else
			return (float)screenHeight / (float)imageHeight;
	}
	else
		return 1;
}

// GTK doesn't centre the window correctly.  Use this function to centre windows with a canvas that is yet to resized.
void UI_centreCanvasWindow(GtkWindow *window, GtkWidget *canvas, int newCanvasWidth, int newCanvasHeight)
{
	int winWidth, winHeight, widgetWidth, widgetHeight;
	uint32_t resWidth, resHeight;

	UI_getPhysicalScreenSize(&resWidth, &resHeight);
	gtk_widget_get_size_request((GtkWidget*)canvas, &widgetWidth, &widgetHeight);
	gtk_window_get_size(window, &winWidth, &winHeight);

	winWidth = newCanvasWidth;
	winHeight = (winHeight - widgetHeight) + newCanvasHeight;

	// Take borders and captions into consideration (GTK doesn't seem to support this so we'll have to guess)
	winWidth += 10;
	winHeight += 40;

	gtk_window_move(window, ((int)resWidth - winWidth) / 2, ((int)resHeight - winHeight) / 2);
}
// EOF
