/***************************************************************************
                          gui_xv.cpp  -  description
                             -------------------

	This part is strongly derivated from xine/mplayer/mpeg2dec

    begin                : Tue Jan 1 2002
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
#include <string.h>
#include "config.h"

#ifdef USE_XV
//#define VERBOSE_XV
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <X11/extensions/Xvlib.h>
#include <X11/extensions/XShm.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <errno.h>


#include "avi_vars.h"
#include "ADM_assert.h"

#include "ADM_colorspace/colorspace.h"
#include "ADM_gui2/GUI_render.h"

#include "ADM_gui2/GUI_accelRender.h"
#include "ADM_gui2/GUI_xvDraw.h"

static uint8_t 	GUI_XvList(Display * dis, uint32_t port, uint32_t * fmt);
static uint8_t 	GUI_XvInit(GtkWidget * window, uint32_t w, uint32_t h);
static void 	GUI_XvEnd( void );
static uint8_t 	GUI_XvDisplay(uint8_t * src, uint32_t w, uint32_t h);
static uint8_t 	GUI_XvSync(void);


//________________Wrapper around Xv_______________
XvAccelRender::XvAccelRender( void ) 
{

}
uint8_t XvAccelRender::init( GtkWidget * window, uint32_t w, uint32_t h)
{
	printf("Xv start\n");
	return  GUI_XvInit( window,  w,  h);
}
uint8_t XvAccelRender::end(void)
{
	 GUI_XvEnd( );
	 printf("Xv end\n");
	 return 1;
}

uint8_t XvAccelRender::display(uint8_t *ptr, uint32_t w, uint32_t h)
{
	return GUI_XvDisplay(ptr, w, h);
}
//________________Wrapper around Xv_______________

static unsigned int xv_port;
static uint32_t xv_format;
static Display *xv_display;
static XvImage *xvimage = NULL;
static GC xv_gc;
static XGCValues xv_xgc;
static Window xv_win;
static XShmSegmentInfo Shminfo;
static uint8_t GUI_XvExpose( void );
//
//	Free all ressources allocated by xv
//


void GUI_XvEnd( void )
{
	ADM_assert(xv_port);
 	ADM_assert(xv_display);


  	printf("\n Releasing Xv Port\n");
  	XLockDisplay (xv_display);
	if(XvUngrabPort(xv_display,xv_port,0)!=Success)
 				printf("\n Trouble releasing port...\n");
	XUnlockDisplay (xv_display);


     xvimage=NULL;
     xv_display=NULL;
     xv_port=0;

}

//------------------------------------
uint8_t GUI_XvDisplay(uint8_t * src, uint32_t w, uint32_t h)
{

    if (xvimage)
      {

	  // put image in shared segment

	  // for YV12, 4 bits for Y 4 bits for u, 4 bits for v
	  // total 1.5*
	  memcpy(xvimage->data, src, (w*h*3)>>1);
	  XLockDisplay (xv_display);
	  // And display it !
#if 1
	  XvShmPutImage(xv_display, xv_port, xv_win, xv_gc, xvimage, 0, 0, w, h,	// src
			0, 0, w, h,	// dst
			False);
#else
	 XvPutImage(xv_display, xv_port, xv_win, xv_gc, xvimage, 0, 0, w, h,	// src
			0, 0, w, h	// dst
			);

#endif
	  //XSetForeground (xv_display, xv_gc, 0);

	  XSync(xv_display, False);
	  XUnlockDisplay (xv_display);
	  //GUI_XvExpose();

      }
    return 1;

}
uint8_t GUI_XvSync(void)
{
	if(xv_display)
	  	XSync(xv_display, False);
	return 1;		
}
//------------------------------------
//
//------------------------------------
uint8_t GUI_XvInit(GtkWidget * window, uint32_t w, uint32_t h)
{

    GdkWindow *win;
    unsigned int ver, rel, req, ev, err;
    unsigned int port, adaptors;
    static XvAdaptorInfo *ai;
    static XvAdaptorInfo *curai;
   

    win = gtk_widget_get_parent_window(window);
    xv_display = GDK_WINDOW_XDISPLAY(win);
//      xv_win= RootWindow(xv_display,0);
    xv_win = GDK_WINDOW_XWINDOW(GTK_WIDGET(window)->window);
#define WDN xv_display
    xv_port = 0;

    if (Success != XvQueryExtension(WDN, &ver, &rel, &req, &ev, &err))
      {
	  printf("\n Query Extension failed\n");
	  goto failed;
      }
    /* check for Xvideo support */
    if (Success != XvQueryAdaptors(WDN,
				   DefaultRootWindow(WDN), &adaptors, &ai))
      {
	  printf("\n Query Adaptor failed\n");
	  goto failed;
      }
    curai = ai;
    XvFormat *formats;
    // Dump infos
    port = 0;
    for (uint16_t i = 0; (!port) && (i < adaptors); i++)
      {
/*
XvPortID base_id;
unsigned long num_ports;
char type;
char *name;
unsigned long num_formats;
XvFormat *formats;
unsigned long num_adaptors;
*/
#ifdef VERBOSE_XV
	  printf("\n_______________________________\n");
	  printf("\n Adaptator	: %d", i);
	  printf("\n Base ID		: %ld", curai->base_id);
	  printf("\n Nb Port	 	: %lu", curai->num_ports);
	  printf("\n Type			 	: %d,",
		 curai->type);
#define CHECK(x) if(curai->type & x) printf("|"#x);
	  CHECK(XvInputMask);
	  CHECK(XvOutputMask);
	  CHECK(XvVideoMask);
	  CHECK(XvStillMask);
	  CHECK(XvImageMask);

	  printf("\n Name			 	: %s",
		 curai->name);
	  printf("\n Num Adap	 	: %lu", curai->num_adaptors);
	  printf("\n Num fmt	 	: %lu", curai->num_formats);
#endif	  
	  formats = curai->formats;

	  //
	  uint16_t k;

	  for (k = 0; (k < curai->num_ports) && !port; k++)
	    {
		if (GUI_XvList(WDN, k + curai->base_id, &xv_format))
		    port = k + curai->base_id;
	    }


	  curai++;
      }
    //
    if (!port)
      {
	  printf("\n no port found");
	  goto failed;
      }
#ifdef 	COLORSPACE_YV12
    printf("\n Xv YV12 found at port :%d, format : %ld", port, xv_format);
#else
    printf("\n Xv YUY2 found at port :%d, format : %ld", port, xv_format);
#endif

    if (Success != XvGrabPort(WDN, port, 0))
	goto failed;
    {
	
	xv_port = port;
/*
   Display *display,
   XvPortID port,
   int id,
   char* data,
   int width,
   int height,
   XShmSegmentInfo *shminfo

*/
	xvimage = XvShmCreateImage(WDN, xv_port,
				   xv_format, 0, w, h, &Shminfo);

	Shminfo.shmid = shmget(IPC_PRIVATE, xvimage->data_size,
			       IPC_CREAT | 0777);
	Shminfo.shmaddr = (char *) shmat(Shminfo.shmid, 0, 0);
	Shminfo.readOnly = False;
	xvimage->data = Shminfo.shmaddr;
	XShmAttach(WDN, &Shminfo);
	XSync(WDN, False);
	shmctl(Shminfo.shmid, IPC_RMID, 0);
	memset(xvimage->data, 0, xvimage->data_size);

	xv_xgc.graphics_exposures = False;

	xv_gc = XCreateGC(xv_display, xv_win, 0L, &xv_xgc);
	
	//ADM_assert(BadWindow!=XSelectInput(xv_display, xv_win, ExposureMask | VisibilityChangeMask));

    }
    printf("\n Xv init succeedeed\n");

    return 1;
  failed:
    printf("\n Xv init failed..\n");
    return 0;
}

// _________________________________________________
//
// _________________________________________________
uint8_t GUI_XvList(Display * dis, uint32_t port, uint32_t * fmt)
{
    XvImageFormatValues *formatValues;
    int imgfmt;
    int k, f = 0;

    formatValues = XvListImageFormats(dis, port, &imgfmt);
    if (formatValues)
	for (k = 0; !f || (k < imgfmt); k++)
	  {
#ifdef VERVOSE_XV
	      printf("\n %lx %d --> %s", port, formatValues[k].id,
		     formatValues[k].guid);
#endif
#ifdef 	COLORSPACE_YV12
	      if (!strcmp(formatValues[k].guid, "YV12"))
#else
	      if (!strcmp(formatValues[k].guid, "YUY2"))
#endif
		{
		    f = 1;
		    *fmt = formatValues[k].id;
		}
    } else
	f = 0;
    XFree(formatValues);
    return f;
}
void GUI_XvBuildAtom(Display * dis, Atom * atom, char *string)
{
    UNUSED_ARG(dis);
    UNUSED_ARG(atom);
    UNUSED_ARG(string);
}


uint8_t GUI_XvRedraw( void )
{
	printf("Xv need redraw !\n");

}
#endif
