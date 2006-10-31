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
#include "ADM_video/ADM_vidSwapSmart.h"
#include "ADM_video/ADM_interlaced.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_osSupport/ADM_debug.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM nullParam={0,{""}};


SCRIPT_CREATE(swapsmart_script,AVDMVideoSwapSmart,nullParam);
BUILD_CREATE(swapsmart_create,AVDMVideoSwapSmart);


char *AVDMVideoSwapSmart::printConf( void )
{
 	static char buf[50];

 	sprintf((char *)buf," Smart Swap fields");
        return buf;
}

//_______________________________________________________________
AVDMVideoSwapSmart::AVDMVideoSwapSmart(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
UNUSED_ARG(setup);
  	_in=in;
   	memcpy(&_info,_in->getInfo(),sizeof(_info));	
	_uncompressed=new ADMImage(_info.width,_info.height);



}
// ___ destructor_____________
AVDMVideoSwapSmart::~AVDMVideoSwapSmart()
{
 	delete  _uncompressed;
	_uncompressed=NULL;

}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoSwapSmart::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
//static Image in,out;
			if(frame>=_info.nb_frames) return 0;


			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;

		uint32_t w=_info.width;
		uint32_t h=_info.height;
		uint32_t page=w*h;
		uint32_t stride;



		uint8_t *odd,*even,*target,*target2;

		even=YPLANE(data);
		odd=even+w;
		target=YPLANE(_uncompressed);
		target2=_uncompressed->data+w;
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
		// now we have straight and swapped
		// which one is better ?
		uint32_t s,m;
		s=      ADMVideo_interlaceCount( YPLANE(data),_info.width, _info.height);
		m=      ADMVideo_interlaceCount( YPLANE(_uncompressed),_info.width, _info.height);

		aprintf(" %lu straight vs %lu swapped => %s\n",s,m,(s*2>m*3?"swapped":"straight"));

		// if swapped <= 66 % straight

		if(s*2>m*3)  // swapped is better
		{
			memcpy(YPLANE(data),YPLANE(_uncompressed),page);

		}

      return 1;
}


#endif
