//
// C++ Implementation: ADM_aacInfo
//
// Description: 
//		Decode an aac frame an fill the info field
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
#include "ADM_audio/ADM_aacinfo.h"


static 	uint32_t aacBitrate[16]=
{
	96000, 88200, 64000, 48000,
	44100, 32000, 24000, 22050,
	16000, 12000, 11025,  8000,
	0,     0,     0,     0 
};
/*
	12 bits 111111	0xFC Sync tag
	1		1 Mpeg2 0 Mpeg4
	2		00
	1		prot : 1 absent 0 present
	
	
	2		profile  00 main/01 LC
	4		sampling index
	1		private
	
	3		channel
	1		original
	1		Home
	
	28 bits
	
	
	1		copyriht
	1		copyright id 
	13		*** AAC frame length (including headers)
	11		Buffer fullness 0x7FF = vbr
	2		nb raw frame
	
	28 bits
	
	-- total = 56 bits = 8 bytes
	
	16		crc
	
*/

uint8_t	getAACFrameInfo(uint8_t *stream,uint32_t maxSearch, AacAudioInfo *mpegInfo,AacAudioInfo *templ,uint32_t *offset)
{
uint32_t start=0,found=0;
uint8_t  a[8];
uint32_t nfq,fqindex,brindex,index,nbframe;
			memset(mpegInfo,0,sizeof(*mpegInfo));
			memcpy(a+1,stream,7);
			do
			{
				
				memmove(a,a+1,7);
				a[7]=stream[start+7];
				if(start>=maxSearch-7) break;
				start++;
				if(a[0]==0xff && ((a[1]&0xF0)==0xF0))
				{
					// Layer
					mpegInfo->layer=(a[1]>>3)&1;	
					if((a[1]>>1) & 3) continue;
					
					mpegInfo->profile=a[2]>>6;
					mpegInfo->samplerate=aacBitrate[(a[2]>>2) & 0xF];
					mpegInfo->channels=(a[3]>>5);
					nbframe=(a[7]>>6)&3;
					nbframe++;
					mpegInfo->size=((a[4]&3)<<11)
							+((a[5])<<3)
							+((a[6]>>5));
						
					if(!mpegInfo->samplerate) continue;
					
					found=1;
				}
				
			}while(!found && start<maxSearch-4);
			if(!found)
				{	
					return 0;
				}
			*offset=start-1;
			printf("AAC Frame found at offset :%lu layer:%lu profile:%lu samperate:%lu channels:%lu size:%lu nbBlock:%lu\n",
					*offset,
					mpegInfo->layer,
					mpegInfo->profile, 
					mpegInfo->samplerate,
					mpegInfo->channels,
					mpegInfo->size,
					nbframe);
			return 1;
			

	

}
//____________

