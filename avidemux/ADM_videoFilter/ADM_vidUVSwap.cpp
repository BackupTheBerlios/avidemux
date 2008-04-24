/***************************************************************************
                          ADM_vidUVSwap.cpp  -  description
                             -------------------
    begin                : Tue Sep 10 2002
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

#include "ADM_videoFilter.h"
#include "ADM_vidUVSwap.h"


static FILTER_PARAM nullParam={0,{""}};


SCRIPT_CREATE(swapuv_script,ADMVideoUVSwap,nullParam);
BUILD_CREATE(swapuv_create,ADMVideoUVSwap);

char *ADMVideoUVSwap::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," UV Swap");
        return buf;
}

ADMVideoUVSwap::ADMVideoUVSwap(  AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
	_buf=NULL;
 	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));

		
  _info.encoding=1;
 _buf=new uint8_t [(_info.width*_info.height)>>2];


}
ADMVideoUVSwap::~ADMVideoUVSwap()
{
		if(_buf)
		{
            		delete []_buf;
			_buf=NULL;
		}
}
uint8_t ADMVideoUVSwap::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{

		if(frame>= _info.nb_frames) return 0;
		// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;

		uint32_t sz;
		uint8_t *start;
					
			sz=_info.width*_info.height;
			sz>>=2;
				
			start=UPLANE(data);
			memcpy(_buf,start,sz);
					
			memcpy(UPLANE(data),VPLANE(data),sz);
			memcpy(VPLANE(data),_buf,sz);
			


      return 1;
}

