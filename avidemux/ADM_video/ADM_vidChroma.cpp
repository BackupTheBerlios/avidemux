/***************************************************************************
                          ADM_vidChroma.cpp  -  description
                             -------------------
    begin                : Wed Aug 28 2002
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
#include <assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidChroma.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM nullParam={0,{""}};


SCRIPT_CREATE(chromaU_script,ADMVideoChromaU,nullParam);
SCRIPT_CREATE(chromaV_script,ADMVideoChromaV,nullParam);

BUILD_CREATE(chromaU_create,ADMVideoChromaU);
BUILD_CREATE(chromaV_create,ADMVideoChromaV);


char *ADMVideoChromaU::printConf( void )
{
 	static char buf[50];
 	
 	sprintf((char *)buf," chroma u only");
        return buf;
}

//_______________________________________________________________

ADMVideoChromaU::ADMVideoChromaU(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
    UNUSED_ARG(setup);

  	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  			 	
  _info.encoding=1;
   	_uncompressed=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];
  assert(_uncompressed);
  	  	
}
ADMVideoChromaU::~ADMVideoChromaU()
{
 	
	delete [] _uncompressed;
 	
}

//
//	Remove y and v just keep U and expand it
//
   uint8_t ADMVideoChromaU::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags)
{
   uint32_t x,w;
			assert(frame<_info.nb_frames);
								
			
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);       			

				// remove y & v
               memset(data+_info.width*_info.height,128,(_info.width*_info.height)>>2);

				// now expand  u
				uint8_t *y,*v;

				y=data;
				v=_uncompressed+  _info.width*_info.height;

				for(w= _info.height>>1;w>0;w--)
				{
				for(x= _info.width>>1;x>0;x--)
						{
                      	*y=*v;
			               *(y+_info.width)=*v;
							 y++;		
                      	*y=*v;
			               *(y+_info.width)=*v++;
							 y++;		

								
						}
                	y+=_info.width;
       		 }        		       		
      return 1;
}

//---______________________________________________---------v--------------
//---______________________________________________---------v--------------
//---______________________________________________---------v--------------
//---______________________________________________---------v--------------
//---______________________________________________---------v--------------
//---______________________________________________---------v--------------
//---______________________________________________---------v--------------
//---______________________________________________---------v--------------




char *ADMVideoChromaV::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," chroma v only");
        return buf;
}

//_______________________________________________________________

ADMVideoChromaV::ADMVideoChromaV(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
    UNUSED_ARG(setup);

  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
  _info.encoding=1;
   	_uncompressed=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];
  assert(_uncompressed);

}
ADMVideoChromaV::~ADMVideoChromaV()
{

	delete [] _uncompressed;

}

//
//	Remove y and v just keep U and expand it
//
   uint8_t ADMVideoChromaV::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags)
{
   uint32_t x,w;
			assert(frame<_info.nb_frames);


       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1);

				// remove y & v
               memset(data+_info.width*_info.height,128,(_info.width*_info.height)>>2);

				// now expand  u
				uint8_t *y,*v;

				y=data;
				v=_uncompressed+  _info.width*_info.height+(_info.width*_info.height>>2);

				for(w= _info.height>>1;w>0;w--)
				{
				for(x= _info.width>>1;x>0;x--)
						{
                      	*y=*v;
			               *(y+_info.width)=*v;
							 y++;
                      	*y=*v;
			               *(y+_info.width)=*v++;
							 y++;


						}
                	y+=_info.width;
       		 }
      return 1;
}

