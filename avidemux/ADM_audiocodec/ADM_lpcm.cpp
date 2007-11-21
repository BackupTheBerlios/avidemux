/***************************************************************************
                          ADM_lpcm .cpp  -  description
                             -------------------

	Handle DVD LPCM, i.e. swapped PC PCM
    begin                : Fri May 5 2003
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
//#include <sstream>
#include "ADM_assert.h"
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "ADM_audiofilter/audiofilter_channel_route.h"

ADM_AudiocodecWavSwapped::ADM_AudiocodecWavSwapped( uint32_t fourcc ) : ADM_Audiocodec(fourcc)
{
	ch_route.input_type[0] = CH_FRONT_LEFT;
	ch_route.input_type[1] = CH_FRONT_RIGHT;
	ch_route.input_type[2] = CH_FRONT_CENTER;
	ch_route.input_type[3] = CH_LFE;
	ch_route.input_type[4] = CH_REAR_LEFT;
	ch_route.input_type[5] = CH_REAR_RIGHT;
}

ADM_AudiocodecWavSwapped::~ADM_AudiocodecWavSwapped()
{

}

uint8_t ADM_AudiocodecWavSwapped::beginDecompress()
{
         return 1;
}
uint8_t ADM_AudiocodecWavSwapped::endDecompress()
{
       return 1;
}

uint8_t ADM_AudiocodecWavSwapped::isCompressed( void )
{
 	return 1;
}

uint8_t ADM_AudiocodecWavSwapped::isDecompressable( void )
{
 	return 1;
}

uint8_t ADM_AudiocodecWavSwapped::run(uint8_t * inptr, uint32_t nbIn, float *outptr, uint32_t * nbOut)
{
	if (nbIn < 2)
		return 1;

	if (nbIn&1) {
		printf("Error: nbIn (%i) odd in lpcm", nbIn);
		abort();
	}

	int16_t sample;
	*nbOut=nbIn / 2;
	for (int i = 0; i < *nbOut; i++) {
		sample = (*inptr << 8 | *(inptr+1));
		*(outptr++) = (float)sample / 32768;
		inptr += 2;
	}

	return 1;
}

