/***************************************************************************
                          ADM_codecwma.cpp  -  description
                             -------------------
        We do also AMR here                      
                             
    begin                : Tue Nov 12 2002
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
#include "ADM_lavcodec.h"

#include "fourcc.h"

#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "ADM_audiocodec/ADM_audiocodec.h"

#define _context ((AVCodecContext *)_contextVoid)


   uint8_t ADM_AudiocodecWMA::beginDecompress( void ) 
   {
		 return _inStock=0;
   };
   uint8_t ADM_AudiocodecWMA::endDecompress( void ) 
   {
         return _inStock=0;
   };
   
 ADM_AudiocodecWMA::ADM_AudiocodecWMA(uint32_t fourcc,WAVHeader *info,uint32_t l,uint8_t *d)
       :  ADM_Audiocodec(fourcc)
 {
		ADM_assert(fourcc==WAV_WMA);	
		_contextVoid=(void *)avcodec_alloc_context();
		ADM_assert(_contextVoid);
	// Fills in some values...	
    _context->channels 		= info->channels;
    _context->sample_rate = info->frequency;
    _context->bit_rate = info->byterate<<3; // byte -> bits
//    _context->fourcc = fourcc;
    _context->block_align = info->blockalign;       // ...
    _context->codec_id = CODEC_ID_WMAV2;
    _blockalign=info->blockalign;
    _context->extradata=(void *)d;
    _context->extradata_size=(int)l;	
    printf(" Using %ld bytes of extra header data\n",l);
    mixDump(d,l);
    printf("\n");
		 if (avcodec_open(_context, &wmav2_decoder) < 0) 
		      {
				printf("\n WMA decoder init failed !\n");
				ADM_assert(0);
			}
	printf("FFwma init successful (blockalign %d)\n",info->blockalign);
}
 ADM_AudiocodecWMA::~ADM_AudiocodecWMA()
 {
		avcodec_close(_context);
		ADM_dealloc(_context);
		_contextVoid=NULL;
}    
/*-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------*/

uint8_t ADM_AudiocodecWMA::run( uint8_t * ptr, uint32_t nbIn, uint8_t * outptr,   uint32_t * nbOut)
{
int out;
int max=0,pout=0,toread;
	
	*nbOut=0;
	
	while(1)
	{
		#define PREFILL _blockalign
    	ADM_assert(_inStock<PREFILL);
     	max=_inStock+nbIn;
 //     printf("\n Align :%d max : %d stock :%d in : %d",PREFILL,max,_inStock,nbIn);
       if(max>=(int)PREFILL)
       		{
					max=PREFILL;
					toread=max-_inStock;
					memcpy(_buffer+_inStock,ptr,toread);
					ptr+=toread;	 
					nbIn-= toread;	
					_inStock=0;
				 	out=avcodec_decode_audio(_context,(int16_t *)outptr,&pout,_buffer,max);
				  	if(out<0)
				   {
						 printf( " *** WMA decoding error ***\n");
						  continue;
					}
	//				printf("\n out : %d",pout);

				   if(out<max) 
				   	{ // put it back
				    		uint8_t *zin,*zout;
				      			zin=_buffer+out;
				          		zout=_buffer;
				             		for(uint32_t k=max-out;k>0;k--)
				               			*zout++=*zin++;
							_inStock=max-out;
					}
				   *nbOut+=pout;
				   outptr+=pout;
				} else break;
	}
		// leftover
		memcpy(_buffer+_inStock,ptr,nbIn);
		_inStock+=nbIn;
	   return 1;
}
//---
