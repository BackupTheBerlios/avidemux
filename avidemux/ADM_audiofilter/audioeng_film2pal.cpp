/***************************************************************************
                          Audio codec that tries to keep the sync
				when doing PAL<->FILM fps change.
			  Simply enough we drop or duplicate sample
			  to compensate. 
                             -------------------
    begin                : Sun Jan 13 2002
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
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "audioprocess.hxx"
#include "audioeng_film2pal.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"

// Integer only
#define PAL_SAM  25025
#define FILM_SAM 24000

AVDMProcessAudio_Film2Pal::AVDMProcessAudio_Film2Pal(AVDMGenericAudioStream * instream):AVDMBufferedAudioStream
    (instream)
{
    // nothing special here...
    _target=0;
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    _wavheader->byterate =
	_wavheader->channels * _wavheader->frequency * 2;
    _instream->goToTime(0);
    strcpy(_name, "PROC:F2PL");
    _length = instream->getLength();
    
    double d;
    
    d=(double)_length;
    
    d*=FILM_SAM;
    d/=PAL_SAM;
    d*=1.05; // Add 5 % margin
    
    _length=(uint32_t)floor(d);
    printf("Film 2 pal : %lu\n",(unsigned long int)_length);

};
  AVDMProcessAudio_Film2Pal::~AVDMProcessAudio_Film2Pal()
  {
     	delete _wavheader;
	_wavheader=NULL;
  }


//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t 	AVDMProcessAudio_Film2Pal::grab(uint8_t *obuffer)
{
    uint32_t rd,rendered,org;
    uint8_t *out=NULL,*copy=NULL;
    uint32_t min;
    
    org=rd = _instream->readDecompress(8192*4, _bufferin);
    if(!rd) return MINUS_ONE;        
   
    
   
    min=_wavheader->channels*2;
    
    rendered=0;
    
    // copy four by four
    copy=_bufferin;
    out=obuffer;
    
    while(rd>=min)
    {
    	for(uint32_t i=0;i<min;i++)
	{
		*out++=*copy++;
		
	}
	
	rendered+=min;
	rd-=min;
	
	_target+=(PAL_SAM-FILM_SAM);
	//_______________________________________
	// Remove sample
	// To compensate
	//_______________________________________
	// Fps higher=we have to accelerate sound = remove samples
	// The compression factor is 1-((25-24)/25)	
	while(_target>PAL_SAM)
	{
					
		rendered-=min;	
		out-=min;
		_target=_target-PAL_SAM;
	}
	
    }
    return rendered;

};
//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________

AVDMProcessAudio_Pal2Film::AVDMProcessAudio_Pal2Film(AVDMGenericAudioStream * instream ):AVDMBufferedAudioStream
    (instream)
{
    // nothing special here...
    _target=0;
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    _wavheader->byterate =
	_wavheader->channels * _wavheader->frequency * 2;
    _instream->goToTime(0);
    strcpy(_name, "PROC:P2FL");
    _length = instream->getLength();
    
    double d;
    
    d=(double)_length;
    
    d*=PAL_SAM;
    d/=FILM_SAM;
    
    _length=(uint32_t)floor(d);
    printf("Pal2Film : %lu\n",(unsigned long int)_length);

};
 AVDMProcessAudio_Pal2Film::~AVDMProcessAudio_Pal2Film()
  {
     	delete _wavheader;
	_wavheader=NULL;
  }


uint32_t 	AVDMProcessAudio_Pal2Film::grab(uint8_t *obuffer)
{
    uint32_t rd,rendered;
    uint8_t *out=NULL,*copy=NULL;
    uint32_t min;
    
    rd = _instream->readDecompress(8192*4, _bufferin);
    if(!rd) return MINUS_ONE;        
   
    min=_wavheader->channels*2;
    
    rendered=0;
    
    // 
    // copy sample per sample, 16 bits mono to stereo
    // 
    copy=_bufferin;
    out=obuffer;
    while(rd>min)
    {
    	for(uint32_t i=0;i<min;i++)
	{
		*out++=*copy++;
		rendered++;
		rd--;
		
	}	
	_target+=(PAL_SAM-FILM_SAM);
	//_______________________________________
	// Duplicate sample
	// To compensate
	//_______________________________________
	// In : One second is worth FILM_SAM samples
	// Out:FILM_SAM+(PAL_SAM-FILM_SAM)*FILM_SAM/FILM_SAM=PAL_SAM = 25 fps

	while(_target>FILM_SAM)
	{
		for(uint32_t i=0;i<min;i++)
		{
			*out++=*(copy-min+i);
			rendered++;
		
		}			
		_target=_target-FILM_SAM;
	}
    }
    
    return rendered;

};

