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
#include <stdio.h>         
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <assert.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>


#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <time.h>
#include <sys/time.h>


#include "config.h"
#include "avi_vars.h"
#include "prototype.h"
#include "ADM_colorspace/colorspace.h"
#include "ADM_gui2/GUI_render.h"

#include "ADM_gui2/GUI_accelRender.h"
#include "ADM_gui2/GUI_sdlDraw.h"
#include "ADM_toolkit/toolkit_gtk.h"

#if defined(USE_SDL)
extern "C" {
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"


}
static uint8_t sdl_running=0;
static SDL_Overlay *sdl_overlay=NULL;
static SDL_Surface *sdl_display=NULL;
static SDL_Rect disp;
#define TEST_YU2 1

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
		SDL_Quit();
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
GdkWindow *win=NULL;
Window sdl_win;
Display *sdl_windisplay;

	 if ( SDL_Init(SDL_INIT_VIDEO) < 0 ) 
	 {
        	fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
        	return 0;
    	  }
	sdl_running=1;
	flags = SDL_ANYFORMAT | SDL_HWPALETTE | SDL_HWSURFACE | SDL_NOFRAME;
  	bpp= SDL_VideoModeOK( w, h,  16, flags );
	sdl_display= SDL_SetVideoMode( w, h,  bpp, flags );	
	if(!sdl_display)
	{
		end();
		printf("Cannot create get surface\n");
		return 0;
	
	}
	
	// Ask for the position of the drawing window at start
	//_______________________________________________________
	gint x,y;
	win=  gtk_widget_get_parent_window(window);
	gdk_window_get_position         (win,
                                            &x,
                                             &y);
	sdl_win = GDK_WINDOW_XWINDOW(GTK_WIDGET(window)->window);
	sdl_windisplay = GDK_WINDOW_XDISPLAY(win);
	disp.w=w;
	disp.h=h;
	disp.x=1;
	disp.y=1;
	printf("Sdl win :%d %d\n",x,y);
	//_______________________________________________________
	sdl_overlay=SDL_CreateYUVOverlay(w,h,
#if defined(CONFIG_DARWIN) || defined(TEST_YU2)	
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
	
// 	sdl_overlay->planes=3;
// 	sdl_overlay->pitches[0]=w;
// 	sdl_overlay->pitches[1]=w>>1;
// 	sdl_overlay->pitches[2]=w>>1;
	// Put the window on top of the previous one
	SDL_SysWMinfo info;

	SDL_VERSION(&info.version);
	// FIXME : Does not work....
	 //XMoveWindow(info.info.x11.display, info.info.x11.wmwindow,x,y);
	
	printf("SDL_init ok, type is : %d,planes :%d\n",sdl_overlay->hw_overlay,sdl_overlay->planes);
	return 1;
}
uint8_t sdlAccelRender::display(uint8_t *ptr, uint32_t w, uint32_t h)
{

	assert(sdl_overlay);
	SDL_LockYUVOverlay(sdl_overlay);	
	/*sdl_overlay->pixels[0]=ptr;
	sdl_overlay->pixels[1]=ptr+(w*h);
	sdl_overlay->pixels[2]=ptr+((w*h*5)>>2);*/
#if defined(CONFIG_DARWIN) || defined(TEST_YU2)	
	YV12_422( ptr, sdl_overlay->pixels[0],   w,  h);
#else
	memcpy(sdl_overlay->pixels[0],ptr,w*h);
	memcpy(sdl_overlay->pixels[1],ptr+h*w,(w*h)>>2);
	memcpy(sdl_overlay->pixels[2],ptr+((w*h*5)>>2),(w*h)>>2);
#endif		
	SDL_UnlockYUVOverlay(sdl_overlay);
	SDL_DisplayYUVOverlay(sdl_overlay,&disp);
	//printf("*\n");
	return 1;
}

#endif
