/***************************************************************************
                          ADM_guiDenoise.cpp  -  description
                             -------------------
    begin                : Tue Nov 26 2002
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
  /*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <ADM_assert.h>
#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidDenoise.h"

extern uint8_t DIA_dnr(uint32_t *llock,uint32_t *lthresh, uint32_t *clock,
			uint32_t *cthresh, uint32_t *scene);


uint8_t ADMVideoDenoise::configure(AVDMGenericVideoStream * instream)
{
  UNUSED_ARG(instream);

    return DIA_dnr(	&(_param->lumaThreshold),
    				&(_param->lumaLock),
				&(_param->chromaThreshold),
				&(_param->chromaLock),
				&(_param->sceneChange)
               						);


}
 #endif
