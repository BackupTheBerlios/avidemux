
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

#include <ADM_assert.h>

#include "ADM_library/default.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"
//
extern "C"
{
#include "aften/aften.h"
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

    _HANDLE->sample_format=A52_SAMPLE_FMT_FLT;
    _HANDLE->channels=_wavheader->channels;
    _HANDLE->samplerate=_wavheader->frequency;
    
    _HANDLE->acmod=0;
    _HANDLE->lfe=0;
    
    int er= aften_encode_init(_HANDLE);
    if(er<0)
    {
      printf("[Aften] init error %d\n",er); 
      return 0;
    }
    _chunk=256*6*_wavheader->channels;
    return 1;
}

//_____________________________________________
//  Need to multiply the float by 65535, can't use
//  generic fill buffer
//----------------------------------------------
uint8_t AUDMEncoder_Aften::refillBuffer(int minimum)
{
  uint32_t filler=_wavheader->frequency*_wavheader->channels;
  uint32_t nb;
  AUD_Status status;
  if(eof_met) return 0;
  while(1)
  {
    ADM_assert(tmptail>=tmphead);
    if((tmptail-tmphead)>=minimum) return 1;
  
    if(tmphead && tmptail>filler/2)
    {
      memmove(&tmpbuffer[0],&tmpbuffer[tmphead],(tmptail-tmphead)*sizeof(float)); 
      tmptail-=tmphead;
      tmphead=0;
    }
    ADM_assert(filler>tmptail);
    nb=_incoming->fill( (filler-tmptail)/2,&tmpbuffer[tmptail],&status);
    if(!nb)
    {
      if(status!=AUD_END_OF_STREAM) ADM_assert(0);
      
      if((tmptail-tmphead)<minimum)
      {
        memset(&tmpbuffer[tmptail],0,sizeof(float)*(minimum-(tmptail-tmphead)));
        tmptail=tmphead+minimum;
        eof_met=1;  
        return minimum;
      }
      else continue;
    } else
    {
      float *s=&(tmpbuffer[tmptail]);
      for(int i=0;i<nb;i++)
      {
        *s=*s*65535.;
        s++;
      }
      tmptail+=nb;
    }
  }
}


//______________________________________________
uint8_t	AUDMEncoder_Aften::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  uint32_t count=0;
  int r;
_again:
        *samples = _chunk/_wavheader->channels;
        *len = 0;

        if(!refillBuffer(_chunk ))
        {
          return 0; 
        }
        ADM_assert(tmptail>=tmphead);
        r=aften_encode_frame(_HANDLE, dest,(void *)&(tmpbuffer[tmphead]));
        if(r<0)
        {
          printf("[Aften] Encoding error %d\n",r);
          return 0; 
        }
        
        *samples=256*6;
        tmphead+=_chunk;
        return 1;
}

#endif		
// EOF
