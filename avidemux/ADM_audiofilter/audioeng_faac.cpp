
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
				 &samples_input);
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
    cfg->outputFormat = 0;
    cfg->inputFormat = FAAC_INPUT_16BIT;
	_wavheader->byterate=(bitrate*1000)/8;
    if (!faacEncSetConfiguration(_handle, cfg)) 
    {
        printf("FAAC: Cannot set conf for faac with fq=%lu chan=%lu br=%lu\n",
				_wavheader->frequency,_wavheader->channels,bitrate);
	return 0;
    }
    return 1;
}

//_____________________________________________
uint32_t AVDMProcessAudio_Faac::grab(uint8_t * obuffer)
{
uint32_t len,sam;
	if(getPacket(obuffer,&len,&sam))
		return sam;
	return MINUS_ONE;
}
//______________________________________________
uint8_t	AVDMProcessAudio_Faac::getPacket(uint8_t *dest, uint32_t *len, 
					uint32_t *samples)
{				
uint8_t  *buf=dropBuffer;
uint32_t rdall=0,asked,rd;
int wr;

	asked=_incoming_frame*_wavheader->channels*2;
	rd=_instream->read(asked,buf);
	printf("Faac: asked :%lu got %lu",asked,rd);
	if(rd==0)
		return MINUS_ONE;
	    
  // Now encode
  
   wr = faacEncEncode(_handle,
                      (int32_t *)buf,
                      rd/(2*_wavheader->channels),
                      dest,
                      64*1024
		      );
	*len=wr;
	*samples=rd/(2*_wavheader->channels);
		printf("Faac:  out:%d\n",wr);
	return 1;
}

#endif		
// EOF
