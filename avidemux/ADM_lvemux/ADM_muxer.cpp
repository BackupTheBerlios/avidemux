//
// C++ Implementation: ADM_muxer
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
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>

#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_lvemux/ADM_muxer.h"

extern "C" {
	#include "ADM_lvemux/mux_out.h"
};
MpegMuxer::MpegMuxer( void )
{
	packStream=NULL;
	byteHead=byteTail=0;
}
MpegMuxer::~MpegMuxer(  )
{
	assert(!packStream);
}
uint8_t MpegMuxer::forceRestamp(void)
{
	assert(packStream);
	((PackStream *)packStream)->forceRestamp=1;
	return 1;
}
uint8_t MpegMuxer::open(char *filename, uint32_t vbitrate, uint32_t fps1000, WAVHeader *audioheader)
{

double fps;
uint32_t abitrate,frequency;
PackStream *pack=NULL;

int bytes_needed,samples_needed;
float bn,sn;

	fps=fps1000;
	fps/=1000.;
	
	// lookup audio info from wavheader
	abitrate=audioheader->byterate*8;
	frequency=audioheader->frequency;
	switch(audioheader->encoding)
	{
		case WAV_MP2: audioType=AUDIO_ID_MP2;break;
		case WAV_AC3: audioType=AUDIO_ID_AC3;break;
		default:
			GUI_Alert("Audio type is not suitable for DVD!");
			return 0;
	}
	
	pack=mux_open(filename,(int)vbitrate,fps,(int)abitrate,(int)frequency,audioType);
	if(!pack)
	{
		GUI_Alert("Init of lvemux failed!");
		return 0;
	}
	packStream=(void *)pack;
	printf("Lvemux successfully initialized with :\n fq=%lu audio bitrate"
	" %lu video bitrate %lu framerate :%f id:%lu",frequency,abitrate,vbitrate,fps,audioType);
	
	sn=audioheader->frequency*1000.0;
	sn/=fps1000;
	samples_needed = (int)floor(sn+0.5); // pcm sample per frame
	  
	bn=1000./fps1000;
	bn*=audioheader->byterate;
	
	bytes_needed=(int)floor(bn+0.5);
	
	printf("Sample per frame : %d %f, bytes per frame :%d %f\n",samples_needed,sn,bytes_needed,bn);
	//_packSize=bytes_needed;	  
	_packSize=pack->audio_encoded_fs;
	printf("Pack size:%d\n",_packSize);
  
	return 1;

}

uint8_t MpegMuxer::writeAudioPacket(uint32_t len, uint8_t *buf)
{
int r;
uint32_t t=0;
uint32_t n;
	assert(packStream);
	memcpy(buffer+byteTail,buf,len);
	byteTail+=len;
	
	
	n=(byteTail-byteHead)/_packSize;
	if(n)
	
	{
		printf("Writing :%d bytes\n",n*_packSize);
		r=mux_write_packet((PackStream *)packStream, 
                               audioType, buffer+byteHead, n*_packSize); 
		byteHead+=n*_packSize;
		t+=n*_packSize;
	}
			     
	//aprintf("This round : %lu\n",t);  
	if(byteTail>=MUX_BUFFER_SIZE)
	{
		memmove(buffer,buffer+byteHead,byteTail-byteHead);
		byteTail-=byteHead;
		byteHead=0;
	
	}
	if(byteTail>MUX_BUFFER_SIZE)
	{
		printf("Tail : %lu\n",byteTail);
		printf("Head : %lu\n",byteHead);
		printf("Delta: %lu\n",byteTail-byteHead);
		printf("Pack : %lu\n",_packSize);
		assert(0);
	
	}
	return 1;

}
uint8_t MpegMuxer::writeVideoPacket(uint32_t len, uint8_t *buf)
{
int r;
	assert(packStream);
	r=mux_write_packet((PackStream *)packStream, 
                               VIDEO_ID, buf, (int) len); 
	return 1;

}
uint8_t MpegMuxer::close ( void )
{
	assert(packStream);
	 mux_close((PackStream *)packStream);
	 packStream=NULL;
	 return 1;
	
}

