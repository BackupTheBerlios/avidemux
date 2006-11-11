/***************************************************************************
                          ADM_vidChromaShift.cpp  -  description
                             -------------------
	Try to remove the blue-to-theleft & red to the right effect by shifting chroma

    begin                : Sun Aug 14 2003
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
#include "ADM_video/ADM_vidChromaShift.h"

#include "ADM_filter/video_filters.h"


static FILTER_PARAM cshiftParam={2,{"u","v"}};


SCRIPT_CREATE(chromashift_script,ADMVideoChromaShift,cshiftParam);
BUILD_CREATE(create_chromashift,ADMVideoChromaShift);

char *ADMVideoChromaShift::printConf( void )
{
 	static char buf[50];

 	snprintf((char *)buf,50," Chroma shift U:%d  V:%d",
            _param->u,_param->v);
        return buf;
}

ADMVideoChromaShift::ADMVideoChromaShift(  AVDMGenericVideoStream *in,CONFcouple *couples)
{

 	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  		
	 _param=NEW(CHROMASHIFT_PARAM);
		if(couples)
		{
				GET(u);
				GET(v);
		}
			else
		{	// default parameter
				_param->u=0;
				_param->v=0;
		}

	_uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
	ADM_assert(_uncompressed);
	_info.encoding=1;
}


uint8_t	ADMVideoChromaShift::getCoupledConf( CONFcouple **couples)
{

		ADM_assert(_param);
		*couples=new CONFcouple(2);
		CSET(u);
		CSET(v);
		return 1;

}

ADMVideoChromaShift::~ADMVideoChromaShift()
{
	if(_uncompressed)
 		delete _uncompressed;
	_uncompressed=NULL;
	DELETE(_param);

}
uint8_t ADMVideoChromaShift::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{

		ADM_assert(frame<_info.nb_frames);
		ADM_assert(_param);									
								
		// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

		// copy luma as it is left untouched
		uint32_t w=_info.width;
		uint32_t h=_info.height;
//		uint8_t *in,*out;
		uint32_t page;

		page=(w*h);

		memcpy(YPLANE(data),YPLANE(_uncompressed),page);

		if(!_param->u)
			{
				memcpy(UPLANE(data),UPLANE(_uncompressed),page>>2);
			}
		else
			{
				shift(UPLANE(data),UPLANE(_uncompressed),w>>1,h>>1,_param->u);
			}
		if(!_param->v)
			{
				memcpy(VPLANE(data),VPLANE(_uncompressed),page>>2);
			}
		else
			{
				shift(VPLANE(data),VPLANE(_uncompressed),w>>1,h>>1,_param->v);
			}
		if(_param->u)
			fixup(YPLANE(data),w,h,_param->u*2);
		if(_param->v)
			fixup(YPLANE(data),w,h,_param->v*2);
		  data->copyInfo(_uncompressed);
      return 1;
}
/*
	Black out the area were we don't have a valid chroma color

*/
uint8_t ADMVideoChromaShift::fixup(uint8_t *target,uint32_t width, uint32_t height,int32_t val)
{
uint32_t line,page;
uint8_t *zero;
uint8_t *zerochroma;
/*
	If val is >0
		Source  ++++++++
		Target   __++++++
*/
	page=(width*height)>>2;
	if(val>0)
		{
			line=val;
			zero=target;
			zerochroma=target+width*height;
			for(uint32_t h=height;h>0;h--)
			{
				memset(zero,0,val);
				zero+=width;
			}
			val>>=1;
			for(uint32_t h=height>>1;h>0;h--)
			{
				memset(zerochroma,128,val);
				memset(zerochroma+page,128,val);
				zerochroma+=width>>1;
			}
		}
/*
	if val is <0
		Source ++++++
		Target  ++++__

*/

		else
		{
			val=-val;

			zero=target+width-val;
			zerochroma=target+width*height;
			zerochroma+=(width-val)/2;

			for(uint32_t h=height;h>0;h--)
			{
				memset(zero,0,val);
				zero+=width;
			}

			val>>=1;
			for(uint32_t h=height>>1;h>0;h--)
			{
				memset(zerochroma,128,val);
				memset(zerochroma+page,128,val);
				zerochroma+=width>>1;
			}
		}
}
uint8_t ADMVideoChromaShift::shift(uint8_t *target,uint8_t *source, uint32_t width, uint32_t height,int32_t val)
{
uint32_t line;

/*
	If val is >0
		Source  ++++++++
		Target   __++++++
*/
	if(val>0)
		{
			line=width-val;
			target+=val;
			for(uint32_t h=height;h>0;h--)
			{
			memcpy(target,source,line);
			target+=width;
			source+=width;
			}
		}
/*
	if val is <0
		Source ++++++
		Target  ++++__

*/

		else
		{
			val=-val;
			line=width-val;
			source+=val;

			for(uint32_t h=height;h>0;h--)
			{
			memcpy(target,source,line);
			target+=width;
			source+=width;
			}
		}
	return 1;
}
uint8_t DIA_getChromaShift( AVDMGenericVideoStream *instream,CHROMASHIFT_PARAM    *param );
uint8_t ADMVideoChromaShift::configure( AVDMGenericVideoStream *instream)

{
    return DIA_getChromaShift(instream,_param);

}

#endif
