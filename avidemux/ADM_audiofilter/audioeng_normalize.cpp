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
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"
// Ctor
//__________

AVDMProcessAudio_Normalize::AVDMProcessAudio_Normalize(AVDMGenericAudioStream * instream):AVDMProcessAudioStream
    (instream)
{
    // nothing special here...
    _scanned = _max = 0;
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
    int16_t min = 0, max = 0, nratio;
    int16_t *ptr;
    uint32_t rd = 0, scanned = 0, ch = 0, ratio = 0;

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
	  ch = _instream->readDecompress(BF, buffer);
	  if (!ch)
	      break;

	  scanned += ch;
	  current+=ch;
	  if(current>percent)
	  {
				windowWorking->update(scanned,llength);
				if(!windowWorking->isAlive() )
					{
						// cannot be aborted
						delete windowWorking;
						windowWorking=new DIA_working("Normalize : Scanning");;
						windowWorking->update(scanned,llength);
					}
		}
	  rd += ch;
	  ptr = (int16_t *) buffer;
	  for (uint32_t i = 0; i < (ch >> 1); i++)
	    {
		if (*ptr > max)
		    max = *ptr;
		if (*ptr < min)
		    min = *ptr;

	    }

      }
    delete windowWorking;
    printf("\n min %d, max: %d\n", min, max);
    min = -min;
    if (min > max)
	max = min;
	

    // rewind at the beginning
    _instream->goToTime(0);
    printf("\n maximum found : %d\n", max);
    double db;
    db = max;
    db = db / 32767;
    if (db)
	db = 10 * log(db) / log(10);
    else
	db = 0;

    printf("--> %2.2f db / %2.2f \n", db, 10 * log(0.5) / log(10));
    // Now we build the conversion table
    // in from -max to max
    // out from -32768 to 32768
    int16_t j;
    memset(_table, 65535, 0);
    // we search the value righr about by
    // step of 256 to cover round up error
    _max = (max + 255) & 0xffffff00;

    // search ratio
    if(max)
    {
    ratio = 32768 / _max;
    ratio = (int16_t) floor(ratio);
    double fratio;
    
    fratio=ratio/2.;
    nratio = (uint16_t) (ratio/2);
    printf("\n Using ratio of : %u\n", nratio);
    if (_max)
      {

	  for (uint32_t i = 0; i < 32768; i++)
	    {
		db = i;
		db *= fratio;
		// Clipping ?
		if (db > 32767.)
		  {
		      //assert(0);
		      db = 32767.;
		  }
		j = (int16_t) floor(db);
		_table[32768 + i] = j;
		_table[32768 - i] = -j;
		
//		printf("%d -> %d\n",i,j);

	    }
      }
     }
//skipit:
    _max = max;
    _scanned = 1;
    strcpy(_name, "PROC:NORM");
    printf("\n table build");
    return 0;
}

//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessAudio_Normalize::read(uint32_t len, uint8_t * buffer)
{
    uint32_t rd, i;
    int16_t *s;
    assert(_scanned);
    rd = _instream->readDecompress(len, buffer);
    s = (int16_t *) buffer;
    for (i = 0; i < (rd >> 1); i++)
      {
	  *s = _table[32768 + *s];
	  s++;
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
