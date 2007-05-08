/***************************************************************************
                          ADM_vidFlipV.cpp  -  description
                             -------------------
    begin                : Wed Nov 6 2002
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

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_vidFlipV.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM flipParam={0,{""}};


SCRIPT_CREATE(flipv_script,ADMVideoFlipV,flipParam);
BUILD_CREATE(flipv_create,ADMVideoFlipV);
 

char *ADMVideoFlipV::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," V-Flip");
        return buf;
}

ADMVideoFlipV::ADMVideoFlipV(  AVDMGenericVideoStream *in,CONFcouple *setup)
{
    UNUSED_ARG(setup);
 	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info)); 	
  	_info.encoding=1;	
	_uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
	ADM_assert(_uncompressed);    	  	
}
ADMVideoFlipV::~ADMVideoFlipV()
{
 	delete  _uncompressed;	
	_uncompressed=NULL;
  
}
uint8_t ADMVideoFlipV::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{

	if(frame>= _info.nb_frames) return 0;
	// read uncompressed frame
	if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

         uint8_t *in,*out;
         uint32_t stride=_info.width;
         uint32_t h=_info.height;
         uint32_t page,qpage;
        

	  
         page=stride*h;
         qpage=page>>2;
         
         in=YPLANE(_uncompressed);
         out=YPLANE(data)+(h-1)*stride;
         // flip y
         for(uint32_t y=h;y>0;y--)
         {
		 memcpy(out,in,stride);
		 in+=stride;
		 out-=stride;
	}
	// Flip U & V			         
        stride>>=1;
	in=UPLANE(_uncompressed);	
        out=UPLANE(data)+qpage-stride;
         // flip u
         for(uint32_t y=h>>1;y>0;y--)
         {
		 memcpy(out,in,stride);
		 in+=stride;
		 out-=stride;
	}
	in=VPLANE(_uncompressed);
        out=VPLANE(data)+qpage-stride;
       
      
         // flip u
         for(uint32_t y=h>>1;y>0;y--)
         {
		 memcpy(out,in,stride);
		 in+=stride;
		 out-=stride;
	}   
	data->copyInfo(_uncompressed);
	return 1;
}



#endif
