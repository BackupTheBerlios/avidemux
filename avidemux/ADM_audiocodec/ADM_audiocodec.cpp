/***************************************************************************
                          ADM_audiocodec.cpp  -  description
                             -------------------
    begin                : Sat Jun 1 2002
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
#include <assert.h>
#include <math.h>

#include "config.h"

#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"


ADM_Audiocodec	*getAudioCodec(uint32_t fourcc,WAVHeader *info,uint32_t extra,uint8_t *extraData)
{
ADM_Audiocodec *out;

		// fake codec for 8 bits
		if(fourcc==WAV_PCM)
			{
				if(info)
					if(info->bitspersample==8)
						fourcc=WAV_8BITS_UNSIGNED;
			
			}
 		switch(fourcc)
   			{
				case WAV_PCM:
    					printf("\n Audio codec:  WAV\n");
    					out= (ADM_Audiocodec *)new ADM_AudiocodecWav(fourcc);
                  			break;
				case WAV_8BITS:
					printf("\n 8 BIts pseudo codec\n");
    					out= (ADM_Audiocodec *)new ADM_Audiocodec8Bits(fourcc);
					break;
				case WAV_8BITS_UNSIGNED:
					printf("\n 8 BIts pseudo codec unsigned\n");
    					out= (ADM_Audiocodec *)new ADM_Audiocodec8Bits(fourcc);
					break;		
		  		case WAV_LPCM:
    					printf("\n Audio codec:  LPCM swapped\n");
    					out= (ADM_Audiocodec *)new ADM_AudiocodecWavSwapped(fourcc);
                  		break;
#ifdef USE_AC3
				case WAV_AC3:
        				printf("\n Audio codec:  AC3\n");

					out= (ADM_Audiocodec *) new ADM_AudiocodecAC3(fourcc);
                  break;
#endif
#ifdef USE_MP3
				case WAV_MP3:
				case WAV_MP2:
        				printf("\n Audio codec:  MP2-3\n");

					out= (ADM_Audiocodec *) new ADM_AudiocodecMP3(fourcc);
                  break;
#endif
#ifdef USE_VORBIS
				case WAV_OGG:
						printf("\n Ogg/Vorbis \n");
						out= (ADM_Audiocodec *) new ADM_vorbis(fourcc,info,
								extra,extraData);
						break;
#endif		

              	case WAV_WMA:
        				printf("\n Audio codec:  ffWMA\n");
				
					out= (ADM_Audiocodec *) new ADM_AudiocodecWMA(fourcc,info,extra,extraData);
                  break;
		  
            	default:
             			printf("\n Unknown codec : %lu",fourcc);
             			out= (ADM_Audiocodec *) new ADM_AudiocodecUnknown(fourcc);
        	}

         return out;


}

ADM_Audiocodec::ADM_Audiocodec( uint32_t fourcc ) {
    UNUSED_ARG(fourcc);
	_running=0;
	_init=0;
}

ADM_Audiocodec::~ADM_Audiocodec()
{}

uint8_t ADM_Audiocodec::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut){
    UNUSED_ARG(ptr);
    UNUSED_ARG(nbIn);
    UNUSED_ARG(outptr);
    UNUSED_ARG(nbOut);
	return 0;
}


ADM_AudiocodecUnknown::~ADM_AudiocodecUnknown(){
}

uint8_t ADM_AudiocodecUnknown::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut){
    UNUSED_ARG(ptr);
    UNUSED_ARG(nbIn);
    UNUSED_ARG(outptr);
    UNUSED_ARG(nbOut);
	return 0;
}
