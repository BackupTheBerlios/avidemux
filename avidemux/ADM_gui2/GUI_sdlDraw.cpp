//
// C++ Implementation: GUI_sdlDraw
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#if defined(USE_SDL)

#include <stdio.h>         
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "ADM_library/default.h"
#ifndef CYG_MANGLING
#include <X11/Xlib.h>
#include <X11/Xutil.h>


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include "ADM_toolkit/toolkit_gtk.h"
#else
#define GtkWidget void
#endif

#include <time.h>
#include <sys/time.h>
#include <ADM_assert.h>
#include "ADM_colorspace/colorspace.h"
#include "ADM_gui2/GUI_render.h"

#include "ADM_gui2/GUI_accelRender.h"
#include "ADM_gui2/GUI_sdlDraw.h"


extern "C" {
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"


}
static uint8_t sdl_running=0;
static SDL_Overlay *sdl_overlay=NULL;
static SDL_Surface *sdl_display=NULL;
static SDL_Rect disp;

#define FORCEYV12
//#define TEST_YU2 1

extern void YV12_422( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h);
sdlAccelRender::sdlAccelRender( void)
{

}
uint8_t sdlAccelRender::end( void)
{
	if(sdl_overlay)
	{
		SDL_FreeYUVOverlay(sdl_overlay);
	}
	if(sdl_display)
	{
		SDL_FreeSurface(sdl_display);
	}
	if(sdl_running)
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
	}
	sdl_running=0;
	sdl_overlay=NULL;
	sdl_display=NULL;
	printf("SDL killed\n");
}
uint8_t sdlAccelRender::init( GtkWidget * window, uint32_t w, uint32_t h)
{
int bpp;
int flags;

	// Ask for the position of the drawing window at start
	//_______________________________________________________
	
	disp.w=w;
	disp.h=h;
	disp.x=0;
	disp.y=0;

	/* Hack to get SDL to use GTK window, ugly but works */
#if !defined(CONFIG_DARWIN) && !defined(CYG_MANGLING)
	{ char SDL_windowhack[32];
		sprintf(SDL_windowhack,"SDL_WINDOWID=%ld",
			GDK_WINDOW_XWINDOW(window->window));
		putenv(SDL_windowhack);
	}
#endif
	if (  SDL_InitSubSystem(SDL_INIT_VIDEO) < 0 ) 
	 {
        	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        	return 0;
    	  }
	sdl_running=1;
	flags = SDL_ANYFORMAT | SDL_HWPALETTE | SDL_HWSURFACE ;//| SDL_NOFRAME;
  	bpp= SDL_VideoModeOK( w, h,  16, flags );
	sdl_display= SDL_SetVideoMode( w, h,  bpp, flags );	
	if(!sdl_display)
	{
		end();
		printf("Cannot create get surface\n");
		return 0;
	
	}
	
	
	//_______________________________________________________
	sdl_overlay=SDL_CreateYUVOverlay((w),(h),
#if (defined(CONFIG_DARWIN)|| defined(TEST_YU2)	) && !defined(FORCEYV12)
		SDL_YUY2_OVERLAY
#else		
		SDL_YV12_OVERLAY
#endif		
		,sdl_display);
	if(!sdl_overlay)
	{
		end();
		printf("Cannot create SDL overlay\n");
		return 0;
	}
	
	printf("SDL_init ok, type is : %d,planes :%d\n",sdl_overlay->hw_overlay,sdl_overlay->planes);
	if(!sdl_overlay->hw_overlay)
	{
		printf("** HW Acceleration disabled **\n");
	#ifdef CONFIG_DARWIN
		printf("** Darwin**\n");
	#endif
	}
	return 1;
}
uint8_t sdlAccelRender::display(uint8_t *ptr, uint32_t w, uint32_t h)
{

	ADM_assert(sdl_overlay);
	SDL_LockYUVOverlay(sdl_overlay);	
	
#if (defined(CONFIG_DARWIN) || defined(TEST_YU2)	)&&!defined(FORCEYV12)
	YV12_422( ptr, sdl_overlay->pixels[0],   w,  h);
#else
	memcpy(sdl_overlay->pixels[0],ptr,w*h);
	memcpy(sdl_overlay->pixels[1],ptr+h*w,(w*h)>>2);
	memcpy(sdl_overlay->pixels[2],ptr+((w*h*5)>>2),(w*h)>>2);
#endif		
	disp.w=w;
	disp.h=h;
	disp.x=0;
	disp.y=0;
	SDL_UnlockYUVOverlay(sdl_overlay);
	SDL_DisplayYUVOverlay(sdl_overlay,&disp);
	//printf("*\n");
	return 1;
}

#endif
