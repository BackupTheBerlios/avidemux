/***************************************************************************
    copyright            : (C) 2006 by mean
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ADM_assert.h>
#include "ADM_utilities/default.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"
//
#include "ADM_audiofilter/audioencoder_pcm.h"


#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_osSupport/ADM_debug.h"




// Ctor: Duplicate
//__________

AUDMEncoder_PCM::AUDMEncoder_PCM(uint32_t reverted,uint32_t fourCC,AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  printf("[PCM] Creating PCM\n");
  ADM_assert(fourCC==WAV_PCM || fourCC==WAV_LPCM);
  _wavheader->encoding=fourCC;
  revert=reverted;
};


AUDMEncoder_PCM::~AUDMEncoder_PCM()
{
  printf("[PCM] Deleting PCM\n");
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
uint8_t AUDMEncoder_PCM::init(ADM_audioEncoderDescriptor *config)
{
  
  _wavheader->byterate=_wavheader->channels*_wavheader->frequency*2;
  _chunk = (_wavheader->frequency/10)*_wavheader->channels*2;
  

 
  printf("[PCM]Incoming :fq : %lu, channel : %lu \n",_wavheader->frequency,_wavheader->channels);
  printf("[PCM]PCM successfully initialized\n");
  return 1;       
}

uint8_t	AUDMEncoder_PCM::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  uint32_t nbout;
  
  *samples = _chunk; //FIXME
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
  dither16(&(tmpbuffer[tmphead]),_chunk,_wavheader->channels);
  if(!revert)
    memcpy(dest,&(tmpbuffer[tmphead]),_chunk*2);
  else
  {
    uint16_t *in,*out,tmp;
    in=(uint16_t*)&(tmpbuffer[tmphead]);
    out=(uint16_t *)dest;
    for(int i=0;i<_chunk;i++)
    {
      tmp=*in++;
      tmp=((tmp&0xff)<<8)+(tmp>>8);
      *out++=tmp;
    }
  }
  tmphead+=_chunk;
  *len=_chunk*2;
  *samples=_chunk/_wavheader->channels;
  return 1;
}


// EOF
