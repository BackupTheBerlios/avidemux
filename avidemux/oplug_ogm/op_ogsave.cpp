//
// C++ Implementation: op_ogsave
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <math.h>
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
#include "oplug_ogm/op_ogpage.h"


uint8_t ogmSave(char  *name)
{
FILE *fd=NULL;
uint8_t *bufr;
uint32_t len,flags;
uint8_t error=0;
double audioTarget,audioCurrent,audioInc;
WAVHeader	*info=NULL;
		bufr=new uint8_t[avifileinfo->width*avifileinfo->height*3];
		fd=fopen(name,"wb");
		if(!fd)
		{
			GUI_Alert("Problem writing to that file\n");
			return 0;
		}
		ogm_page *videoStream,*audioStream=NULL;
		videoStream=new ogm_page(fd,0);
	
		if(currentaudiostream)
		{
			audioStream=new ogm_page(fd,1);
		}
		//______________ Write headers..._____________________
		stream_header header;
		
		memset(&header,0,sizeof(header));
		
		memcpy(&(header.streamtype),"video\0\0\0",8);
		memcpy(&(header.subtype),&(avifileinfo->fcc),4);
		header.size=sizeof(header);
		header.video.width=avifileinfo->width;
		header.video.height=avifileinfo->height;
		// Timing ..
		double duration; // duration in 10us
		duration=avifileinfo->fps1000;
		duration=1000./duration;
		duration*=1000*1000;
		duration*=10;
		
		header.time_unit=(int64_t)duration;
		header.samples_per_unit=1;
		
		header.buffersize=0x10000;
		header.bits_per_sample=24;
		
		
		videoStream->writeHeaders(sizeof(header),(uint8_t *)&header); // +4 ?
		//______________ Write headers/ Audio..._____________________
		if(audioStream)
		{
		char string[40];
		stream_header header;
		
		info=currentaudiostream->getInfo();
		memset(&header,0,sizeof(header));
		
		memcpy(&(header.streamtype),"audio\0\0\0",8);
		memset(&(header.subtype),0,4);
		sprintf(string,"%04X",info->encoding);
		//memcpy(&(header.subtype),&(info->encoding),2);
		memcpy(&(header.subtype),string,4);
		printf("audio encoding:%x\n",info->encoding);
		header.size=sizeof(header);
		header.audio.channels=info->channels;
		header.audio.blockalign=info->blockalign;
		header.audio.avgbytespersec=info->byterate;
		// Timing .. FIXME
		double duration; // duration in 10us
		duration=avifileinfo->fps1000;
		duration=1000./duration;
		duration*=1000*1000;
		duration*=10;
		
		header.time_unit=(int64_t)duration;
		header.samples_per_unit=1;
		
		header.buffersize=0x10000;
		header.bits_per_sample=16;
		
		
		audioStream->writeHeaders(sizeof(header),(uint8_t *)&header); // +4 ?
		audioInc=info->byterate;
		audioInc*=1000;
		audioInc/=avifileinfo->fps1000;
		audioCurrent=0;
		audioTarget=0;
		}
		uint32_t chunk,red;
		double audioPcm;
		// ___________________Then body_______________________
		for(uint32_t j=frameStart;j<=frameEnd && !error;j++)
		{
			error=!  video_body->getFrameNoAlloc (j, bufr, &len,
				      &flags);	
			if(error)
			{
				 continue;
			}
			videoStream->write(len,bufr,flags,j-frameStart);
			
			//
			if(audioStream)
			{
				audioTarget+=audioInc;
				chunk=(uint32_t)floor(audioTarget-audioCurrent);
				//printf("This chunk :%d\n",chunk);
				red=currentaudiostream->read(chunk, bufr);
				if(red==MINUS_ONE)
				{
					printf("Read failed\n");
					error=1;
					continue;
				}
				
				audioPcm=audioCurrent;
				audioCurrent+=red;
				audioPcm*=info->frequency; // back to sample
				audioPcm/=info->byterate; // durtion in second				
				
				audioStream->write(red,bufr,0,(uint64_t)floor(audioPcm));
				
			}
		}
		//________________ Flush______________________
		videoStream->flush();
		if(audioStream) audioStream->flush();
		// ____________Close____________________
		delete videoStream;
		if(audioStream) 
		{
			delete audioStream;
		}
		delete [] bufr;
		fclose(fd);

	return !error;
}
