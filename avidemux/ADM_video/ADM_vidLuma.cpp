/***************************************************************************
                          ADM_vidLuma.cpp  -  description
                             -------------------
    begin                : Sat Aug 24 2002
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
#include "ADM_video/ADM_vidLuma.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM nullParam={0,{""}};


SCRIPT_CREATE(luma_script,ADMVideoLuma,nullParam);
BUILD_CREATE(luma_create,ADMVideoLuma);

 

char *ADMVideoLuma::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," Luma only");
        return buf;
}

ADMVideoLuma::ADMVideoLuma(  AVDMGenericVideoStream *in,CONFcouple *setup)
{

 	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  		
	
		if(setup)
		{
			 _param=(void *)setup;
		}	
			else 			
		{	// default parameter	
				_param= NULL;
		}				
					 	
  _info.encoding=1;

  	  	
}
ADMVideoLuma::~ADMVideoLuma()
{
 	
}
uint8_t ADMVideoLuma::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{

			ADM_assert(frame<_info.nb_frames);
						
								
			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;

		uint32_t sz;
			
					sz=_info.width*_info.height;
					memset(data->data+sz,128,sz>>1);


      return 1;
}



#endif
