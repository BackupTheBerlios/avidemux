/***************************************************************************
                          ADM_guiBSMear.cpp  -  description
                             -------------------
 			Actually it is the same as crop
 			Blacken borders to remove for example, vhs parasites at bottom

    begin                : Fri Mar 29 2002
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
#include <ADM_assert.h>

#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>

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

extern int DIA_getCropParams(char *n,uint32_t *w,uint32_t *w2,uint32_t *h,uint32_t *h2,uint32_t tw,uint32_t th,uint8_t *in);

uint8_t AVDMVideoStreamBSMear::configure( AVDMGenericVideoStream *instream)
{
UNUSED_ARG(instream);

CROP_PARAMS *par;
uint32_t w,h,l,f;
uint8_t ret=0;
ADMImage *video1;

	// Get info from previous filter
	w=_in->getInfo()->width;
	h= _in->getInfo()->height;
	printf("\n AddBlackBorder in : %u  x %u\n",w,h);

	//video1=(uint8_t *)malloc(w*h*4);
	video1=new ADMImage(w,h);
	
	ADM_assert(video1);	
	// ask current frame from previous filter
	ADM_assert(instream->getFrameNumberNoAlloc(curframe, &l, video1,&f));
	

     	par=_param;

	switch(DIA_getCropParams("Border Smear",&par->left,&par->right,&par->top,&par->bottom,
			w,h,video1->data )){
		case 0:  printf("cancelled\n");
		         break;
		case 1:  _info.width=_in->getInfo()->width-_param->left-_param->right;
		         _info.height=_in->getInfo()->height-_param->top-_param->bottom;
		         ret=1;
		         break;
		default: ADM_assert(0);
	}

	delete video1;	
	return ret;

}

#endif
