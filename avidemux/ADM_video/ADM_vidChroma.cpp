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
#include <ADM_assert.h>

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
	
  	  	
}
ADMVideoChromaU::~ADMVideoChromaU()
{
 	
	
 	
}

//
//	Remove y and v just keep U and expand it
//
   uint8_t ADMVideoChromaU::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{				
uint32_t w,x;
uint32_t page;
		ADM_assert(frame<_info.nb_frames);
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;
		
		page= _info.width*_info.height;
		*len=(page*3)>>1;


		// now expand  u
		uint8_t *y,*v,*y2;

		y=YPLANE(data);
		y2=y+_info.width;
		v=UPLANE(data);
		for(w= _info.height>>1;w>0;w--)
		{
			for(x= _info.width>>1;x>0;x--)
			{
				*y=*v;
				*y2=*v;
				*(y+1)=*v;
				*(y2+1)=*v;
				v++;
				y+=2;
				y2+=2;
			}
                	y+=_info.width;
			y2+=_info.width;
       		 }

		 // Remove chroma u & v
		 memset(UPLANE(data),0x80,page>>2);
		 memset(VPLANE(data),0x80,page>>2);
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
 

}
ADMVideoChromaV::~ADMVideoChromaV()
{

	

}

//
//	Remove y and v just keep U and expand it
//
   uint8_t ADMVideoChromaV::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint32_t w,x;
uint32_t page;
		ADM_assert(frame<_info.nb_frames);
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;
		
		page= _info.width*_info.height;
		*len=(page*3)>>1;


		// now expand  u
		uint8_t *y,*v,*y2;

		y=YPLANE(data);
		y2=y+_info.width;
		v=VPLANE(data);
		for(w= _info.height>>1;w>0;w--)
		{
			for(x= _info.width>>1;x>0;x--)
			{
				*y=*v;
				*y2=*v;
				*(y+1)=*v;
				*(y2+1)=*v;
				v++;
				y+=2;
				y2+=2;
			}
                	y+=_info.width;
			y2+=_info.width;
       		 }

		 // Remove chroma u & v
		 memset(UPLANE(data),0x80,page>>2);
		 memset(VPLANE(data),0x80,page>>2);
}

