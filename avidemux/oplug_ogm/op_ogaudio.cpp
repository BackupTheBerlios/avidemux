//
// C++ Implementation: op_ogaudio
//
// Description: 
//
//
// This file handles the audio track in OGM output plugin
// The difficult part with audio is that the counter is in
//  audio packet hence depends on the codec itself
//
// We will always try to put complete audio packet in the stream
// as the muxer is a bit stupid anyway
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audio/ADM_a52info.h"

#include "oplug_ogm/op_ogsave.h"

// should be enougth, at 500 kbps / 25 fps -> 3kbyte per slice
#define OGM_AUDIO_BUFFER 64000

extern const char *getStrFromAudioCodec( uint32_t codec);	

//__________________________________________________
uint8_t		ADM_ogmWrite::initAudio(void)
{
uint32_t 	tstart;
WAVHeader	*info=NULL;

		if(!currentaudiostream)
		{
			encoding_gui->setAudioCodec("None");
			return 1;
		}
		if(currentaudiostream->isCompressed() && currentaudiostream->isDecompressable())
		{
			audioStream=new ogm_page(_fd,2);
		}
		else 
		{			
			return 0;
		}
		//
		if(audioProcessMode)
		{
			uint16_t fcc;
			audioFilter=  buildAudioFilter (currentaudiostream,
							video_body->getTime (frameStart),
				  			0xffffffff);
			fcc=audioFilter->getInfo()->encoding;
			encoding_gui->setAudioCodec(getStrFromAudioCodec(fcc));
		}
		else
		{
			audioFilter= (AVDMGenericAudioStream *) currentaudiostream;
			tstart = video_body->getTime (frameStart);
	  		audioFilter->goToTime (tstart);
			encoding_gui->setAudioCodec("Copy");
			
		}
		//______________ Write headers/ Audio..._____________________
		
		
		_audioBuffer=new uint8_t[OGM_AUDIO_BUFFER*2]; 
		
		char string[40];
		stream_header header;
		
		info=audioFilter->getInfo();
		memset(&header,0,sizeof(header));
		
		memcpy(&(header.streamtype),"audio\0\0\0",8);
		memset(&(header.subtype),0,4);
		sprintf(string,"%04X",info->encoding);
		//memcpy(&(header.subtype),&(info->encoding),2);
		memcpy(&(header.subtype),string,4);
		printf("audio encoding:%x\n",info->encoding);
		header.size=sizeof(header);
		header.audio.channels=info->channels;
		// not reliable header.audio.blockalign=info->blockalign;
		switch(info->encoding)
		{
			case WAV_MP3:
			case WAV_MP2:
					header.bits_per_sample=0;
					header.audio.blockalign=1152;break;
			case WAV_AC3:	
					header.bits_per_sample=2;
					header.audio.blockalign=1536;break;
			default:
				header.audio.blockalign=info->blockalign;
				header.bits_per_sample=0;
				break;
		
		}
		header.audio.avgbytespersec=info->byterate;
		
		header.time_unit=(int64_t)10000000;
		header.samples_per_unit=info->frequency;
		
		header.buffersize=info->frequency; // half a sec
		
		header.default_len=1;
		
		audioStream->writeHeaders(sizeof(header),(uint8_t *)&header); // +4 ?
		_oneRound=info->byterate;
		_oneRound*=1000;
		_oneRound/=_fps1000;
		_audioCurrent=0;
		_audioTarget=0;
		
		_audioHead=_audioTail=0;
		printf("One video frame is %f audio bytes, fps is %d, byterate is %lu fq is %lu \n",_oneRound,_fps1000,info->byterate,info->frequency);
		return 1;	
	
}
//__________________________________________________
uint8_t		ADM_ogmWrite::writeAudio(uint32_t vframe)
{
uint32_t chunk,red;
	if(!audioStream) return 1; // nothing to do
		
	// Do some sanity check / cleanup on the buffer
	
	assert(_audioTail>=_audioHead);
	
	//
	if(_audioTail>OGM_AUDIO_BUFFER)
	{
		memmove(_audioBuffer,_audioBuffer+_audioHead,_audioTail-_audioHead);
		_audioTail-=_audioHead;
		_audioHead=0;
	}
	assert((_audioTail-_audioHead)<(OGM_AUDIO_BUFFER>>1));
	
	
	// Fill buffer (CBR)
	
	_audioTarget+=_oneRound;
	//if(!vframe) _audioTarget+=_oneRound;
	if(_audioCurrent<_audioTarget)
	{
		chunk=(uint32_t)floor(_audioTarget-_audioCurrent);
		
		red=audioFilter->read(chunk, _audioBuffer+_audioTail);
		if(red==MINUS_ONE)
		{
			printf("OGM muxer:Read failed\n");
			return 0;
		}
					
		if(red>chunk)
		{
			printf("Bonanza!\n");
		}
		_audioTail+=red;
	}
	uint32_t wrote;
	// Try to write complete audio frames
	//if(_audioCurrent>2000)  // else don't bother
	{
		switch(audioFilter->getInfo()->encoding)
		{
			//case WAV_AC3:
			case WAV_MP3:
					wrote=putMP3(vframe);
					_audioHead+=wrote;
					_audioCurrent+=wrote;
					break;
			default:
					
					wrote=putAC3(vframe);
					_audioHead+=wrote;
					_audioCurrent+=wrote;
					break;
						
		}
	}
	return 1;
}
// We assume it starts aligned
uint32_t ADM_ogmWrite::putAC3( uint32_t vframe )
{
uint8_t *start=_audioBuffer+_audioHead;
uint8_t *sstart=start;
uint32_t left=_audioTail-_audioHead;
int flags, sample_rate,bit_rate;
int size=0;

		
		while(size+6<left)
		{
			if(start[0]!=0x0b || start[1]!=0x77)
			{
				printf("Head: %d  Tail:%d x:%x %x\n",_audioHead,_audioTail,start[0],start[1]);
				printf("Left:%d\n",left);
				return 0;
			}
			
			size= ADM_a52_syncinfo (start, &flags, &sample_rate, &bit_rate);
			//printf("This round%d %d\n",size,sample_rate);
			if(!size)
			{
				printf("OGM: Cannot sync\n");
				printf("%x %x, packet %x\n",start[0],start[1],_packet);
				printf("left :%d\n",left);
				return 0;
			}
			if(size+6<left)
			{
				_packet++;			
				start+=size;
				left-=(size);
			}
			
		}
		uint32_t put=start-sstart;		
		// nothing to write
		if(!put) return 0;
		
		double l;
		l=_audioCurrent+put;		
		l=l/audioFilter->getInfo()->byterate;
		l=l*audioFilter->getInfo()->frequency;
		
		audioStream->write(put,_audioBuffer+_audioHead,AVI_KEY_FRAME,(uint64_t)(floor(l)));
		return put;

}
//______________________________________________
uint32_t ADM_ogmWrite::putMP3( uint32_t vframe )
{
		uint32_t n=_audioTail-_audioHead,t;
		n=n/1152;
		n*=1152;
		//printf("Tail :%lu Head :%lu n:%lu\n",_audioTail,_audioHead,n);
		t=_audioCurrent+n;
		if(!n) return 0;
		double l;
		
		l=t;
		l=l/audioFilter->getInfo()->byterate;
		l=l*audioFilter->getInfo()->frequency;
		

		audioStream->write(n,_audioBuffer+_audioHead,AVI_KEY_FRAME,(uint64_t)(floor(l)));
		
		return n;

}
//________________________________________
uint8_t		ADM_ogmWrite::endAudio(void)
{
	if(!audioStream) return 1;
		
		double l;
		
		l=_audioCurrent+_audioTail-_audioHead;
		l=l/audioFilter->getInfo()->byterate;
		l=l*audioFilter->getInfo()->frequency;
		
	audioStream->write(_audioTail-_audioHead,_audioBuffer+_audioHead,AVI_KEY_FRAME,(uint64_t)(floor(l)));
	if(audioStream) audioStream->flush();
	return 1;

}
//________
