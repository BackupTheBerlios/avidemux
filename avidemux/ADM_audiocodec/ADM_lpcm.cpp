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
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"

ADM_AudiocodecWavSwapped::ADM_AudiocodecWavSwapped( uint32_t fourcc ) : ADM_Audiocodec(fourcc)
{
		_instore=0;
}
ADM_AudiocodecWavSwapped::~ADM_AudiocodecWavSwapped()
{

}

void ADM_AudiocodecWavSwapped::purge()
{
	_instore=0;
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
uint8_t ADM_AudiocodecWavSwapped::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
		*nbOut=0;
            // if we have instored ...
	    	if((_instore+nbIn)<2) return 1;
		if(_instore)
		{
			*(outptr+1)=_hold;;
			*(outptr)=*ptr++;
			*nbOut+=2;
			nbIn--;
			_instore=0;
			outptr+=2;
		}
		_instore=0;
		if((nbIn&1))
			{
				_hold=*(ptr+nbIn-1);
				_instore=1;
				nbIn--;
			}

			uint8_t *i1;
			i1=ptr;
			*nbOut+=nbIn;
			for(uint32_t i=nbIn>>1;i>0;i--)
			{
				*(outptr+1)=*(i1);
				*(outptr)=*(i1+1);
				outptr+=2;
				i1+=2;
			}

			return 1;
}

