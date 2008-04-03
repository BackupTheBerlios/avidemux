/***************************************************************************
                          ADM_guiResize.cpp  -  description
                             -------------------

GUI part of resize.
			     
    begin                : Thu Mar 21 2002
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
#include "ADM_default.h"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_resizebis.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"
#include "DIA_coreToolkit.h"

static int getResizeParams(uint32_t * w, uint32_t * h, uint32_t * algo,uint32_t ow,uint32_t oh,uint32_t fps);


extern uint8_t DIA_resize(uint32_t *width,uint32_t *height,uint32_t *algo,uint32_t originalw, uint32_t originalh,uint32_t fps1000);

uint8_t AVDMVideoStreamResize::configure(AVDMGenericVideoStream * instream)
{
    UNUSED_ARG(instream);

    RESIZE_PARAMS *par;
//    uint8_t ret=0;

    _init = 0;			// force recompute
    par = _param;



    if (!getResizeParams(&par->w, &par->h, &par->algo,instream->getInfo()->width,instream->getInfo()->height,_info.fps1000))
      {
	  return 0;
      }
      printf("\n OK was selected \n");
    // update other parameters
    _info.width = _param->w;
    _info.height = _param->h;
    // intermediate buffer
    if (_intermediate_buffer)
      {
	  delete  [] _intermediate_buffer;
	  _intermediate_buffer = NULL;
      }
    //_intermediate_buffer=(uint8_t *)malloc(3*_info.width*_in->getInfo()->height);
    _intermediate_buffer =
	new uint8_t[3 * _info.width * _in->getInfo()->height];

    return 1;

}

//
//  
//
//
//   static GtkWidget *resi;
int getResizeParams(uint32_t * w, uint32_t * h, uint32_t * algo,uint32_t ow,uint32_t oh,uint32_t fps)
{
uint32_t ww,hh;
	while(1)
	{
		ww=*w;
		hh=*h;
  		if(!DIA_resize(&ww,&hh,algo,ow,oh,fps)) return 0;
                if(!ww || !hh) GUI_Error_HIG(QT_TR_NOOP("Width and height cannot be 0"), NULL);
		else
                  if( ww&1 || hh &1) GUI_Error_HIG(QT_TR_NOOP("Width and height cannot be odd"), NULL);
			else
			{
				*w=ww;
				*h=hh;
				return 1;
			}
	}
}



