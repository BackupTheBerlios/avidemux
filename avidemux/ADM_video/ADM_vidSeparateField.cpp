/***************************************************************************
                          Separate Fields.cpp  -  description
                             -------------------
Convert a x*y * f fps video into -> x*(y/2)*fps/2 video

Same idea as for avisynth separatefield


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
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFieldUtil.h"
#include "ADM_video/ADM_vidSeparateField.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM swapParam={0,{""}};

SCRIPT_CREATE(separatefield_script,AVDMVideoSeparateField,swapParam);
SCRIPT_CREATE(mergefield_script,AVDMVideoMergeField,swapParam);

BUILD_CREATE(separatefield_create,AVDMVideoSeparateField);
BUILD_CREATE(mergefield_create,AVDMVideoMergeField);

char *AVDMVideoSeparateField::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Separate Fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoSeparateField::AVDMVideoSeparateField(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
//	_uncompressed=new uint8_t[3*_info.width*_info.height];
	_uncompressed=new ADMImage(_info.width,_info.height);

	_info.height>>=1;
	_info.fps1000*=2;
	_info.nb_frames*=2;

}

// ___ destructor_____________
AVDMVideoSeparateField::~AVDMVideoSeparateField()
{
 	

}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoSeparateField::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint32_t ref;
			if(frame>=_info.nb_frames) return 0;

		ref=frame>>1;

		  if(!_in->getFrameNumberNoAlloc(ref, len, _uncompressed, flags)) return 0;

		if(frame&1) // odd image
			 vidFieldKeepOdd(_info.width,_info.height*2,_uncompressed->data,data->data);
		else
			 vidFieldKeepEven(_info.width,_info.height*2,_uncompressed->data,data->data);

      return 1;
}
//------------------ and merge them ------------------


char *AVDMVideoMergeField::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Merge fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoMergeField::AVDMVideoMergeField(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));
	//_uncompressed=new uint8_t[3*_info.width*_info.height];
	_uncompressed=new ADMImage(_info.width,_info.height);
	_uncompressed2=new ADMImage(_info.width,_info.height);
	_cache=new ADMImage(_info.width,_info.height);
	

	_info.height<<=1;
	_info.fps1000>>=1;
	_info.nb_frames>>=1;
	_lastAsked=0xffffffff;

}

// ___ destructor_____________
AVDMVideoMergeField::~AVDMVideoMergeField()
{
 		delete _uncompressed;
		delete _uncompressed2;
		delete _cache;
		_cache=_uncompressed=_uncompressed2=NULL;
}

/**
	Interleave frame*2 and frame*2+1
*/
uint8_t AVDMVideoMergeField::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint32_t ref,ref2;
		if(frame>=_info.nb_frames) return 0;

		ref=frame<<1;
		ref2=ref+1;

		if(_lastAsked==ref)
		{
				memcpy(_uncompressed->data,_cache->data, (_info.width*_info.height*3)>>2);
		}
		else
		{
			if(!_in->getFrameNumberNoAlloc(ref, len, _uncompressed, flags)) return 0;
		}
		if(!_in->getFrameNumberNoAlloc(ref2, len, _uncompressed2, flags)) return 0;

		 vidFieldMerge(_info.width,_info.height,_uncompressed->data,_uncompressed2->data,data->data);
		 _lastAsked=ref2;
		 memcpy(_cache->data,_uncompressed2->data, (_info.width*_info.height*3)>>2);

      return 1;
}
