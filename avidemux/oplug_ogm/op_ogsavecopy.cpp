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

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME 0
#include "ADM_toolkit/ADM_debug.h"

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
		
		header.default_len=1;
		
		return videoStream->writeHeaders(sizeof(header),(uint8_t *)&header); // +4 ?

}
//___________________________________________________
uint8_t	ADM_ogmWriteCopy::writeVideo(uint32_t frame)
{
uint32_t len,flags_s,flags;
uint32_t forward;
uint8_t ret1=0;

		// Check for B_frames
		if(!video_body->isReordered(frameStart+frame))
		{

			if(!  video_body->getFrameNoAlloc (frameStart+frame, _videoBuffer, &len,     &flags)) 
				return 0;		
			encoding_gui->feedFrame(len);
			return videoStream->write(len,_videoBuffer,flags,frame);
		}
		
		// we DO have b frame
		// 
		video_body->getFlags (frameStart + frame, &flags_s);
		if(flags_s & AVI_B_FRAME)
			{ 	// search if we have to send a I/P frame in adance
				
				uint32_t forward;
				forward=searchForward(frameStart+frame);
				// if we did not sent it, do it now
				if(forward!=_lastIPFrameSent)
				{
					aprintf("\tP Frame not sent, sending it :%lu\n",forward);
					ret1 = video_body->getFrameNoAlloc (forward, _videoBuffer, &len,
				      		&flags_s);
					_lastIPFrameSent=forward;

				}
				else
				{
					// we already sent it :)
					// send n-1
					aprintf("\tP Frame already, sending  :%lu\n",frameStart+frame-1);
					ret1 = video_body->getFrameNoAlloc (frameStart+frame-1, _videoBuffer, &len,
				      	&flags_s);

				}

			}
			else // it is not a B frame and we have nothing on hold, sent it..
			{
				// send n-1 if we reach the fwd reference frame
				if((frame+frameStart)==_lastIPFrameSent)
				{
					aprintf("\tSending Last B-frame :(%lu)\n",frameStart + frame-1);
					ret1 = video_body->getFrameNoAlloc (frameStart + frame-1, _videoBuffer, &len,
				      		&flags_s);

				}
				else
				{
					aprintf("\tJust sending it :(%lu)-(%lu)\n",frameStart + frame,_lastIPFrameSent);
					ret1 = video_body->getFrameNoAlloc (frameStart + frame, _videoBuffer, &len,
				      		&flags_s);

				}
			}

		return ret1;

}
// Return the next non B frame
// 0 if not found
//___________________________________________________
uint32_t ADM_ogmWriteCopy::searchForward(uint32_t startframe)
{
		uint32_t fw=startframe;
		uint32_t flags;
		uint8_t r;

			while(1)
			{
				fw++;
				r=video_body->getFlags (fw, &flags);
				if(!(flags & AVI_B_FRAME))
				{
					return fw;

				}
				assert(r);
				if(!r)
				{
					printf("\n Could not locate last non B frame \n");
					return 0;
				}

			}
}
//___________________________________________________
ADM_ogmWriteCopy::ADM_ogmWriteCopy( void)
{
	_lastIPFrameSent=0;

}
//___________________________________________________
ADM_ogmWriteCopy::~ADM_ogmWriteCopy()
{


}

