/***************************************************************************
                          ADM_vidPalShift.cpp  -  description
                             -------------------
    begin                : Sat Aug 24 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
 ***************************************************************************/
#if 0
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
#include "ADM_video/ADM_vidPalShift.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM nullParam={0,{""}};


SCRIPT_CREATE(addPALShift_script,ADMVideoPalShift,nullParam);
BUILD_CREATE(addPALShift_create,ADMVideoPalShift);


char *ADMVideoPalShift::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," PAL field shift");
        return buf;
}

ADMVideoPalShift::ADMVideoPalShift(  AVDMGenericVideoStream *in,CONFcouple *setup)
{
	UNUSED_ARG(setup);
	

	_reverse=NULL;
 	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  		
	

				_reverse=new uint8_t;;
				*_reverse=1;
						
					
 	//_uncompressed=(uint8_t *)malloc(3*_in->getInfo()->width*_in->getInfo()->height);
 	//_uncompressed=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];
  ADM_assert(_uncompressed);

 	_cache=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];
  ADM_assert(_cache);

  _info.encoding=1;
	_cacheno=0xffffffff;


  	  	
}
ADMVideoPalShift::~ADMVideoPalShift()
{
 	delete []_uncompressed;

 	
}
uint8_t ADMVideoPalShift::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{

      	uint32_t full,half;

				full=_info.width*_info.height;
				half=full>>4;

			ADM_assert(frame<_info.nb_frames);
						
								
			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;
				if(frame==0)
						{
                      		memcpy(data,_uncompressed,full+(full>>1));
								memcpy(_cache,_uncompressed,full+(full>>1));
								_cacheno=0;
								return 1;
						}

				// Is the frame in cache the good one ?
				if(_cacheno!=(frame-1))
					{  // get it !
					       		if(!_in->getFrameNumberNoAlloc(frame-1, len,_cache,flags)) return 0;
									_cacheno=frame-1;
					}					
       		
				// copy u & v as they are
			

				memcpy(data+full,_uncompressed+full,full>>1);

				// now copy odd field from framei and even frame from frame i-1
				uint8_t *out;
				uint8_t *cur;
				uint8_t *prev;
				uint32_t dline=_info.width;

			if(!*_reverse)
			{
				prev=_cache+_info.width;
				cur=_uncompressed;
				out=data;
	
				for(uint32_t y=(_info.height>>1);y>0;y--)
					{
                   	memcpy(out,cur, dline);
						out+=dline;
                   	memcpy(out,prev, dline);
						out+=dline;
						cur+=dline;
						cur+=dline;
						prev+=dline;
						prev+=dline;
					}
				}
				else
				{
				prev=_cache;
				cur=_uncompressed+_info.width;
				out=data;
	
				for(uint32_t y=(_info.height>>1);y>0;y--)
					{
						memcpy(out,prev, dline);
						out+=dline;
                   	memcpy(out,cur, dline);
						out+=dline;
                   	
						cur+=dline;
						cur+=dline;
						prev+=dline;
						prev+=dline;
					}
				}
                   // fill cache
						_cacheno=frame;
						memcpy(  _cache,_uncompressed, full);




      return 1;
}



#endif
#endif