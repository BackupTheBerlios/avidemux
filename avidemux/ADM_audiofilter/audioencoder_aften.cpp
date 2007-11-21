
/***************************************************************************
    copyright            : (C) 2002-6 by mean
    email                : fixounet@free.fr

    Interface to Aften

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
#ifdef USE_AFTEN
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_assert.h"

#include "default.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"
//
extern "C"
{
#if defined(USE_AFTEN_06)
	#include "aften.h"
#else	// Aften 0.05 & 0.07 onwards
	#include "aften/aften.h"
#endif
};
#include "ADM_audiofilter/audioencoder_aften_param.h"
#include "ADM_audiofilter/audioencoder_aften.h"

#define _HANDLE ((AftenContext *)_handle)
AUDMEncoder_Aften::AUDMEncoder_Aften(AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  uint32_t channels;
  channels=instream->getInfo()->channels;
  _handle=(void *)new AftenContext;
  memset(_handle,0,sizeof(AftenContext));
  aften_set_defaults(_HANDLE);
  _wavheader->encoding=WAV_AC3;
#if defined(USE_AFTEN_05) || defined(USE_AFTEN_06)
#elif defined(USE_AFTEN_07)
  _HANDLE->params.n_threads=1; // MThread collides with avidemux multithreading
#else
  _HANDLE->system.n_threads=1;
#endif
};


AUDMEncoder_Aften::~AUDMEncoder_Aften()
{
    if(_handle)
      aften_encode_close(_HANDLE);
    delete(_HANDLE);
    _handle=NULL;

    printf("[Aften] Deleting aften\n");
    cleanup();
};


//________________________________________________
//   Init lame encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode                         : ADM_STEREO etc...
// bitrate              : Bitrate in kbps (96,192...)
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________
uint8_t AUDMEncoder_Aften::init(ADM_audioEncoderDescriptor *config)
{


int ret=0;

#if defined(USE_AFTEN_05) || defined(USE_AFTEN_06)
int mask;
#else
unsigned int mask;
#endif

    _wavheader->byterate=(config->bitrate*1000)/8;
    _HANDLE->sample_format=A52_SAMPLE_FMT_FLT;
    _HANDLE->channels=_wavheader->channels;
    _HANDLE->samplerate=_wavheader->frequency;
    
    _HANDLE->params.bitrate=config->bitrate;
    switch(_wavheader->channels)
    {
        case 1: mask = 0x04;  break;
        case 2: mask = 0x03;  break;
        case 3: mask = 0x07;  break;
        case 4: mask = 0x107; break;
        case 5: mask = 0x37;  break;
        case 6: mask = 0x3F;  break;
      }

#if defined(USE_AFTEN_05) || defined(USE_AFTEN_06)
	aften_wav_chmask_to_acmod(_wavheader->channels, mask, &(_HANDLE->acmod), &(_HANDLE->lfe));
#else
	aften_wav_channels_to_acmod(_wavheader->channels, mask, &(_HANDLE->acmod), &(_HANDLE->lfe));
#endif

   //   _HANDLE->params.verbose=2;
    int er= aften_encode_init(_HANDLE);
    if(er<0)
    {
      printf("[Aften] init error %d\n",er); 
      return 0;
    }
    _chunk=256*6*_wavheader->channels;
    printf("[Aften] Initialized with fd %u Channels %u bitrate %u\n",_HANDLE->samplerate,
                                                                    _HANDLE->channels,_HANDLE->params.bitrate);
    return 1;
}


//______________________________________________
uint8_t	AUDMEncoder_Aften::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  uint32_t count=0;
  int r;
  void *ptr;
_again:
        *len = 0;
        _chunk=256*6*_wavheader->channels;
        if(!refillBuffer(_chunk ))
        {
          return 0; 
        }
        ptr=(void *)&(tmpbuffer[tmphead]);
        ADM_assert(tmptail>=tmphead);

#ifdef USE_AFTEN_05
		aften_remap_wav_to_a52(ptr, 256*6, _wavheader->channels, A52_SAMPLE_FMT_FLT, _HANDLE->acmod, _HANDLE->lfe);
#else
		aften_remap_wav_to_a52(ptr, 256*6, _wavheader->channels, A52_SAMPLE_FMT_FLT, _HANDLE->acmod);
#endif

        r=aften_encode_frame(_HANDLE, dest,(void *)ptr);
        if(r<0)
        {
          printf("[Aften] Encoding error %d\n",r);
          return 0; 
        }
        
        *samples=256*6;
        *len=r;
        tmphead+=_chunk;
        return 1;
}

#endif		
// EOF
