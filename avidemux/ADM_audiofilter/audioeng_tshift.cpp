/***************************************************************************
                          audioeng_tshift.cpp  -  description
                             -------------------
	This filter add a positive or negative time offset to the audio stream


    begin                : Mon Jan 21 2002
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
// Ctor       `AVDMProcessAudioStream::AVDMProcessAudioStream(AVD
//__________

AVDMProcessAudio_TimeShift::AVDMProcessAudio_TimeShift(AVDMGenericAudioStream * instream, int32_t msoff):AVDMProcessAudioStream
    (instream)
{
    // nothing special here...

    _timeoffset = msoff;
    if (msoff > 0)
    {
	_byteoffset = _instream->convTime2Offset(msoff);
	_byteoffset=_byteoffset & 0xfffffffc;      // jakub bug 2
	}
    else
    {
	_byteoffset = _instream->convTime2Offset(-msoff);
	_byteoffset=_byteoffset & 0xfffffffc;      // jakub bug 2
	_byteoffset=-_byteoffset;
	}
	
  printf("\n %ld byteoffset",_byteoffset);
	
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _instream->goToTime(0);
    strcpy(_name, "PROC:TSHI");
    _length = instream->getLength();
    _outpos = -_byteoffset;

};

AVDMProcessAudio_TimeShift::~AVDMProcessAudio_TimeShift()
{
    delete _wavheader;

}

//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessAudio_TimeShift::read(uint32_t len, uint8_t * buffer)
{

    return readDecompress(len, buffer);

};
uint32_t AVDMProcessAudio_TimeShift::readDecompress(uint32_t len,
						    uint8_t * buffer)
{

    uint32_t pos;

    if (_outpos == 0)
	return _instream->read(len, buffer);
    if (_outpos < 0)		// we asked for delay -> Fill 0 at the beginning
      {
	  pos = (uint32_t) (-_outpos);
	  if (len > (pos))	// asked more than what we got
	    {
		memset(buffer, 0, pos);
		_outpos = 0;
		return (pos + readDecompress(len - pos, buffer + pos));
	  } else		// we got enougth blanks
	    {
		memset(buffer, 0, len);
		_outpos += len;
		return len;
	    }
      }
    // if we get here positive offset, we read ahead
    uint8_t buf[4 * 1024];
    uint32_t ask;
    while (_outpos > 0)
      {
	  if (_outpos > 4 * 1024)
	      ask = 4 * 1024;
	  else
	      ask = _outpos;
	  _instream->read(ask, buf);
	  _outpos -= ask;
      }
    ADM_assert(_outpos == 0);
    return _instream->read(len, buffer);


}

//
//      We do nothing more that asking the input stream to go at the same place 
//
//___________________________________________

uint8_t AVDMProcessAudio_TimeShift::goToTime(uint32_t newoffset)
{
    // reset time offset
    if (newoffset == 0)
      {
	  _outpos = -_byteoffset;
      }
    return _instream->goToTime(newoffset);
};
