/***************************************************************************
                          audioeng_6dbpress.cpp  -  description
                             -------------------
		Strongly inspired by flask mpeg (even ripped)
		--> Removed, replaced -6 Db compressor (linear)
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
// Ctor
//__________


AVDMProcessAudio_Compress::AVDMProcessAudio_Compress(AVDMGenericAudioStream * instream):AVDMBufferedAudioStream
    (instream)
{
#define AMP 4
    // nothing special here...
    double ramp, coef, off;

    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    _wavheader->byterate =
	_wavheader->channels * _wavheader->frequency * 2;
    _instream->goToTime(0);
//
// The base is 100 ms
//

    strcpy(_name, "PROC:PRESS");
    // build linear table
    coef = AMP - 1.;
    coef = coef / 32768.;
    off = AMP;
    _table[0]=0;
    for (uint32_t i = 1; i <= 32768; i++)
      {
	  // Press:
	  /*
	  ramp = i;
	  ramp = ramp * (off - coef * ramp);
	  ramp += 0.5;
	  */
	  ramp=32768./i;
	  ramp=pow(10,ramp)/10.;
	  ramp=4-3/ramp;
	  ramp=ramp*i;
	  
	  
	  _table[i + 32768] = (int16_t) floor(ramp);
	  _table[32768 - i] = -(int16_t) floor(ramp);
	  
  	/*  _table[i + 32768] =i;
	  _table[32768 - i] = -i;*/

//	  printf("\n %lu -> %d", i, _table[32768 + i]);
      }


};

//
AVDMProcessAudio_Compress::~AVDMProcessAudio_Compress()
{
 	delete _wavheader;
}


//_____________________________________________
uint32_t AVDMProcessAudio_Compress::grab(uint8_t * _outbuff)
{

    uint32_t rdall = 0;
    int16_t *s, *o;

    rdall = _instream->read(1000, (uint8_t *) (_buffer));
    //printf("\n grabbing :%lu-->%lu\n",ms100,rdall);               
    // Block not filled
    if (rdall != 1000)
      {
	  printf("\n not enough...\n");
	  if (rdall == 0)
	      return MINUS_ONE;	// we could not get a single byte ! End of stream
	  // Else fillout with 0
      }
    s = (int16_t *) _buffer;
    o = (int16_t *) _outbuff;
    for (uint32_t i = 0; i < (rdall >> 1); i++)
      {
	  *o = _table[32768 + *s];
	  s++;
	  o++;
      }
    rdall=(rdall >>1)<<1; // to be sure it is even ...
    return rdall;

}
