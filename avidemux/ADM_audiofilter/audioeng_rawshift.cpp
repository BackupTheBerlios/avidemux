/***************************************************************************
                          audioeng_rawshift.cpp  -  description
                             -------------------

 	This filter i used only in copy mode
			     
    begin                : Fri Jun 28 2002
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

AVDMProcessAudio_RawShift::AVDMProcessAudio_RawShift(AVDMGenericAudioStream * instream, int32_t msoff,
	uint32_t starttime):
AVDMProcessAudioStream    (instream)
{
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    

    // first go to the begining....
	_starttime=starttime;
 	_msoff=-msoff;
	_filterOn=0;
	_sampleOffset=0;

    	if (_msoff > 0) // just seek in the file
   	{
      		_instream->goToTime(  _starttime+_msoff);
		_filterOn=0; // no need for a filter
  	}
    	else
	{
		// need to dupe the beginning
		// if there is enough audio
		if(_starttime>=-_msoff)
		{
			_instream->goToTime(  _starttime+_msoff); // just rewind to compensate
			_filterOn=0; // no need for a filter
		}
		else
		{
			// we have to dupe a bit
			int32_t dupe;
			_instream->goToTime(0);
			_filterOn=1;
			dupe=-(_starttime+_msoff);
			_sampleOffset=(dupe*_wavheader->frequency)/1000;
		}

  	}


    strcpy(_name, "PROC:RAWS");
    _length = instream->getLength();
    printf("\n Raw shit : Start:%lu ms, Sample offset = %lu\n",_starttime,_sampleOffset);


};
//
//	If filterOn, it means we have to dupe sampleOffset sample, going back to start
//		and do it again
//
uint8_t	 AVDMProcessAudio_RawShift::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
	uint8_t r;
		
		if(!_filterOn)
			return _instream->getPacket(dest,len,samples);

		// filter is still on
		r=_instream->getPacket(dest,len,samples);
		if(!r)
		{
			printf("RawShift: Readerror\n");
			*len=0;
			*samples=0;
		}
		if(_sampleOffset<=*samples)
		{
			printf("RawShift : End of prebuffering\n");
			_filterOn=0;
			_instream->goToTime(_starttime);
		}
		else
		{
			_sampleOffset-=*samples;
			//printf("%lu to go\n",_sampleOffset);
		}
		return r;

}
//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessAudio_RawShift::read(uint32_t len, uint8_t * buffer)
{

    return readDecompress(len, buffer);

};
uint32_t AVDMProcessAudio_RawShift::readDecompress(uint32_t len,
						    uint8_t * buffer)
{

  	uint32_t l,sam;
		if(!getPacket(buffer,&len,&sam))
			return 0;
		return len;

}


//___________________________________________

uint8_t AVDMProcessAudio_RawShift::goToTime(uint32_t newoffset)
{
 	// since we are in copy mode, the only value accepted is 0 here
	ADM_assert(!newoffset);
	return 1;
}

uint8_t AVDMProcessAudio_RawShift::goTo(uint32_t newoffset)
{

	// since we are in copy mode, the only value accepted is 0 here
	ADM_assert(!newoffset);
	return 1;
};


AVDMProcessAudio_RawShift::~AVDMProcessAudio_RawShift()
{
    delete _wavheader;

}


