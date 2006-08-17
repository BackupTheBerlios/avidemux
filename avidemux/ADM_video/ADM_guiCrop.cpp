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
#include "config.h"
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <time.h>
#include <sys/time.h>

#include <ADM_assert.h>

#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"

#include "ADM_colorspace/colorspace.h"
extern int DIA_getCropParams(	char *name,CROP_PARAMS *param,AVDMGenericVideoStream *in);
uint8_t AVDMVideoStreamCrop::configure( AVDMGenericVideoStream *instream)

{
		uint8_t r;
		uint32_t w,h;
    	if(r = (DIA_getCropParams("Crop Settings",_param,instream )))
    	{
			w=_param->left+_param->right;
			h=_param->top+_param->bottom;
			ADM_assert(w<instream->getInfo()->width);
			ADM_assert(h<instream->getInfo()->height);
			_info.width=instream->getInfo()->width-w;
			_info.height=instream->getInfo()->height-h;
		}
		return r;
}
#endif
