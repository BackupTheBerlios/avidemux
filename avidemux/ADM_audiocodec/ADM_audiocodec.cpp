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
#include <ADM_assert.h>
#include <math.h>

#include "config.h"

#include "fourcc.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "ADM_audiofilter/audiofilter_channel_route.h"
#include "ADM_libraries/ADM_libwrapper/libwrapper_global.h"

ADM_Audiocodec	*getAudioCodec(uint32_t fourcc,WAVHeader *info,uint32_t extra,uint8_t *extraData)
{
ADM_Audiocodec *out = NULL;

		// fake codec for 8 bits
		if(fourcc==WAV_PCM)
			{
				if(info)
					if(info->bitspersample==8)
                                        {
                                                info->encoding=fourcc=WAV_8BITS_UNSIGNED;
                                        }
			
			}
 		switch(fourcc)
   			{
                                case WAV_IMAADPCM:
                                        printf("\n Audio codec:  IMA MS ADPCM\n");
                                        out= (ADM_Audiocodec *)new ADM_AudiocodecImaAdpcm(fourcc,info);
                                        break;
				case WAV_PCM:
    					printf("\n Audio codec:  WAV\n");
#ifdef ADM_BIG_ENDIAN				
    					out= (ADM_Audiocodec *)new ADM_AudiocodecWavSwapped(fourcc);
#else
					out= (ADM_Audiocodec *)new ADM_AudiocodecWav(fourcc);
#endif					
                  			break;
#ifdef USE_AMR_NB
                                case WAV_AMRNB:
                                        printf("\n AMR narrow band codec\n");
                                        out=(ADM_Audiocodec *)new ADM_AudiocodecAMR(fourcc,info);
                                        break;
#endif
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
#ifndef ADM_BIG_ENDIAN				
    					out= (ADM_Audiocodec *)new ADM_AudiocodecWavSwapped(fourcc);
#else
					out= (ADM_Audiocodec *)new ADM_AudiocodecWav(fourcc);
#endif					
                  		break;
#ifdef USE_AC3
				case WAV_AC3:
        				printf("\n Audio codec:  AC3\n");

					out= (ADM_Audiocodec *) new ADM_AudiocodecAC3(fourcc, info);
                  break;
#endif
#ifdef USE_LIBDCA
                case WAV_DTS:
					if (dca->isAvialable())
					{
						printf("\n Audio codec:  DTS\n");
						out= (ADM_Audiocodec *) new ADM_AudiocodecDCA(fourcc, info);						
					}

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
#ifdef USE_FAAD
                                case WAV_AAC_HE:
						printf("\n MP4 audio (HE)\n");
						out= (ADM_Audiocodec *) new ADM_faad(fourcc,1,info,
								extra,extraData);
						break;

				case WAV_AAC:
				case WAV_MP4:
                                case 0x706D:  // Weird fourcc/id code used by mencoder
						printf("\n MP4 audio \n");
						out= (ADM_Audiocodec *) new ADM_faad(fourcc,0,info,
								extra,extraData);
						break;
#endif
				case WAV_ULAW:
						printf("\n ULAW codec\n");
						out=(ADM_Audiocodec *) new ADM_AudiocodecUlaw(fourcc,info);
						break;
            case WAV_WMA:
                printf("\n Audio codec:  ffWMA\n");
                out= (ADM_Audiocodec *) new ADM_AudiocodecWMA(fourcc,info,extra,extraData);
                break;
            case WAV_QDM2:
                printf("\n Audio codec:  ffQDM2\n");
                out= (ADM_Audiocodec *) new ADM_AudiocodecWMA(fourcc,info,extra,extraData);
                break;
        	}

	if (out == NULL)
	{
		printf("\n Unknown codec : %lu",fourcc);
		out = (ADM_Audiocodec *) new ADM_AudiocodecUnknown(fourcc);
	}

	ch_route.input_type[0] = CH_FRONT_LEFT;
	ch_route.input_type[1] = CH_FRONT_RIGHT;

	return out;
}

ADM_Audiocodec::ADM_Audiocodec( uint32_t fourcc ) {
    UNUSED_ARG(fourcc);
	_init=0;
}

ADM_Audiocodec::~ADM_Audiocodec()
{}

