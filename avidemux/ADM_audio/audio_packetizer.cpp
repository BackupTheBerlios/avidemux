/***************************************************************************
                          aviaudio.cpp  -  description
                             -------------------
    begin                : 
    copyright            : (C) 2004 by mean
    email                : fixounet@free.fr

Split a stream into packet(s)

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>
#ifdef CYG_MANGLING
#include <io.h>
#endif

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"
#include "ADM_library/fourcc.h"
#include "ADM_openDML/ADM_openDML.h"
#include "aviaudio.hxx"

#include "ADM_audio/ADM_a52info.h"
#include "ADM_audio/ADM_mp3info.h"

#define MINSTOCK 5000
#define MINUS_ONE 0xffffffff	

//

uint8_t		AVDMGenericAudioStream::getPacket(uint8_t *dest, uint32_t *len, 
						uint32_t *samples)
{
	uint32_t instock=0,rd=0;	

	ADM_assert(_wavheader);
	
	instock=packetTail-packetHead;
	if(instock<MINSTOCK)
	{
		rd=read(MINSTOCK,&packetBuffer[packetTail]);
		if(rd!=MINUS_ONE)
		{
			 instock+=rd;
			 packetTail+=rd;
		}
		else
		{
			printf("**PKTZ:READ ERROR\n");
		}
	}
			
	switch(_wavheader->encoding)
	{
		case WAV_MP2:
		case WAV_MP3:
				return getPacketMP3(dest,len,samples);
				break;
		case WAV_AC3:
				return getPacketAC3(dest,len,samples);
				break;
		default:
				printf("Unsupported!\n");
				return 0;
	}
	return 0;

}
uint8_t		AVDMGenericAudioStream::getPacketAC3(uint8_t *dest, uint32_t *len, 
								uint32_t *samples)
{
	uint32_t instock,rd;
	uint32_t startOffset,endOffset;
	
				
			ADM_assert(_wavheader->encoding==WAV_AC3);
			if(packetTail<packetHead+6)
			{
				printf("PKTZ:Buffer empty\n");
				return 0;
			}
			// It is MP3, read packet
			// synchro start
			int flags,sample_rate,bit_rate;
			int found=0;
			uint32_t start,size=0;
			
			start=packetHead;

			while(start+6<packetTail)
			{
				if(packetBuffer[start]!=0x0b || packetBuffer[start+1]!=0x77)
				{
					printf("No sync for AC3!\n");
					continue;
				}
			
			
				size= ADM_a52_syncinfo (&packetBuffer[start], &flags, 
						&sample_rate, &bit_rate);			
				if(!size)
				{
					printf("OGM: Cannot sync\n");
					continue;
				}
				if(size+6+packetHead<packetTail)
				{
					// Found
					memcpy(dest,&packetBuffer[start],size);
					packetHead=start+size;
					found=1;
					break;
				}
			
			}
			if(!found)
			{
				printf("AC3pak: Cannot find packet\n");
				return 0;
			}
			*len=size;
			*samples=1536;
			
			if(packetTail>SIZE_INTERNAL)
			{
				// Wrap
				memmove(packetBuffer,&packetBuffer[packetHead],packetTail-packetHead);
				packetTail-=packetHead;
				packetHead=0;
			}
			return 1;
}
/*

	Extract a packet from MP3 stream
	We only sync with mpeg audio startcode and frequency ATM
	We should/will compute the min packet size also
		to avoid as much as possible false detection

*/
uint8_t		AVDMGenericAudioStream::getPacketMP3(uint8_t *dest, uint32_t *len, 
								uint32_t *samples)
{
	uint32_t instock,rd;
	uint32_t startOffset,endOffset;
	uint32_t layer;
	uint32_t size;
	MpegAudioInfo mpegInfo,mpegTemp;
			
			ADM_assert(_wavheader->encoding==WAV_MP2||_wavheader->encoding==WAV_MP3);
			if(packetTail<packetHead+4)
			{
				printf("PKTZ:Buffer empty\n");
				return 0;
			}
			// Build template, only fq ATM
			memset(&mpegTemp,0,sizeof(mpegTemp));
			mpegTemp.samplerate=_wavheader->frequency;
			
			// It is MP3, read packet
			// synchro start
			
			if(!getMpegFrameInfo(&(packetBuffer[packetHead]),packetTail-packetHead,
						&mpegInfo,&mpegTemp,&startOffset))
			{
				// Error decoding mpeg
				printf("MPInfo:** CANNOT FIND MPEG START CODE**\n");
				packetHead+=4;
				return 0;
			}
			memcpy(dest,&packetBuffer[packetHead+startOffset],
					mpegInfo.size);
			*len=mpegInfo.size;
			*samples=mpegInfo.samples;
			packetHead+=startOffset+mpegInfo.size;
			if(packetTail>SIZE_INTERNAL)
			{
				// Wrap
				memmove(packetBuffer,&packetBuffer[packetHead],packetTail-packetHead);
				packetTail-=packetHead;
				packetHead=0;
			}
			return 1;
}		
//---
