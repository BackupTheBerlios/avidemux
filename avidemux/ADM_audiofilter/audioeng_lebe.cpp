/***************************************************************************
                          audioeng_LEBE.cpp  -  description
                             -------------------

	Swap Little Endian byte order
	Used for playback on sparc/powerpc

    begin                : Fri Jul 16 2002
    copyright            : (C) 2003 by mean
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

AVDMProcessAudio_LEBE::AVDMProcessAudio_LEBE( AVDMGenericAudioStream * instream):
AVDMProcessAudioStream    (instream)
{
    _wavheader = new WAVHeader;
    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    strcpy(_name, "PROC:LEBE");
    _length = instream->getLength();

};


uint32_t AVDMProcessAudio_LEBE::readDecompress(uint32_t len,
					    uint8_t * buffer)
{
	uint8_t  *b=buffer,s;
	uint32_t i=0;

	len=len&0xffffffe;	
	len= _instream->read(len, buffer);
	for(i=0;i<len>>1;i++)
	{
		s=*b;
		*b=*(b+1);
		*(b+1)=s;
		b+=2;
	}

	return len;
}

//
//      We do nothing more that asking the input stream to go at the same place
//
//  if _byteoffset is > 0 we go to offset+byteoffset
// else byteoffset is <0 so it means we duplicate a  [0, byteoffset] seg
//
// if  _newoffset is [0,byteoffset] we go to it
// else we go to newoffset-byteoffset
//___________________________________________

uint8_t AVDMProcessAudio_LEBE::goToTime(uint32_t newoffset)
{
 return goTo(convTime2Offset(newoffset))   ;
 }

uint8_t AVDMProcessAudio_LEBE::goTo(uint32_t newoffset)
{

       	 return _instream->goTo(newoffset);

};


AVDMProcessAudio_LEBE::~AVDMProcessAudio_LEBE()
{
    delete _wavheader;

}

//
//      First we read the input buffer and apply the volume transformation
//  in it.    No need for specific buffer
//___________________________________________
uint32_t AVDMProcessAudio_LEBE::read(uint32_t len, uint8_t * buffer)
{

    return readDecompress(len, buffer);

};
