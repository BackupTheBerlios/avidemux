/***************************************************************************
                          audioeng_null.cpp  -  description
                             -------------------
	This audio plug in does nothing at all.
 	except that you are sure that the output is WAV PCM  with the given size

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
//#include "toolkit.hxx"

// Ctor: Duplicate
//__________
    
    
AVDMProcessAudio_Null::AVDMProcessAudio_Null
(AVDMGenericAudioStream * instream, uint32_t time_offset, uint32_t length):
AVDMBufferedAudioStream    (instream)
{
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    
    // bug spotted by jmisack
      _wavheader->bitspersample=16;
      
      
    // Case AC3 Downmixing to stereo ...
    if( _wavheader->channels >2)
    		_wavheader->channels =2;
    _wavheader->byterate =_wavheader->channels * _wavheader->frequency * 2;
    strcpy(_name, "PROC:NULL");
    _instream->goToTime(time_offset);
    _start_time = time_offset;
    _size = length;
    _length = length;

};

AVDMProcessAudio_Null::~AVDMProcessAudio_Null()
{
    delete(_wavheader);
};
uint32_t 	AVDMProcessAudio_Null::grab(uint8_t *obuffer)
{
uint32_t rdall=0;
  rdall = _instream->readDecompress(1000, obuffer);
  if (rdall == 0)
	      return MINUS_ONE;	// we could not get a single byte ! End of stream
  return rdall;
}
uint8_t  AVDMProcessAudio_Null::goToTime(uint32_t newoffset)
{
	 _instream->goToTime(_start_time);
	 if(newoffset)
	 {
	 	printf("AFNull: MMm Null not going to 0 ?\n");
	 }
	return 1;
}

// EOF
