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
#include "oplug_ogm/op_ogsave.h"

//_______________________________________________
uint8_t ogmSave(char  *name)
{
uint8_t ret;
	ADM_ogmWrite *writter;
	if(videoProcessMode)
		writter=new ADM_ogmWriteProcess;
	else
		writter=new ADM_ogmWriteCopy;
	writter->save(name);
	
	delete writter;
	return ret;
}

//_______________________________________________
ADM_ogmWrite::ADM_ogmWrite(void)
{
_audioBuffer=_videoBuffer=NULL;
videoStream=videoStream=NULL;
encoding_gui=NULL;
_fd=NULL;
_togo=0;
}

//_______________________________________________
ADM_ogmWrite::~ADM_ogmWrite()
{
#define FREE_IF(x) if(x) {delete x;x=NULL;}
#define FREE_IFX(x) if(x) {delete [] x;x=NULL;}

	FREE_IFX(_audioBuffer);
	FREE_IFX(_videoBuffer);
	FREE_IF(videoStream);
	FREE_IF(videoStream);
	FREE_IF(encoding_gui);
	if(_fd) fclose(_fd);
	_fd=NULL;


}
//_______________________________________________
uint8_t ADM_ogmWrite::save(char *name)
{

uint8_t *bufr;
uint32_t len,flags;
uint8_t error=0;
double audioTarget,audioCurrent,audioInc;
WAVHeader	*info=NULL;

		_fd=fopen(name,"wb");
		if(!_fd)
		{
			GUI_Alert("Problem writing to that file\n");
			return 0;
		}

		videoStream=new ogm_page(_fd,0);
	/*
		if(currentaudiostream)
		{
			audioStream=new ogm_page(fd,1);
		}
	*/
		encoding_gui=new DIA_encoding(25000);
		//______________ Write headers..._____________________
		
		if(!initVideo())
		{
			fclose(_fd);
			GUI_Alert("Troubles initializing video\n");
			return 0;
		
		}
#if 0		
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
#endif		
		encoding_gui->setFps(_fps1000);
		encoding_gui->reset();
		// ___________________Then body_______________________
		for(uint32_t j=0;j<=_togo && !error;j++)
		{
			encoding_gui->setFrame(j,_togo);
			if(!encoding_gui->isAlive())
			{
				error=1;
				continue;
			}
			if(!writeVideo(j)) error=1;
#if 0			
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
				audioPcm/=1152; // Hardcoded to MP3 as of now
				
				audioStream->write(red,bufr,0,(uint64_t)floor(audioPcm));
				
			}
#endif			
		}
		delete encoding_gui;
		encoding_gui=NULL;
		//________________ Flush______________________
		videoStream->flush();
		//if(audioStream) audioStream->flush();
		// ____________Close____________________
		fclose(_fd);

	return !error;
}
// Dummy ones
uint8_t	ADM_ogmWrite::initVideo(void)
{
		assert(0);
		return 0;

}
//___________________________________________________
uint8_t	ADM_ogmWrite::writeVideo(uint32_t frame)
{
		assert(0);
		return 0;

}
// EOF
