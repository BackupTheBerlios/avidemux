/***************************************************************************
                          ADM_codecmp3.cpp  -  description
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
//#include <sstream>
#include <assert.h>
#include <math.h>

#include "config.h"
#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"

#ifdef USE_MP3
#include "ADM_audiocodec/avdm_mad.h"

ADM_AudiocodecMP3::ADM_AudiocodecMP3( uint32_t fourcc) :   ADM_Audiocodec(fourcc)
{
 	if((fourcc!=WAV_MP3) && (fourcc!=WAV_MP2))
  		assert(0);
	if(fourcc==WAV_MP2) printf("Mpeg1/2 audio codec created\n");

}
ADM_AudiocodecMP3::~ADM_AudiocodecMP3( )
{

}
uint8_t ADM_AudiocodecMP3::beginDecompress( void )
{
		return AVDM_MadInit();
}
uint8_t ADM_AudiocodecMP3::endDecompress( void )
{
		return AVDM_MadEnd();
}
uint8_t ADM_AudiocodecMP3::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
 	return AVDM_MadRun(ptr,nbIn,outptr,nbOut);


}

#endif
