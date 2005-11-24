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
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"
// Ctor
//__________

AVDMProcessAudio_Normalize::AVDMProcessAudio_Normalize(AVDMGenericAudioStream * instream):AVDMProcessAudioStream
    (instream)
{
    // nothing special here...
    _scanned = 0;
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    _wavheader->byterate =
	_wavheader->channels * _wavheader->frequency * 2;
    _instream->goToTime(0);
    strcpy(_name, "PROC:NORM");
    _length = instream->getLength();

};
  AVDMProcessAudio_Normalize::~AVDMProcessAudio_Normalize()
  {
     	delete _wavheader;

    }
//
// For normalize, we scan the input stream
// to check for maximum value
//___________________________________________
uint8_t AVDMProcessAudio_Normalize::preprocess(void)
{
    int16_t min = 0, max = 0;
    int16_t *ptr;
    uint32_t scanned = 0, ch = 0;
    _ratio = 0;

    uint32_t percent=0;
    uint32_t current=0,llength=0;
    // uint8_t warned=0;

#define BF 2*2*20*1024
    uint8_t buffer[2 * BF];
    // Go to the beginning...
    //
    _instream->goToTime(0);

    llength=_instream->getLength();
    percent=llength/100;

    printf("\n Seeking for maximum value, that can take a while\n");

	DIA_working *windowWorking=new DIA_working("Normalize : Scanning");;

    // For unknow length just give a big number
    while (1)
      {
      	  if(scanned>llength)
	  	break;
	  ch = _instream->readDecompress(BF, buffer);
	  if (!ch)
	      break;

	  scanned += ch;
	  current+=ch;
	  if(current>percent)
	  {
	  			if(scanned<llength)
					windowWorking->update(scanned,llength);
				if(!windowWorking->isAlive() )
					{
						// cannot be aborted
						delete windowWorking;
						windowWorking=new DIA_working("Normalize : Scanning");;
						if(scanned<llength)
							windowWorking->update(scanned,llength);
					}
		}
	  ptr = (int16_t *) buffer;
	  for (uint32_t i = 0; i < (ch >> 1); i++)
	    {
		if (*ptr > max)
		    max = *ptr;
		if (*ptr < min)
		    min = *ptr;
		ptr++;
	    }

      }
    delete windowWorking;
    printf("\n min %d, max: %d (%lu)\n", min, max,scanned);
    min = -min;
    if (min > max)
	max = min;
	

    // rewind at the beginning
    _instream->goToTime(0);
    printf("\n maximum found : %d\n", max);
    double db_in, db_out;
    db_out =  -3;
    db_in = max;
    db_in = db_in / 32767;
    if (db_in)
	db_in = 20 / log(10) * log(db_in);
    else
	db_in = 0;

    printf("--> %2.2f db / %2.2f \n", db_in, db_out);

    // search ratio
    if(max)
    {

    _ratio = expf(db_out * (1.0 / (20.0 / logf(10.0)))) * 32767;
    _ratio = _ratio / max;

  //  printf("\n Using ratio of : %u\n", _ratio);
     }
//skipit:
    _scanned = 1;
    memset(_dither_prior, MAX_AUDIO_CH, 0);

    strcpy(_name, "PROC:NORM");
    // Go to the beginning...
    //
    _instream->goToTime(0);

    return 0;
}

//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessAudio_Normalize::read(uint32_t len, uint8_t * buffer)
{
    uint32_t rd, i, j,rd2;
    int16_t *s;
    float f, dn;

    ADM_assert(_scanned);
    ADM_assert(_wavheader->channels <= MAX_AUDIO_CH);
    rd = _instream->readDecompress(len, buffer);
    s = (int16_t *) buffer;
    rd2 = rd / 2 / _wavheader->channels;
    for (i = 0; i < rd2; i++)
	for (j = 0; j < _wavheader->channels; j++)
      	{
		// Triangle dithering
		dn = (rand() / (float)RAND_MAX) * 2.0 - 1.0;
		f = (float)*s;
		f *= _ratio;
	  	f += dn - _dither_prior[j];
	  	*s = (int16_t) f;
	  	s++;
		_dither_prior[j] = dn;
      	}

    return rd;

};
uint32_t AVDMProcessAudio_Normalize::readDecompress(uint32_t len,
						    uint8_t * buffer)
{
    return read(len, buffer);
}

//
//      We do nothing more that asking the input stream to go at the same place 
//
//___________________________________________

uint8_t AVDMProcessAudio_Normalize::goToTime(uint32_t newoffset)
{
    return _instream->goToTime(newoffset);
};
