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
#include <assert.h>
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
AVDMProcessAudioStream    (instream)
{
    // nothing special here...

    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->channels=2;
    _wavheader->byterate*=2;

    _instream->goToTime(0);
    strcpy(_name, "PROC:MONO");
    _length = instream->getLength()*2;

};

AVDMProcessMono2Stereo::~AVDMProcessMono2Stereo()
{
    delete _wavheader;

}

// builAudiodFilter
//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessMono2Stereo::read(uint32_t len, uint8_t * buffer)
{

    return readDecompress(len, buffer);

};
uint32_t AVDMProcessMono2Stereo::readDecompress(uint32_t len,
						    uint8_t * buffer)
{

	// convert mono to stereo

    uint32_t pos;

    pos=len>>2; // in sample mono

   if(pos> PROCESS_BUFFER_SIZE) pos=    PROCESS_BUFFER_SIZE;


    if(!_instream->read(len>>1, (uint8_t *)_buffer)) return 0;

    uint16_t *in,*out;

    in=( uint16_t *)_buffer;
    out=(uint16_t *)buffer;

    for(;pos>0;pos--)
    	{
        	*(out+1)=*out=*in++;
         	out+=2;
       }
	return len;

}

 uint8_t  AVDMProcessMono2Stereo::goToTime(uint32_t newoffset)
 {  	
       	return _instream->goToTime(newoffset>>1);
   }
