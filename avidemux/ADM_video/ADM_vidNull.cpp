/***************************************************************************
                          ADM_vidNull.cpp  -  description
                             -------------------

    This filter takes format xxx as input and output uncompressed video

    begin                : Wed Mar 20 2002
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
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"



//_______________________________________________________________

AVDMVideoStreamNull::AVDMVideoStreamNull(ADM_Composer *in,uint32_t start, uint32_t nb)
{
aviInfo aviinf;
  	_in=in;	
  	
  	// now build infos
  	
  	ADM_assert(_in->getVideoInfo(&aviinf));
  	_info.width=aviinf.width;
  	_info.height=aviinf.height;
	if(start+nb==aviinf.nb_frames-1)  nb++; // take last frame
  	_info.nb_frames=nb;
	_start=start;
  	_info.encoding=0;
	_info.fps1000=aviinf.fps1000;
	_info.orgFrame=start;
  	_uncompressed=(uint8_t *)malloc(3*aviinf.width*aviinf.height);
  	ADM_assert(_uncompressed);
	ADM_assert(start+nb<=aviinf.nb_frames);

  	aprintf("\n Null stream initialized with start frame = %lu, nbframe=%lu \n",_start,nb);
  	
}
AVDMVideoStreamNull::~AVDMVideoStreamNull()
{

 	free(_uncompressed);
}
//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoStreamNull::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
   																	uint8_t *data,uint32_t *flags)
{
        UNUSED_ARG(len);
    	UNUSED_ARG(flags);
		if(!(frame<_info.nb_frames))
		{
				printf("\n going out of bounds! :%ld / %ld \n",frame,_info.nb_frames);
				return 0;
		}
		
			// read uncompressed frame

    	if(!_in->getUncompressedFrame(_start+frame,data)     )
     			return 0;
        return 1;


}

uint8_t AVDMVideoStreamNull::configure( AVDMGenericVideoStream *instream){
	UNUSED_ARG(instream);
	return 0;
}
