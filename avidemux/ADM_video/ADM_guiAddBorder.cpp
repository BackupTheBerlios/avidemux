/***************************************************************************
                          ADM_guiAddBorder.cpp  -  description
                             -------------------
    begin                : Sun Aug 11 2002
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


#include <gtk/gtk.h>
#include <time.h>
#include <sys/time.h>


#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER

#include <ADM_assert.h>
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
//#include "ADM_video/ADM_resizebis.hxx"
#include "ADM_video/ADM_vidAddBorder.h"

extern uint8_t DIA_4entries(char *title,uint32_t *left,uint32_t *right,uint32_t *top,uint32_t *bottom);


uint8_t AVDMVideoAddBorder::configure( AVDMGenericVideoStream *instream)
{
UNUSED_ARG(instream);

CROP_PARAMS *par;

     	par=_param;

	 if(DIA_4entries((char *)"Add black borders",&par->left,&par->right,&par->top,&par->bottom))
	 {
	 		memcpy(&_info,_in->getInfo(),sizeof(_info));
			_info.width+=_param->right+_param->left;
			_info.height+=_param->bottom+_param->top;
	 }
	return 1;

}

#endif
