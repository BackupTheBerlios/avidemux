/***************************************************************************
                          audioenf_lpcm.cpp  -  description
                             -------------------
                Lpcm codec (so simple)

    copyright            : (C) 2005 by mean
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

#include "default.h"
#include "audioprocess.hxx"
#include "audioeng_lpcm.h"
// Ctor: Duplicate
//__________

AVDMProcessAudio_Lpcm::AVDMProcessAudio_Lpcm(AVDMGenericAudioStream * instream):AVDMBufferedAudioStream (instream)
{
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_LPCM;
    strcpy(_name, "PROC:LPCM");
    _instream->goToTime(0);     // rewind
    _length = _instream->getLength();
};

void AVDMProcessAudio_Lpcm::init()
{
printf("LPCM init\n");
	_chunk = 4096;
	_in = new float [_chunk];
	ADM_assert(_in);
}


AVDMProcessAudio_Lpcm::~AVDMProcessAudio_Lpcm()
{
    delete(_wavheader);
};


//_____________________________________________
uint32_t AVDMProcessAudio_Lpcm::grab(uint8_t * obuffer)
{
 	uint8_t *in,*out;

	if (readChunk() == 0)
		return MINUS_ONE;	// End of stream

	dither16bit();

	in = (uint8_t *)_in;
	out = obuffer;
	for (int i =0 ; i < _chunk; i++) {
		out[1] = in[0];
		out[0] = in[1];
		in += 2;
		out += 2;
	}

	return _chunk*2;
}
