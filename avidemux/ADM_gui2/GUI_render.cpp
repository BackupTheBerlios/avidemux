/***************************************************************************
                          gui_render.cpp  -  description

	The final render a frame. The external interface is the same
	whatever the mean (RGB/YUV/Xv)
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

#include <stdio.h>         
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "avi_vars.h"
#include "prototype.h"
#include "ADM_colorspace/colorspace.h"
//#include "ADM_gui/GUI_vars.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_gui2/GUI_xvDraw.h"
#include "ADM_toolkit/toolkit_gtk.h"

 
static uint8_t	updateWindowSize(GtkWidget * win, uint32_t w, uint32_t h);


uint8_t  GUI_InitRender (GtkWidget *g, uint32_t w,uint32_t h);


extern void GUI_XvEnd( void );
extern void yuv420_argb32_mmx (uint8_t * image, uint8_t * py,
			       uint8_t * pu, uint8_t * pv,
			       int h_size, int v_size);

extern GtkWidget *getDrawWidget( void );

//_______________________________________

static uint8_t 		xv_mode=0;
static uint8_t 		*screenBuffer=NULL;
static uint8_t		 *lastImage=NULL;
static GtkWidget 	*draw=NULL;
static uint32_t 		renderW=0,renderH=0;
//static uint8_t 		_XvAvailable = 0;

//_______________________________________
/**
	Render init, initialize internals. Constuctor like function

*/
uint8_t renderInit( void )
{
	draw=getDrawWidget(  );
	return 1;
}
/**
	Warn the renderer that the display size is changing

*/
//----------------------------------------
uint8_t renderResize(uint32_t w, uint32_t h)
{
	if(screenBuffer) 
		{
			delete [] screenBuffer;
			screenBuffer=NULL;
		}
	assert(screenBuffer=new uint8_t [(w*h*3)]);		// RGB -> *3
	updateWindowSize( draw,w,h);
	UI_purge();
	return 1;

}
/**
	Update the image and render it
	The width and hiehgt must NOT have changed

*/
//----------------------------------------
uint8_t renderUpdateImage(uint8_t *ptr)
{
	lastImage=ptr;
	if(!xv_mode)
	 	GUI_ConvertRGB(ptr,screenBuffer, renderW,renderH);
	renderRefresh();


	return 1;

}
/**
	Refresh the image from internal buffer / last image
	Used for example as call back for X11 events

*/
//_______________________________________________
uint8_t renderRefresh(void)
{
	if(!screenBuffer)
	{
		if(xv_mode) assert(0);
		return 0;
	}

#if defined(USE_XV)
    if ( xv_mode)
    {
	GUI_XvDisplay(lastImage, renderW, renderH);
		//GUI_XvRedraw();
   }
    else
#endif
	GUI_RGBDisplay(screenBuffer, renderW, renderH,draw);
    return 1;
}

uint8_t renderExpose(void)
{
	if(!screenBuffer)
	{
		if(xv_mode) assert(0);
		return 0;
	}

#if defined(USE_XV)
    if ( xv_mode)
    {
	
		GUI_XvRedraw();
   }
    else
#endif
	GUI_RGBDisplay(screenBuffer, renderW, renderH,draw);
    return 1;

}
uint8_t renderStartPlaying( void )
{

	#if defined(USE_XV)

	xv_mode=GUI_XvInit(draw, renderW, renderH);
	if(xv_mode)
	{
		//GUI_XvSync();
		printf("\n Using xv\n");
	}

	#else
	xv_mode=0;
	#endif

	return 1;
}


uint8_t renderStopPlaying( void )
{
	#if defined(USE_XV)

	if(xv_mode)
	{
		GUI_XvEnd();
		GUI_XvInit(draw, 16, 16);
		GUI_XvEnd();

		printf("Xv port released\n");
	}

	#else
	#endif
	xv_mode=0;
	return 1;
}


//___________________________________________________________________________
//
//
uint8_t	updateWindowSize(GtkWidget * win, uint32_t w, uint32_t h)
{
    assert(screenBuffer);
    renderW = w;
    renderH = h;

    gtk_widget_set_usize(win, w, h);
	UI_purge();
    return 1;
}

uint8_t GUI_ConvertRGB(uint8_t * in, uint8_t * out, uint32_t w, uint32_t h)
{
#ifdef COLORSPACE_YV12

	        COL_yv12rgb (w, h,in, out);

#else
    		yuv2rgb(in, out, w, h);
#endif
    return 1;
}



