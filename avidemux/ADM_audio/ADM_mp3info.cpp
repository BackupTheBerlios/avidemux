//
// C++ Implementation: ADM_MP3Info
//
// Description: 
//		Decode an mp3 frame an fill the info field
//			The second is a template to check we do not do bogus frame detection
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//	
//
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


#include "ADM_library/default.h"
#include "aviaudio.hxx"
#include "ADM_audio/ADM_mp3info.h"

static  uint32_t MP3Fq[4] = { 44100, 48000, 32000, 0 };       
static uint32_t MP2Fq[4] = { 22050, 24000, 16000, 0 };       
static uint32_t Bitrate[8][16]=
{
	// Level 1 / Layer 0
	{0,0,0,0,0,0,0,0   ,0,0,0,0,0,0,0,0},
	// Level 1/ Layer 1
	{0,32,64,96,128,160,192,224   ,256,288,320,352,384,416,448,0},
	// Level 1/Layer 2
	{ 0, 32, 48, 56,64, 80, 96, 112,     128, 160, 192, 224,   256, 320, 384, 0},
	//Level 1 / Layer 3
	{ 0, 32, 40, 48, 56, 64, 80, 96,         112, 128, 160, 192,  224, 256, 320, 0},
	// Level 2/Layer 0
	{0,0,0,0,0,0,0,0   ,0,0,0,0,0,0,0,0},
	// Level 2 Layer 1
	{ 0, 32, 48, 56,64, 80, 96, 112,          128, 144, 160,176,  192, 224, 256, 0},
	// Level 2 Layer 2
	{ 0, 8, 16, 24,  32, 40, 48, 56,    64, 80, 96, 112,    128, 144, 160, 0 },
	// Id for Layer 3
	{ 0, 8, 16, 24,  32, 40, 48, 56,    64, 80, 96, 112,    128, 144, 160, 0 },
};


uint8_t	getMpegFrameInfo(uint8_t *stream,uint32_t maxSearch, MpegAudioInfo *mpegInfo,MpegAudioInfo *templ,uint32_t *offset)
{
uint32_t start=0,found=0;
uint8_t  a[4];
uint32_t nfq,fqindex,brindex,index;
			memcpy(a+1,stream,3);
			do
			{
				
				memmove(a,a+1,3);
				a[3]=stream[start+3];
				if(start>=maxSearch-3) break;
				start++;
				if(a[0]==0xff && ((a[1]&0xF0)==0xF0))
				{
					// Layer
					mpegInfo->layer=4-(a[1]>>1)&3;	
					mpegInfo->level=4-(a[1]>>3)&3;
					if(mpegInfo->level==3) continue;
					if(mpegInfo->level==4) mpegInfo->level=3;
					mpegInfo->protect=a[1]&1;
					mpegInfo->padding=(a[2]>>1)&1;
					mpegInfo->mode=(a[3])>>6;
										
					fqindex=(a[2]>>2)&3;
					brindex=(a[2]>>4);
					
					// Remove impossible case
					if(mpegInfo->layer==0) continue;
					// Check fq
					switch(mpegInfo->level)
					{
						case 1: mpegInfo->samplerate=MP3Fq[fqindex];break;
						case 2: mpegInfo->samplerate=MP2Fq[fqindex];break;
						case 3: mpegInfo->samplerate=MP2Fq[fqindex]>>1;break;
						default: mpegInfo->samplerate=0;break;
					}
					// impossible fq
					if(!mpegInfo->samplerate) continue;
					// Bitrate now
					
					// Compute bitrate
					switch(mpegInfo->level)
					{
						case 2:
						case 3:
							index=4+mpegInfo->layer;
							break;
						case 1:
							index=mpegInfo->layer;	
							break;
						default:
							continue;
					}
					
					mpegInfo->bitrate=Bitrate[index][brindex];
					if(!mpegInfo->bitrate) continue;
					
					// Check consistency
					if(templ)
					{
						if(templ->samplerate!=mpegInfo->samplerate) continue;
					
					}
					found=1;
				}
				
			}while(!found && start<maxSearch-4);
			if(!found)
				{	
					return 0;
				}
/*	*/
			// Sample in the packet
			if(1==mpegInfo->layer) 
				mpegInfo->samples=384;
			else
				mpegInfo->samples=1152;
			*offset=start-1;
			
			// Packet size
			//L1:FrameLengthInBytes = (12 * BitRate / SampleRate + Padding) * 4
		
			switch(mpegInfo->layer)
			{
				case 1:
					mpegInfo->size=((12*1000*mpegInfo->bitrate)/mpegInfo->samplerate)
								+mpegInfo->padding;
					mpegInfo->size=mpegInfo->size+4+2*mpegInfo->protect;
					break;
				default:
				//FrameLengthInBytes = 144 * BitRate / SampleRate + Padding
					mpegInfo->size=(144*mpegInfo->bitrate*1000)/mpegInfo->samplerate;
					mpegInfo->size+=mpegInfo->padding;			
			}
			if(*offset)
				{
					printf("MP3: Skipped %lu bytes\n",*offset);
				}
#if 0			
			printf("Packet found : at :%d level:%d layer:%d fq:%d bitrate:%d mode:%d\n",
					start-1,mpegInfo->level,mpegInfo->layer,mpegInfo->samplerate,
					mpegInfo->bitrate,mpegInfo->mode);
			printf("Padd:%lu, crc on:%lu size:%lu\n",mpegInfo->padding,mpegInfo->protect,
								mpegInfo->size);
#endif					
			return 1;
			

	

}
//____________

