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


extern uint8_t DIA_4entries(char *title,uint32_t *left,uint32_t *right,uint32_t *top,uint32_t *bottom);

uint8_t AVDMVideoStreamBSMear::configure( AVDMGenericVideoStream *instream)
{
UNUSED_ARG(instream);

CROP_PARAMS *par;

     	par=_param;

	return ( DIA_4entries((char *)"Blacken Border",&par->cropx,&par->cropx2,&par->cropy,&par->cropy2));

}
#endif
