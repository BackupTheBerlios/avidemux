/***************************************************************************
                          ADM_guiCrop.cpp  -  description
                             -------------------
    begin                : Sun Mar 24 2002
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
#include <string.h>
#include <assert.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"

#include "ADM_colorspace/colorspace.h"


extern void GUI_RGBDisplay(uint8_t * dis, uint32_t w, uint32_t h, void *widg);

static void 				gui_ko(GtkButton * button, gpointer user_data);
static void 				gui_ok(GtkButton * button, gpointer user_data);
extern  int 				DIA_getCropParams(	uint32_t *w,uint32_t 	*w2, uint32_t *h,uint32_t *h2,uint32_t tw,
											uint32_t th,uint8_t *in);
static GtkWidget  *create_dialog4 (void);
static gboolean  			gui_draw(GtkWidget * widget,
			     GdkEventExpose * event, gpointer user_data);
static void gui_update(GtkButton * button, gpointer user_data);

static int croplock;

static   GtkWidget *entry_x, *entryy,*entry_x2,*entry_y2;
static  GtkWidget *drawingarea9=NULL;;

// Ugly !
static uint32_t globalw=0,globalh=0;
static uint8_t *globalvideo,*globalsource;

static void gui_autocrop(GtkButton * button, gpointer user_data);

uint8_t AVDMVideoStreamCrop::configure( AVDMGenericVideoStream *instream)

{
CROP_PARAMS *par;
uint32_t w,h,l,f;
uint8_t ret=0;
uint8_t *video1,*video2;

							video1=video2=NULL;

		                 // Get info from previous filter
							w=_in->getInfo()->width;
							h= _in->getInfo()->height;

							printf("\n Crop in : %lu  x %lu\n",w,h);

							video2=(uint8_t *)malloc(w*h*3);							
							assert(video2);
							video1=(uint8_t *)malloc(w*h*3);							
							assert(video1);

							// ask current frame from previous filter
							assert(instream->getFrameNumberNoAlloc(curframe, &l,
          																				video2,&f));
						
							// From now we work in RGB !
                         COL_yv12rgb(w,h,video2,video1);

							
							
			par=_param;
		
    	 	switch(DIA_getCropParams(&par->cropx,&par->cropx2,&par->cropy,&par->cropy2,
     							w,h,(uint8_t *)video1 ))
				{
				case 0:
		      		printf("cancelled\n");
					break;
				case 1:
					_info.width=_in->getInfo()->width-_param->cropx-_param->cropx2;
					_info.height=_in->getInfo()->height-_param->cropy-_param->cropy2;
					ret=1;
					break;
				default:
					assert(0);
				}

					free(video1);
					free(video2);
					video1=video2=NULL;

					return ret;
}

#endif
