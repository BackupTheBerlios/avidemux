/***************************************************************************
                          audioeng_mono.cpp  -  description
                             -------------------
                             Simple mono to stereo converter


    begin                : Mon Jun 10 2002
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
//#include <stream.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
//#include "../toolkit.hxx"
// Ctor       `AVDMProcessAudioStream::AVDMProcessAudioStream(AVD
//__________

AVDMProcessMono2Stereo::AVDMProcessMono2Stereo(AVDMGenericAudioStream * instream ):
AVDMBufferedAudioStream    (instream)
{
    // nothing special here...
	
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    if(_instream->getInfo()->channels==1)
    {
    	_engaged=1;
    	_wavheader->channels=2;
    	_wavheader->byterate*=2;
	_length = instream->getLength()*2;
	printf("Mono2stereo: enabled\n");
   }
   else
   	printf("Mono2stereo disabled, source is not mono\n");
    _instream->goToTime(0);
    strcpy(_name, "PROC:MONO");
    

};

AVDMProcessMono2Stereo::~AVDMProcessMono2Stereo()
{
    delete _wavheader;

}


uint32_t AVDMProcessMono2Stereo::grab( uint8_t * buffer)
{
uint16_t o,*in,*out;

	// convert mono to stereo

    uint32_t size,osize;

    size=_wavheader->frequency/20; // 50 ms slice
    size&=0xfffffe; // Even


    if(!_engaged)
    {
     	if(!(size=_instream->read(size, (uint8_t *)buffer))) return MINUS_ONE;
	return size;    
    }
    
    if(!(size=_instream->read(size, (uint8_t *)_buffer))) return MINUS_ONE;

    in=( uint16_t *)_buffer;
    out=(uint16_t *)buffer;
    size>>=1; 	// 16
    osize=size<<2;
    for(;size>0;size--)
    	{
        	*(out+1)=*out=*in++;
         	out+=2;
       }
	return osize;

}

//______________________________________________________________________
AVDMProcessStereo2Mono::AVDMProcessStereo2Mono(AVDMGenericAudioStream * instream ):
AVDMBufferedAudioStream    (instream)
{
    // nothing special here...
	
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    if(_wavheader->channels==2)
    {
    	_wavheader->channels=1;
    	_wavheader->byterate/=2;
	_length = instream->getLength()/2;
	printf("Stereo2mono: enabled\n");
	_engaged=1;
     }
     else
     	printf("Stereo2mono: disabled, source is not stereo\n");
    _instream->goToTime(0);
    strcpy(_name, "PROC:MONO");
    

};

AVDMProcessStereo2Mono::~AVDMProcessStereo2Mono()
{
    delete _wavheader;

}


uint32_t AVDMProcessStereo2Mono::grab( uint8_t * buffer)
{

int16_t *in,*out;
int32_t o;

	// convert mono to stereo

    uint32_t size,osize;

    size=_wavheader->frequency/20; // 50 ms slice
    size&=0xfffffC; // Quad aligned

    if(!_engaged)
    {
     	if(!(size=_instream->read(size, (uint8_t *)buffer))) return MINUS_ONE;
	return size;    
    }
    if(!(size=_instream->read(size, (uint8_t *)_buffer))) return MINUS_ONE;

    in=( int16_t *)_buffer;
    out=(int16_t *)buffer;
    size>>=2; 	// /4 = in sample
    osize=size<<1;
    for(;size>0;size--)
    	{
		o=*in++;
		o+=*in++;
		o=o/2;
        	*out++=o;
       }
	return osize;

}


