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
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiocodec/ADM_audiocodec.h"
#include "ADM_audiocodec/ADM_audiocodec.h"

#define _context ((AVCodecContext *)_contextVoid)

static uint8_t scratchPad[48000*2];

   uint8_t ADM_AudiocodecWMA::beginDecompress( void ) 
   {
            _tail=_head=0;
            return 1;
   };
   uint8_t ADM_AudiocodecWMA::endDecompress( void ) 
   {
          _tail=_head=0;
          return 1;
   };
   
 ADM_AudiocodecWMA::ADM_AudiocodecWMA(uint32_t fourcc,WAVHeader *info,uint32_t l,uint8_t *d)
       :  ADM_Audiocodec(fourcc)
 {
    _tail=_head=0;
    ADM_assert(fourcc==WAV_WMA || fourcc==WAV_QDM2);	
    _contextVoid=(void *)avcodec_alloc_context();
    ADM_assert(_contextVoid);
    // Fills in some values...
    _context->sample_rate = info->frequency;
    _context->channels = info->channels;
    _context->block_align = info->blockalign;      
    if(fourcc==WAV_WMA)
        _context->codec_id = CODEC_ID_WMAV2;
    else
        if(fourcc==WAV_QDM2)
        _context->codec_id = CODEC_ID_QDM2;
            else ADM_assert(0);

    _blockalign=info->blockalign;
    _context->extradata=(void *)d;
    _context->extradata_size=(int)l;

    printf(" Using %ld bytes of extra header data\n",l);
    mixDump((uint8_t *)_context->extradata,_context->extradata_size);

   AVCodec *codec=avcodec_find_decoder(_context->codec_id);
   if(!codec) {GUI_Error_HIG(_("Internal error"), _("Cannot open WMA2 codec."));ADM_assert(0);} 
    if (avcodec_open(_context, codec) < 0)
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

uint8_t ADM_AudiocodecWMA::run(uint8_t *inptr, uint32_t nbIn, float *outptr, uint32_t *nbOut, ADM_ChannelMatrix *matrix)
{
int out=0;
int max=0,pout=0;
int16_t *run16;

        *nbOut=0;
        // Shrink
        if(_head && (_tail+nbIn)*3>ADMWA_BUF*2)
        {
            memmove(_buffer,_buffer+_head,_tail-_head);
            _tail-=_head;
            _head=0;
        }
        //
        ADM_assert(nbIn+_tail<ADMWA_BUF);
        memcpy(_buffer+_tail,inptr,nbIn);
        _tail+=nbIn;
        while(_tail-_head>=_blockalign)
        {
          out=avcodec_decode_audio(_context,(int16_t *)scratchPad,
                                   &pout,_buffer+_head,_blockalign);
                
          if(out<0)
          {
            printf( " *** WMA decoding error (%u)***\n",_blockalign);
            _head+=1; // Try skipping some bytes
            continue;
          }
            
          _head+=out; // consumed bytes
          pout>>=1;
          *nbOut+=pout;
          run16=(int16_t *)scratchPad;
          for(int i=0;i<pout;i++)
          {
            *outptr++=((float)run16[i])/32767.;
          }
        }
        
        
        
        return 1;
}
///************************************************
  uint8_t ADM_AudiocodecAMR::beginDecompress( void ) 
   {
            _tail=_head=0;
            return 1;
   };
   uint8_t ADM_AudiocodecAMR::endDecompress( void ) 
   {
            _tail=_head=0;
            return 1;
   };
/* AMR specific */
 ADM_AudiocodecAMR::ADM_AudiocodecAMR(uint32_t fourcc,WAVHeader *info)
       :  ADM_Audiocodec(fourcc)
 {
        _tail=_head=0;
        ADM_assert(fourcc==WAV_AMRNB);	
        _contextVoid=(void *)avcodec_alloc_context();
        ADM_assert(_contextVoid);
        // Fills in some values...	
        info->channels=_context->channels 		= 1;
        info->frequency=_context->sample_rate = 8000;
        _context->bit_rate = 12000; // byte -> bits
        _context->block_align = 32;       // ...
        _context->codec_id = CODEC_ID_AMR_NB;
        _context->extradata=NULL;
        _context->extradata_size=0;	
        printf(" Opening AMR codec\n");

        AVCodec *codec=avcodec_find_decoder(CODEC_ID_AMR_NB);
        if(!codec) {GUI_Error_HIG(_("Internal error"), _("Cannot open AMR codec."));ADM_assert(0);} 
        if (avcodec_open(_context, codec) < 0)
        {
            printf("\n AMR decoder init failed !\n");
            ADM_assert(0);
        }
        printf("FFamr init successful \n");
}
 ADM_AudiocodecAMR::~ADM_AudiocodecAMR()
 {
        avcodec_close(_context);
        ADM_dealloc(_context);
        _contextVoid=NULL;
}    
/*-------------------------------------------------------------------------------------------------------------------------
-------------------------------------------------------------------------------------------------------------------------*/
#define AMR_PACKET 32
uint8_t ADM_AudiocodecAMR::run(uint8_t *inptr, uint32_t nbIn, float *outptr, uint32_t *nbOut, ADM_ChannelMatrix *matrix)
{
int out;
int pout=0;
int16_t *run16;


        *nbOut=0;
        // Shrink
        if(_head && (_tail+nbIn)*3>ADM_AMR_BUFFER*2)
        {
            memmove(_buffer,_buffer+_head,_tail-_head);
            _tail-=_head;
            _head=0;
        }
        //
        ADM_assert(nbIn+_tail<ADM_AMR_BUFFER);
        memcpy(_buffer+_tail,inptr,nbIn);
        _tail+=nbIn;
        while(_tail-_head>AMR_PACKET)
        {
          out=avcodec_decode_audio(_context,(int16_t *)scratchPad,
                                   &pout,_buffer+_head,_tail-_head);
                
                if(out<0)
                {
                        printf( " *** AMR decoding error ***\n");
                        _head+=1; // Try skipping some bytes
                        continue;
                }
            
                _head+=out; // consumed bytes
                pout>>=1;
                *nbOut+=pout;
                run16=(int16_t *)scratchPad;
                for(int i=0;i<pout;i++)
                {
                  *outptr++=((float)run16[i])/32767.;
                }
        }
        

        return 1;
}

//---
