/***************************************************************************
                          ADM_codecac3.cpp  -  description
                             -------------------
    begin                : Fri May 31 2002
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
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"


#ifdef USE_AC3
#include "ADM_audiocodec/ADM_AC3.h"

ADM_AudiocodecAC3::ADM_AudiocodecAC3( uint32_t fourcc) :   ADM_Audiocodec(fourcc)
{
 	ADM_assert(fourcc==WAV_AC3);

}
ADM_AudiocodecAC3::~ADM_AudiocodecAC3( )
{

}
uint8_t ADM_AudiocodecAC3::beginDecompress( void )
{
		return ADM_AC3Init();
}
uint8_t ADM_AudiocodecAC3::endDecompress( void )
{
		return ADM_AC3End();
}
uint8_t ADM_AudiocodecAC3::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
 	return ADM_AC3Run(ptr,nbIn,outptr,nbOut);


}

#endif
