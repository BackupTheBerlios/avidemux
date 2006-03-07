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
#include "ADM_audio/ADM_aacinfo.h"

#define MINSTOCK 5000
#define MINUS_ONE 0xffffffff	

//
uint8_t AVDMGenericAudioStream::flushPacket(void)
{
	packetTail=packetHead=0;
	return 1;
}
uint8_t		AVDMGenericAudioStream::getPacket(uint8_t *dest, uint32_t *len, 
						uint32_t *samples)
{
	uint32_t instock=0,rd=0;	

	ADM_assert(_wavheader);
	shrink();
	instock=packetTail-packetHead;
	
	while(instock<MINSTOCK)
	{
		rd=read(MINSTOCK,&packetBuffer[packetTail]);
		if(rd)
		{
			 instock+=rd;
			 packetTail+=rd;
		}
		else
		{
			printf("**PKTZ:READ ERROR\n");
			break;
		}
	}
	if(!instock)
	{
		printf("Pkt : incoming buffer empty\n");
		return 0;
	}
	switch(_wavheader->encoding)
	{
		case WAV_MP2:
		case WAV_MP3:
				return getPacketMP3(dest,len,samples);
				break;
		case WAV_AAC:
				return getPacketAAC(dest,len,samples);
				break;
		case WAV_LPCM:
		case WAV_PCM:
                case WAV_8BITS_UNSIGNED:
		case WAV_ULAW:
				return getPacketPCM(dest,len,samples);
				break;
		case WAV_AC3:
				return getPacketAC3(dest,len,samples);
				break;
				
		case WAV_WMA:
				return getPacketWMA(dest,len,samples);
				break;
		default:
				printf("Unsupported!\n");
				return 0;
	}
	return 0;

}
uint8_t AVDMGenericAudioStream::shrink( void )
{
	ADM_assert(packetTail>=packetHead);
	if(packetTail>SIZE_INTERNAL)
	{
		// Wrap
		memmove(packetBuffer,&packetBuffer[packetHead],packetTail-packetHead);
		packetTail-=packetHead;
		packetHead=0;
	}
	
	return 1;
}
//___________________________
uint8_t		AVDMGenericAudioStream::getPacketWMA(uint8_t *dest, uint32_t *len, 
								uint32_t *samples)
{
	uint32_t align=_wavheader->blockalign;
	uint32_t avail;
	
	
	avail=packetTail-packetHead;
	
	if(avail>=align)
	{
		//printf("WMA: %lu\n",align);
		memcpy(dest,&packetBuffer[packetHead],align);
		packetHead+=align;
		*samples=1024; 
		#warning FIXME
		*len=align;
		return 1;
	}
	*len=0;
	*samples=0;
	printf("Packetizer wma: no more data\n");
	return 0;

}
//*************
// The packet is 1024 bytes long BUT the 4/8 bytes (mono/stereo) are used for predictor initialization
//
// See Mike Melanson nice doc http://www.multimedia.cx/simpleaudio.html
//___________________________
uint8_t		AVDMGenericAudioStream::getPacketPCM(uint8_t *dest, uint32_t *len, 
								uint32_t *samples)
{
// Take ~ 10 m packets
//
	uint32_t count,sample;
			sample=_wavheader->frequency/100;			
			count=sample*_wavheader->channels;
                        if(_wavheader->encoding!=WAV_ULAW && _wavheader->encoding!=WAV_8BITS_UNSIGNED)
			{
				count*=2;			
			}
			if(packetTail-packetHead<count)
			{
				count=packetTail-packetHead;
				count&=0xffffffC;
			}
			memcpy(dest,&packetBuffer[packetHead],count);			
			packetHead+=count;
			
			// now revert to sample
			sample=count/_wavheader->channels;
			if(!sample)
			{
				packetHead=packetTail=0;
				printf("Wav Packetizer: running empty, last packet sent\n");
				return 0;
			}
                        if(_wavheader->encoding!=WAV_ULAW && _wavheader->encoding!=WAV_8BITS_UNSIGNED&&
                                        _wavheader->encoding!=WAV_IMAADPCM)
			{
				*samples=sample/2;
			}
			else
			{
				*samples=sample;
			}
			*len=count;
			return 1;
}
	
uint8_t		AVDMGenericAudioStream::getPacketAC3(uint8_t *dest, uint32_t *len, 
								uint32_t *samples)
{
	uint32_t instock,rd;
	uint32_t startOffset,endOffset;
	uint8_t  lock=0;
	uint8_t  headerfound=0;
				
			ADM_assert(_wavheader->encoding==WAV_AC3);
			ADM_assert(packetTail>=packetHead);
			if(packetTail<packetHead+6)
			{
				printf("PKTZ:AC3 Buffer empty\n");
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
					if(!lock)
					{
						printf("AC3: searching sync ");
						lock=1;
					}
					else
					{
						printf(".");
					}
					start++;
					continue;
				}
			
			
				size= ADM_a52_syncinfo (&packetBuffer[start], &flags, 
						&sample_rate, &bit_rate);			
				if(!size)
				{
					printf("AC3: Cannot sync\n");
					start++;
					continue;
				}
				if(size+packetHead<=packetTail)
				{
					// Found
					memcpy(dest,&packetBuffer[start],size);
					packetHead=start+size;
					found=1;
					break;
				}
				else
				{	// not enought data left
					headerfound=1;
					printf("AC3Pkt: Need %lu have:%lu\n",size,packetTail-packetHead);
					break;
				}
			
			}
			if(!found)
			{
				printf("AC3pak: Cannot find packet (%lu)\n",packetTail-packetHead);

				if(!headerfound)
				{

					// no header found, we can skip up to the 6 last bytes
					uint32_t left;
					left=packetTail-packetHead;
					if(left>6) left=6;
					packetHead=packetTail-left;

					printf("AC3Pak: No ac3 header found, purging buffer (%lu - %lu)\n",packetHead,packetTail);
				}
				return 0;
			}
			*len=size;
			*samples=1536;
			
			
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
				printf("PKTZ: MP3Buffer empty:%lu / %lu\n",packetHead,packetTail);
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
				packetHead+=1;
				return 0;
			}
			if(packetHead+startOffset+mpegInfo.size>packetTail)
			{
				printf("MP3 packetizer: not enough data (start %lu needs %lu, available %lu)\n"
						,startOffset,mpegInfo.size,packetTail-packetHead);
				packetHead+=1;
				return 0;
			}
			memcpy(dest,&packetBuffer[packetHead+startOffset],
					mpegInfo.size);
			*len=mpegInfo.size;
			*samples=mpegInfo.samples;
			packetHead+=startOffset+mpegInfo.size;
			return 1;
}		
//---
uint8_t		AVDMGenericAudioStream::getPacketAAC(uint8_t *dest, uint32_t *len, 
								uint32_t *samples)
{
	uint32_t instock,rd;
	uint32_t startOffset,endOffset;
	AacAudioInfo mpegInfo,mpegTemp;
	
				
			ADM_assert(_wavheader->encoding==WAV_AAC);
			if(packetTail<packetHead+8)
			{
				printf("PKTZ:Buffer empty\n");
				return 0;
			}
			if(!getAACFrameInfo(&(packetBuffer[packetHead]),packetTail-packetHead,
						&mpegInfo,&mpegTemp,&startOffset))
			{
				// Error decoding mpeg
				printf("AACInfo:** CANNOT FIND AAC/ADTS START CODE**\n");
				packetHead+=8;
				return 0;
			}
			if(packetHead+startOffset+mpegInfo.size>packetTail)
			{
				printf("AAC packetizer: not enough data\n");
				return 0;
			}
			memcpy(dest,&packetBuffer[packetHead+startOffset],
					mpegInfo.size);
			*len=mpegInfo.size;
			*samples=mpegInfo.samples;
			packetHead+=startOffset+mpegInfo.size;

			return 1;
}
//
