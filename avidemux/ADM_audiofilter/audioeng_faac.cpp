
/***************************************************************************
                          audioeng_faac.cpp  -  description
                             -------------------
    begin                : Sun Sep 112004
    copyright            : (C) 2002-4 by mean
    email                : fixounet@free.fr
    
    Interface to FAAC
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#ifdef USE_FAAC
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "ADM_lavcodec.h"

#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "faac.h"
#include "ADM_audiofilter/audioeng_faac.h"

static uint16_t remap[4096];
// Ctor: Duplicate
//__________

AVDMProcessAudio_Faac::AVDMProcessAudio_Faac(AVDMGenericAudioStream * instream)
:AVDMBufferedAudioStream    (instream)
{
    _wavheader = new WAVHeader;    
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
   _wavheader->encoding=WAV_AAC;
    _instream->goToTime(0);	// rewind
    _length = _instream->getLength();    
    _handle=NULL;
};


AVDMProcessAudio_Faac::~AVDMProcessAudio_Faac()
{
    delete(_wavheader);
    
    if(_handle)
    	 faacEncClose(_handle);
    _handle=NULL;
    _wavheader=NULL;

};


//________________________________________________
//   Init lame encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode                         : ADM_STEREO etc...
// bitrate              : Bitrate in kbps (96,192...)
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________
uint8_t AVDMProcessAudio_Faac::init( uint32_t bitrate)
{
unsigned long int samples_input, max_bytes_output;
faacEncConfigurationPtr cfg;

     _handle = faacEncOpen(_wavheader->frequency,
                                 _wavheader->channels,
                                 &samples_input,
				 &max_bytes_output);
    if(!_handle)
    {
    	printf("Cannot open faac with fq=%lu chan=%lu br=%lu\n",
				_wavheader->frequency,_wavheader->channels,bitrate);
	return 0;    
    }
    printf("FAAC!Sample input:%lu\nmax byte output\n",samples_input,max_bytes_output);
    _incoming_frame=samples_input;
    cfg= faacEncGetCurrentConfiguration(_handle);
    
    // Set default conf, same as ffmpeg
    cfg->aacObjectType = LOW;
    cfg->mpegVersion = MPEG4;
    cfg->useTns = 0;
    cfg->allowMidside = 1;
    cfg->bitRate = bitrate*1000;
    cfg->outputFormat = 0; // 0 Raw 1 ADTS
    cfg->inputFormat = FAAC_INPUT_16BIT;
    cfg->useLfe=1;	
    if (!faacEncSetConfiguration(_handle, cfg)) 
    {
        printf("FAAC: Cannot set conf for faac with fq=%lu chan=%lu br=%lu\n",
				_wavheader->frequency,_wavheader->channels,bitrate);
	return 0;
    }
    // update
    _wavheader->byterate=(bitrate*1000)/8;
//    _wavheader->dwScale=1024;
//    _wavheader->dwSampleSize=0;
    _wavheader->blockalign=4096;
    _wavheader->bitspersample=0;
    
    printf("Faac init*zes\n");
    
    printf("Version        : %s\n",cfg->name);
    printf("Bitrate        : %lu\n",cfg->bitRate);
    printf("Mpeg2 (1)/4(0) : %u\n",cfg->mpegVersion);
    printf("Use lfe      ) : %u\n",cfg->useLfe);
    printf("Sample output  : %lu\n",_incoming_frame);

    
    return 1;
}

//_____________________________________________
uint32_t AVDMProcessAudio_Faac::grab(uint8_t * obuffer)
{
uint32_t len,sam;
	//printf("Faac: Read\n");
	if(getPacket(obuffer,&len,&sam))
		return len;
	return MINUS_ONE;
}
//______________________________________________
uint8_t	AVDMProcessAudio_Faac::getPacket(uint8_t *dest, uint32_t *len, 
					uint32_t *samples)
{				
uint8_t  *buf=dropBuffer;
uint32_t nbSample=0;
uint32_t rdall=0,asked,rd;
int wr;

	asked=_incoming_frame*2*_wavheader->channels;
	rd=_instream->read(asked,buf);
	
	if(rd==0)
		return 0;
	if(rd==MINUS_ONE)
		return 0;		
	    
  // Now encode
  	nbSample= rd/(2*_wavheader->channels);
	switch(_wavheader->channels)
	{
	case 1:
        	wr = faacEncEncode(_handle,
                      (int32_t *)buf,
                      nbSample, // Nb sample for all channels
                      dest,
                      64*1024
		      );
		break;
	case 2:
		// remap
		uint16_t *in;
		in=(uint16_t *)buf;
		for(int i=0;i<nbSample;i++)
		{
			remap[i]=in[2*i];
			remap[i+nbSample]=in[2*i+1];
		}
		wr = faacEncEncode(_handle,
                      (int32_t *)buf,
                      nbSample*2, // Nb sample for all channels
                      dest,
                      64*1024
		      );
		break;
	}
		     
	*len=wr;
	*samples=nbSample;
	printf("Faac: asked :%lu got %lu out len:%d sample:%d\n",asked,rd,wr,nbSample);
	return 1;
}

#endif		
// EOF
