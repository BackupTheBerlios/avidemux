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
AVDMProcessAudioStream    (instream)
{
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_PCM;
    
    // bug spotted by jmisack
      _wavheader->bitspersample=16;
      
      
    // Case AC3 Downmixing to stereo ...
    if( _wavheader->channels >2)
    		_wavheader->channels =2;
	    _wavheader->byterate =
		_wavheader->channels * _wavheader->frequency * 2;
    strcpy(_name, "PROC:NULL");
    _instream->goToTime(time_offset);
    _start_time = time_offset;
    _size = length;
    _length = length;
    _bufferlen = 0;
    _served = 0;

};

AVDMProcessAudio_Null::~AVDMProcessAudio_Null()
{
    delete(_wavheader);
};

// do nothing
//___________________________________________
uint32_t AVDMProcessAudio_Null::read(uint32_t len, uint8_t * buffer)
{
    uint32_t avail = 0;

    if (_served > _size)
	return 0;
  more:

    // have we already got what's needed ?
    if (_bufferlen >= len)
      {
	drain:
	  memcpy(buffer + avail, _buffer, len);
	  //memmove(_buffer,_buffer+len,_bufferlen-len);
	  //  --workaround
	  uint8_t *des, *src;
	  des = (uint8_t *) (_buffer);
	  src = des + len;
	  for (uint32_t y = (_bufferlen - len); y > 0; y--)
	      *des++ = *src++;

	  // --workaround
	  _bufferlen -= len;
	  avail += len;
	  _served += avail;
	  if (_served > _size)
	    {
		avail = avail + _size;
		avail = avail - _served;
	    }
	  return avail;
      }
    // first empty what was available
    memcpy(buffer + avail, _buffer, _bufferlen);
    avail += _bufferlen;
    len -= _bufferlen;
    // ask for more to the previous stream
    _bufferlen = 0;

    _bufferlen = _instream->readDecompress(len, _buffer);
#if 0
    printf("\n got : %lu asked : %lu",_bufferlen,len);
#endif
    if (_bufferlen == 0)
	return avail;		// we got everything  !

    // check if we reach dangerous level
    if (_bufferlen >= (PROCESS_BUFFER_SIZE * 2 / 3))
      {
	  goto drain;
      }
    goto more;


};
uint32_t AVDMProcessAudio_Null::readDecompress(uint32_t len,
					       uint8_t * buffer)
{
    return read(len, buffer);
}

//
//      We do nothing
//
//___________________________________________

uint8_t AVDMProcessAudio_Null::goToTime(uint32_t newoffset)
{
    if (newoffset == 0)
	_served = 0;
    return _instream->goToTime(newoffset + _start_time);
};






// EOF
