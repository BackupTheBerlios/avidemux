/***************************************************************************
                          audioeng_normalize.cpp  -  description
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
#include "audioeng_film2pal.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"
// Ctor
//__________

AVDMProcessAudio_Film2Pal::AVDMProcessAudio_Film2Pal(AVDMGenericAudioStream * instream):AVDMBufferedAudioStream
    (instream)
{
    // nothing special here...
    _current=_target=0;
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
    
    d*=25.;
    d/=23.976;
    
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
    uint32_t rd, i,len;
    int16_t *s;
   
    rd = _instream->readDecompress(8192*4, _bufferin);
    
    _current+=rd;
    
    // we dupe as needed (at least 4 bytes at a time to handle stereo case)
    
    
   
    return rd;

};
