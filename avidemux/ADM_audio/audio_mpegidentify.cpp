/***************************************************************************
                          audio_mpegidentify.cpp  -  description

	Identify the mpegaudio features (layer/fq/bitrate etc...)

                             -------------------
    begin                : Wed Dec 19 2001
    copyright            : (C) 2001 by mean
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
//#include <stream.h>
#include <assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "aviaudio.hxx"

uint16_t MP1L2Bitrate[2][16] = 
{
// mpeg1 Layer 2
{
	 0, 32, 48, 56,
    64, 80, 96, 112,
    128, 160, 192, 224,
    256, 320, 384, 0 // Thx Dopez!    
},
// mpeg1 layer 3
{ 0, 32, 40, 48,
    56, 64, 80, 96,
    112, 128, 160, 192,
    224, 256, 320, 0
}
};
static uint32_t MP3Fq[4] = { 44100, 48000, 32000, 0 };       
static uint16_t MP3Mode[4] = { 2, 2, 2, 1 };
//
uint16_t MP2L1Bitrate[2][16] = 
{
// mpeg2 Layer 1
{
	 0, 32, 48, 56,
    64, 80, 96, 112,
    128, 144, 160,176, 
    192, 224, 256, 0
},
// mpeg1 layer 2/3
{ 	0, 8, 16, 24,
    32, 40, 48, 56,
    64, 80, 96, 112,
    128, 144, 160, 0
}
};
static uint32_t MP2Fq[4] = { 22050, 24000, 16000, 0 };       

//
static const char *MP3AMode[4] =
    { "Stereo", "Joint Stereo", "Dual mono", "mono" };
    
uint8_t mpegAudioIdentify(uint8_t *ptr, uint32_t maxLookUp, WAVHeader *header, uint8_t *tokens)
{

   int a, b, c;
   uint32_t l=0; //,rd=0;
   uint32_t thebitrate;
	
run_again:
    do
      {
	  if ((c = *ptr++)== 0xff)	// synchro frame... ?
	    {
rebranch:
		c = *ptr++ ;
		switch (c)
		  {
		  case 0xff:
		      goto rebranch;
		  case 0xfa:
		  case 0xf3:
	     case 0xf5:
		  case 0xfb:
	  	  case 0xfc:
	  	  case 0xfd:
	
		      goto contact;
		  }
	    }
      }
    while (l++<maxLookUp-1);
    printf("\n cannot detect MPEG frame...(%d)\n",maxLookUp);
    return 0;
  contact:
  
    a = *ptr++ ;
    b =*ptr++ ;

    uint8_t bitrate, fq, mode;
    uint8_t version,layer;

    bitrate = a >> 4;
    fq = 3 & (a >> 2);
    mode = b >> 6;
    
    version=3&(c>>3);
    layer=3&(c>>1);
    
    version=4-version;                                                            
    layer=  4-layer;
    if(tokens)
    {
	tokens[0]=c;
	tokens[1]=a;
	tokens[2]=b;
	
    }
   
    
                  printf("\n (%02x %02x %02x)",c,a,b);
					 printf("\n Mpeg version :%d",version);
					 printf(" Layer version :%d\n",layer);

		switch(version)
		{
			case 1:					 
					if(layer==1)
					{
					printf("\n  mpeg1 layer 1, unsupported\n");
					return 0;		 				
					}
			// Translate to real world value
		    if (MP1L2Bitrate[bitrate] == 0)
			goto run_again;
		    if (MP3Fq[fq] == 0)
			goto run_again;

		    thebitrate=MP1L2Bitrate[layer-2][bitrate];
		    printf("\nBitrt\t: %u\n",thebitrate );
		    printf("Fq \t: %lu\n", MP3Fq[fq]);
		    printf("Mode\t: %u\n", MP3Mode[mode]);
		    printf("Mode\t: %s\n", MP3AMode[mode]);


		    header->channels = MP3Mode[mode];
		    header->frequency = MP3Fq[fq];
		    header->byterate = (1000 * thebitrate) >> 3;   
		    header->bitspersample=16;
		    header->blockalign = 4;
		    if(layer==3)
		    	header->encoding = WAV_MP3;
		    else
		    	header->encoding = WAV_MP2;
		    break;
		    break;

	case 2: // mpeg2 L1/2/3
			if(layer==3) layer=2; // same thing
				
			// Translate to real world value
		    if (MP2L1Bitrate[bitrate] == 0)
			goto run_again;
		    if (MP2Fq[fq] == 0)
			goto run_again;

		    thebitrate=MP2L1Bitrate[layer-1][bitrate];
		    printf("\nBitrt\t: %u\n", thebitrate);
		    printf("Fq \t: %lu\n", MP2Fq[fq]);
		    printf("Mode\t: %u\n", MP3Mode[mode]);
		    printf("Mode\t: %s\n", MP3AMode[mode]);


		    header->channels = MP3Mode[mode];
		    header->frequency = MP2Fq[fq];
		    header->byterate = (1000 * thebitrate) >> 3;   
		    header->bitspersample=16;
			 header->blockalign = 4;
		    if(layer==3)
		    	header->encoding = WAV_MP3;
		    else
		    	header->encoding = WAV_MP2;
		    break;
	   default:
	   		printf("\n unknown mpeg version...\n");
	     	return 0;			  		  
	}		  

    return 1;
 }
