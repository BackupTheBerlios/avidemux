/***************************************************************************
                          ADM_8bits.cpp  -  description
                             -------------------
    begin                : Fri Aug 5 2003
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr

	This is a dummy coded that converts a 8 bits input sample to
	a 16 bits ouput samples

	It is used as it is because internally avidemux can only deal with
	16 bits audio samples

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
#include <assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"


ADM_Audiocodec8Bits::ADM_Audiocodec8Bits( uint32_t fourcc) :   ADM_Audiocodec(fourcc)
{
 	assert(fourcc==WAV_8BITS||fourcc==WAV_8BITS_UNSIGNED);
	if(fourcc==WAV_8BITS_UNSIGNED)
			_unsign=1;
	else
			_unsign=0;

}
ADM_Audiocodec8Bits::~ADM_Audiocodec8Bits( )
{

}
uint8_t ADM_Audiocodec8Bits::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
	uint32_t n=0;
	int16_t o;
	int16_t *pout;
	int8_t *pin;

	pout=(int16_t *)outptr;
	pin=(int8_t *)ptr;

	for(n=nbIn;n>0;n--)
	{
		if(_unsign)
			o=(*pin-128);
		else
			o=*pin;			
		o*=256;
		*pout=o;
		pin++;
		pout++;
	}
	*nbOut=nbIn<<1;
	return 1;

}


