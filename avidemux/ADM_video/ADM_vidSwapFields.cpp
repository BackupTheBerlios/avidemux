/***************************************************************************
                          Swap Fields.cpp  -  description
                             -------------------
Swap each line  (shift up for odd, down for even)


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
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFieldUtil.h"
#include "ADM_video/ADM_vidSwapFields.h"
//static void decimate(uint8_t *src,uint8_t *target, uint32_t linessrc, uint32_t width);

BUILD_CREATE(swapfield_create,AVDMVideoSwapField);
BUILD_CREATE(keepeven_create,AVDMVideoKeepEven);
BUILD_CREATE(keepodd_create,AVDMVideoKeepOdd);


char *AVDMVideoSwapField::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Swap fields");
        return buf;
}
char *AVDMVideoKeepEven::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Keep Even Fields");
        return buf;
}char *AVDMVideoKeepOdd::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Keep Odd Fields");
        return buf;
}
//_______________________________________________________________
AVDMVideoSwapField::AVDMVideoSwapField(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	_uncompressed=new uint8_t[3*_info.width*_info.height];


}
//_______________________________________________________________
AVDMVideoKeepOdd::AVDMVideoKeepOdd(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	_uncompressed=new uint8_t[3*_info.width*_info.height];
	_info.height>>=1;

}
// ___ destructor_____________
AVDMVideoSwapField::~AVDMVideoSwapField()
{
 	delete [] _uncompressed;

}
// ___ destructor_____________
AVDMVideoKeepOdd::~AVDMVideoKeepOdd()
{
 	delete [] _uncompressed;

}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoSwapField::getFrameNumberNoAlloc(uint32_t frame,
																		uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
//static Image in,out;
			if(frame>=_info.nb_frames) return 0;


			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

		uint32_t w=_info.width;
		uint32_t h=_info.height;
		uint32_t page=w*h;
		uint32_t stride;

		memcpy(data+page,_uncompressed+page,page>>1); // copy u & v

		uint8_t *odd,*even,*target,*target2;

		even=_uncompressed;
		odd=even+w;
		target=data;
		target2=data+w;
		stride=2*w;

		h>>=1;
		for(;h--;h>0)
		{
			memcpy(target,odd,w);
			memcpy(target2,even,w);
			target+=stride;
			target2+=stride;
			odd+=stride;
			even+=stride;
		}

      return 1;
}

uint8_t AVDMVideoKeepOdd::getFrameNumberNoAlloc(uint32_t frame,
																		uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
//static Image in,out;
			if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;


		uint32_t w=_info.width;
		uint32_t h=_info.height;

		vidFieldKeepOdd(  w,  h, _uncompressed,data);


      return 1;
}

uint8_t AVDMVideoKeepEven::getFrameNumberNoAlloc(uint32_t frame,
																		uint32_t *len,
   																	uint8_t *data,
   																	uint32_t *flags)
{
//static Image in,out;
			if(frame>=_info.nb_frames) return 0;


			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

		uint32_t w=_info.width;
		uint32_t h=_info.height;



		vidFieldKeepEven(  w,  h, _uncompressed,data);
      return 1;
}

#endif
