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
#include <string.h>


#include <sys/time.h>


#include <../ADM_assert.h>

#include "default.h"


#include "ADM_commonUI/GUI_render.h"

#include "ADM_commonUI/GUI_accelRender.h"
#ifdef USE_XV
#include "ADM_commonUI/GUI_xvRender.h"
#endif
#ifdef USE_SDL
#include "ADM_commonUI/GUI_sdlRender.h"
#endif


#include "../prefs.h"
#include "../../ADM_colorspace/ADM_rgb.h"


extern uint8_t BitBlit(uint8_t *dst, uint32_t pitchDest,uint8_t *src,uint32_t pitchSrc,uint32_t width, uint32_t height);
extern void UI_purge(void);


static uint8_t	updateWindowSize(void * win, uint32_t w, uint32_t h);
void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);
static uint8_t  GUI_ConvertRGB(uint8_t * in, uint8_t * out, uint32_t w, uint32_t h);
static uint8_t  GUI_ConvertRGBBlit(uint8_t * in, uint8_t * out,uint32_t totalW,uint32_t totalH, 
                                  uint32_t startx,uint32_t starty,uint32_t w, uint32_t h,uint32_t primary);


extern uint8_t UI_shrink(uint32_t w,uint32_t h);
//_____________________________________


extern ColYuvRgb rgbConverter;
extern ColYuvRgb rgbConverter2;

//_____________________________________
static AccelRender    *accel_mode=NULL;
static uint8_t        *accelSurface=NULL;
//_______________________________________

static uint8_t 	            *screenBuffer=NULL;
static uint8_t		    *lastImage=NULL;
static void                 *draw=NULL;
static uint32_t 	     renderW=0,renderH=0;
static uint32_t              phyW=0,phyH=0; /* Unzoomed size, only used when accel can do hw resize */

//_______________________________________
/**
	Render init, initialize internals. Constuctor like function

*/
uint8_t renderInit( void )
{
	draw=UI_getDrawWidget(  );
	return 1;
}

/**
	Warn the renderer that the display size is changing

*/
//----------------------------------------
uint8_t renderResize(uint32_t w, uint32_t h,uint32_t pw, uint32_t ph)
{
int mul,xx,yy;

	if(screenBuffer) 
		{
			delete  [] screenBuffer;
			screenBuffer=NULL;
		}
        screenBuffer=new uint8_t[w*h*4];

	updateWindowSize( draw,w,h);
        phyW=pw;
        phyH=ph;
	UI_purge();
	return 1;

}
/**
	Update the image and render it
	The width and hiehgt must NOT have changed

*/
//----------------------------------------
uint8_t renderUpdateImage(uint8_t *ptr,renderZoom zoom)
{

    ADM_assert(screenBuffer);
    lastImage=ptr;

    if(accel_mode)
    {
        if(accel_mode ->hasHwZoom())
        {
           accel_mode->display(lastImage, phyW, phyH,zoom);
        }else
        {
          accel_mode->display(lastImage, renderW, renderH,zoom);
        }
      
    }else
    {
      GUI_ConvertRGB(ptr,screenBuffer, renderW,renderH);
      renderRefresh();
    }
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
          if(lastImage)
              accel_mode->display(lastImage, renderW, renderH);
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
        if(lastImage)
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
uint8_t r=0;
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
        GUI_WindowInfo xinfo;
        UI_getWindowInfo(draw, &xinfo);
        switch(render)
        {
        
#if defined(USE_XV)
	       case RENDER_XV:
		accel_mode=new XvAccelRender();
                if(accel_mode->hasHwZoom()) r=accel_mode->init(&xinfo,phyW,phyH);
                else r=accel_mode->init(&xinfo,renderW,renderH);
                if(!r)
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
		if(accel_mode->hasHwZoom()) r=accel_mode->init(&xinfo,phyW,phyH);
                else r=accel_mode->init(&xinfo,renderW,renderH);
                if(!r)
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
        else
        {
           ADM_assert(!accelSurface);
           accelSurface=new uint8_t[ (renderW*renderH*3)>>1];
          
        }
	
	return 1;
}
/**
      \fn renderHasAccelZoom
      \brief returns 1 if accel can do hw zoom
*/
uint8_t renderHasAccelZoom(void)
{
    if(!accel_mode) return 0;
    return accel_mode->hasHwZoom(); 
}

uint8_t renderStopPlaying( void )
{
	if(accel_mode)
	{
		accel_mode->end();
		delete accel_mode;
                if(accelSurface) delete [] accelSurface;
                accelSurface=NULL;
				
	}
	accel_mode=NULL;	
	return 1;
}


//___________________________________________________________________________
//
//
uint8_t	updateWindowSize(void * win, uint32_t w, uint32_t h)
{
    ADM_assert(screenBuffer);
    renderW = w;
    renderH = h;

    UI_updateDrawWindowSize(win,w,h);
    //gtk_widget_set_usize(win, w, h);
    rgbConverter.reset(w,h);
    return 1;
}

uint8_t GUI_ConvertRGB(uint8_t * in, uint8_t * out, uint32_t w, uint32_t h)
{
    rgbConverter.reset(w,h);
    rgbConverter.scale(in,out);
    return 1;
}
uint8_t GUI_ConvertRGBBlit(uint8_t * in, uint8_t * out,uint32_t totalW,uint32_t totalH, uint32_t startx,uint32_t starty,uint32_t w, uint32_t h,uint32_t primary)
{
  if(!primary)
  {
    rgbConverter.reset(w,h);
    rgbConverter.scale(in,out,startx,starty,w,h,renderW,renderH);
  }else
  {
    rgbConverter2.reset(w,h);
    rgbConverter2.scale(in,out,startx,starty,w,h,renderW,renderH);
  }
    return 1;
}



void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg)
{
    
    UI_rgbDraw(widg,w,h,dis);

}
