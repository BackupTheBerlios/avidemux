//
// C++ Implementation: op_ogsavecopy
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "config.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/toolkit.hxx"
#include "subchunk.h"
#include "avilist.h"

#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_filter/video_filters.h"
#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"

#include "ADM_library/default.h"

#include "oplug_ogm/op_ogsave.h"

uint8_t	ADM_ogmWriteCopy::initVideo(void)
{
		
		_togo=frameEnd-frameStart;
		stream_header header;
		
		_videoBuffer=new uint8_t[avifileinfo->width*avifileinfo->height*3];

		

		
		memset(&header,0,sizeof(header));
		
		memcpy(&(header.streamtype),"video\0\0\0",8);
		memcpy(&(header.subtype),&(avifileinfo->fcc),4);
		
		encoding_gui->setCodec((char *)"Copy");
		encoding_gui->setPhasis((char *)"Saving...");
		
		header.size=sizeof(header);
		header.video.width=avifileinfo->width;
		header.video.height=avifileinfo->height;
		// Timing ..
		double duration; // duration in 10us
		_fps1000=avifileinfo->fps1000;
		duration=avifileinfo->fps1000;
		duration=1000./duration;
		duration*=1000*1000;
		duration*=10;
		
		header.time_unit=(int64_t)duration;
		header.samples_per_unit=1;
		
		header.buffersize=0x10000;
		header.bits_per_sample=24;
		
		
		return videoStream->writeHeaders(sizeof(header),(uint8_t *)&header); // +4 ?

}
//___________________________________________________
uint8_t	ADM_ogmWriteCopy::writeVideo(uint32_t frame)
{
uint32_t len,flags;

		if(!  video_body->getFrameNoAlloc (frameStart+frame, _videoBuffer, &len,     &flags)) return 0;	
		
		encoding_gui->feedFrame(len);
		
		return videoStream->write(len,_videoBuffer,flags,frame);

}
//___________________________________________________
ADM_ogmWriteCopy::ADM_ogmWriteCopy( void)
{


}
//___________________________________________________
ADM_ogmWriteCopy::~ADM_ogmWriteCopy()
{


}

