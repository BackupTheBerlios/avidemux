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
#include "config.h"

#include <stdio.h>         
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>


#include "avi_vars.h"
#include <ADM_assert.h>

#include "prototype.h"
//#include "ADM_colorspace/colorspace.h"
//#include "ADM_gui/GUI_vars.h"


#include "ADM_gui2/GUI_render.h"

#include "ADM_gui2/GUI_accelRender.h"
#include "ADM_gui2/GUI_xvDraw.h"
#include "ADM_gui2/GUI_sdlDraw.h"

#include "ADM_toolkit/toolkit_gtk.h"

#include "prefs.h"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_libswscale/ADM_mp.h"
#include "gtkgui.h"


static uint8_t	updateWindowSize(GtkWidget * win, uint32_t w, uint32_t h);


uint8_t  GUI_InitRender (GtkWidget *g, uint32_t w,uint32_t h);
static renderZoom zoom=ZOOM_1_1;

static ColYuvRgb rgbConverter(640,480);

extern GtkWidget *getDrawWidget( void );
extern uint8_t UI_shrink(uint32_t w,uint32_t h);
static AccelRender *accel_mode=NULL;
//_______________________________________

static uint8_t 	        *screenBuffer=NULL;
static ADMImage                 *resized=NULL;
static ADM_MplayerResize         *resizer=NULL;
static uint8_t		 *lastImage=NULL;
static GtkWidget 	*draw=NULL;
static uint32_t 	renderW=0,renderH=0;

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
uint8_t renderResize(uint32_t w, uint32_t h,renderZoom newzoom)
{
int mul,xx,yy;

	if(screenBuffer) 
		{
			delete  [] screenBuffer;
			screenBuffer=NULL;
		}
        if(resized)
        {
                        delete resized;
                        resized=NULL;
        }
        if(resizer)
        {
                delete resizer;
                resizer=NULL;
        }
        zoom=newzoom;
        switch(zoom)
        {
                case ZOOM_1_4: mul=1;break;
                case ZOOM_1_2: mul=2;break;
                case ZOOM_1_1: mul=4;break;
                case ZOOM_2:   mul=8;break;
                case ZOOM_4:   mul=16;break;
                default : ADM_assert(0);

        }
        xx=(w*mul+3)/4;
        yy=(h*mul+3)/4;

        if(xx&1) xx++;
        if(yy&1) yy++;

        screenBuffer=new uint8_t[xx*yy*4];

        if(zoom!=ZOOM_1_1)
        {
                 resizer=new ADM_MplayerResize(w,h,xx,yy);
                 resized=new ADMImage(xx,yy);
        }

	updateWindowSize( draw,xx,yy);
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
	
        ADM_assert(screenBuffer);
        if(zoom!=ZOOM_1_1)
        {
                ADM_assert(resizer);
                ADM_assert(resized);
                resizer->resize(ptr,resized->data);
                lastImage=resized->data;
                ptr=lastImage;
        }
        else
        {
                lastImage=ptr;
        }
	if(!accel_mode)
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
		if(accel_mode) ADM_assert(0);
		return 0;
	}

	if(accel_mode)
	{
		accel_mode->display(lastImage, renderW, renderH);
		//GUI_XvRedraw();
   	}
    	else
		GUI_RGBDisplay(screenBuffer, renderW, renderH,draw);
    return 1;
}

uint8_t renderExpose(void)
{
	if(!screenBuffer)
	{
		if(accel_mode) ADM_assert(0);
		return 0;
	}


    if ( accel_mode)
    {	
		accel_mode->display(lastImage,renderW,renderH);
    }
    else
	GUI_RGBDisplay(screenBuffer, renderW, renderH,draw);
    return 1;

}
uint8_t renderStartPlaying( void )
{
char *displ;
unsigned int renderI;
ADM_RENDER_TYPE render;
	ADM_assert(!accel_mode);
	// First check if local
	// We do it in a very wrong way : If DISPLAY!=:0.0 we assume remote display
	// in that case we do not even try to use accel
	
	// Win32 does not have display
#ifndef CYG_MANGLING	
	displ=getenv("DISPLAY");
	if(!displ)
	{
		return 0;
	}
	if(strcmp(displ,":0") && strcmp(displ,":0.0"))
	{
		printf("Looks like remote display, no Xv :%s\n",displ);
		return 1;
	}
#endif	
 
        if(prefs->get(DEVICE_VIDEODEVICE,&renderI)!=RC_OK)
        {       
                render=RENDER_GTK;
        }else
        {
                render=(ADM_RENDER_TYPE)renderI;
        }
        switch(render)
        {
        
#if defined(USE_XV)
	       case RENDER_XV:
		accel_mode=new XvAccelRender();
		if(!accel_mode->init(draw,renderW,renderH))
		{
			delete accel_mode;
			accel_mode=NULL;
			printf("Xv init failed\n");
		}
		else
		{
			printf("Xv init ok\n");
		}
                break;
#endif
#if defined(USE_SDL)
              case RENDER_SDL:
		printf("Trying SDL\n");
		accel_mode=new sdlAccelRender();
		if(!accel_mode->init(draw,renderW,renderH))
		{
			delete accel_mode;
			accel_mode=NULL;
			printf("sdl init failed\n");
		}
		else
		{
			printf("SDL init ok\n");
		}
                break;
#endif
                default:break;
        }
        if(!accel_mode)
        {
                rgbConverter.reset(renderW,renderH);
                printf("No accel used for rendering\n");
        }
	
	return 1;
}


uint8_t renderStopPlaying( void )
{
	if(accel_mode)
	{
		accel_mode->end();
		delete accel_mode;				
	}
	accel_mode=NULL;	
	return 1;
}


//___________________________________________________________________________
//
//
uint8_t	updateWindowSize(GtkWidget * win, uint32_t w, uint32_t h)
{
    ADM_assert(screenBuffer);
    renderW = w;
    renderH = h;

    // Shrink the main window
    // so that we do not leave blanks everywhere
    
    //***
  
     /*UI_purge();
     UI_shrink(w,h);
     UI_purge();
     */
    gtk_widget_set_usize(win, w, h);
    return 1;
}

uint8_t GUI_ConvertRGB(uint8_t * in, uint8_t * out, uint32_t w, uint32_t h)
{
    //COL_yv12rgb (w, h,in, out);
    rgbConverter.reset(w,h);
    rgbConverter.scale(in,out);
    return 1;
}

AccelRender::AccelRender( void)
{
}

void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg)
{
    GtkWidget *widget;
    widget = (GtkWidget *)widg; 
    //return;


    gdk_draw_rgb_32_image(widget->window, widget->style->fg_gc[GTK_STATE_NORMAL], 0,    // X
                       0,       // y
                       w,       //width
                       h,       //h*2, // heigth
                       GDK_RGB_DITHER_NONE,
                       //GDK_RGB_DITHER_MAX,  // dithering
                       (guchar *) dis,  // buffer
                       w * 4);


}
