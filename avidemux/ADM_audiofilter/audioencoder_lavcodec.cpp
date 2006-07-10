/***************************************************************************
                        
    copyright            : (C) 2002-2006 by mean
    email                : fixounet@free.fr
    
    Interface to FFmpeg mpeg1/2 audio encoder
    
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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ADM_assert.h>
#include "ADM_library/default.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"
//
#include "ADM_audiofilter/audioencoder_lavcodec.h"

#include "ADM_lavcodec.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"


#define CONTEXT ((AVCodecContext  	*)_context)


// Ctor: Duplicate
//__________

AUDMEncoder_Lavcodec::AUDMEncoder_Lavcodec(uint32_t fourcc,AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  _fourcc=fourcc;
  if(_fourcc!=WAV_MP2 && _fourcc!=WAV_AC3) ADM_assert(0);
  _context=NULL;
  _wavheader->encoding=_fourcc;
  printf("[Lavcodec] Creating Lavcodec\n");
};


AUDMEncoder_Lavcodec::~AUDMEncoder_Lavcodec()
{
  printf("[Lavcodec] Deleting Lavcodec\n");
  if(_context)
  {
    avcodec_close(CONTEXT);
    ADM_dealloc(_context);
  }
  _context=NULL;
  cleanup();
};

//________________________________________________
//   Init lame encoder
//_______________________________________________
uint8_t AUDMEncoder_Lavcodec::init(ADM_audioEncoderDescriptor *config)
{
  int ret;
  _context=( void *)avcodec_alloc_context();
  _wavheader->byterate=(config->bitrate*1000)>>3;

      
  if(_fourcc==WAV_MP2 && _incoming->getInfo()->channels>2)
  {
    printf("[Lavcodec]Too many channels\n");
    return 0; 
  }
  _wavheader->byterate=(config->bitrate*1000)>>3;         
      
  if(_fourcc==WAV_MP2)
    _chunk = 1152*_wavheader->channels;
  else
    _chunk = 1536*_wavheader->channels; // AC3

  printf("[Lavcodec]Incoming : fq : %lu, channel : %lu bitrate: %lu \n",
         _wavheader->frequency,_wavheader->channels,config->bitrate);
  
  
  CONTEXT->channels     =  _wavheader->channels;
  CONTEXT->sample_rate  =  _wavheader->frequency;
  CONTEXT->bit_rate     = (config->bitrate*1000); // bits -> kbits

  AVCodec *codec;
  CodecID codecID;
  
  if(_fourcc==WAV_MP2) codecID=CODEC_ID_MP2;
        else codecID=CODEC_ID_AC3;
  codec = avcodec_find_encoder(codecID);
  ADM_assert(codec);
  
  ret = avcodec_open(CONTEXT, codec);
  if (0> ret) 
  {
    printf("[Lavcodec] init failed err : %d!\n",ret);
    return 0;
  }


  printf("[Lavcodec]Lavcodec successfully initialized\n");
  return 1;       
}
//*********************************
uint8_t	AUDMEncoder_Lavcodec::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  uint32_t nbout;
  
  *samples = _chunk/_wavheader->channels; //FIXME
  *len = 0;

  if(!refillBuffer(_chunk ))
  {
    return 0; 
  }
        
  if(tmptail-tmphead<_chunk)
  {
    return 0; 
  }
        // Do in place replace
  dither16(&(tmpbuffer[tmphead]),_chunk);
        
  ADM_assert(tmptail>=tmphead);
  nbout = avcodec_encode_audio(CONTEXT, dest, 5000, (short *) &(tmpbuffer[tmphead]));

  tmphead+=_chunk;
  if (nbout < 0) 
  {
    printf("[Lavcodec] Error !!! : %ld\n", nbout);
    return 0;
  }
  *len=nbout;
  return 1;
}

// EOF
