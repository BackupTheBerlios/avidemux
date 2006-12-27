
/***************************************************************************
    copyright            : (C) 2002-6 by mean
    email                : fixounet@free.fr

    Interface to FAAC

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
#ifdef USE_FAAC
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

#include "faac.h"
#include "ADM_audiofilter/audioencoder_faac.h"

AUDMEncoder_Faac::AUDMEncoder_Faac(AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  uint32_t channels;
  channels=instream->getInfo()->channels;
  switch(channels)
  {
    case 1:ch_order[1] = CH_FRONT_LEFT;break;
    case 2:
      ch_order[0] = CH_FRONT_LEFT;
      ch_order[1] = CH_FRONT_RIGHT;
      break;
    default :
      ch_order[0] = CH_FRONT_CENTER;
      ch_order[1] = CH_FRONT_LEFT;
      ch_order[2] = CH_FRONT_RIGHT;
      ch_order[3] = CH_REAR_LEFT;
      ch_order[4] = CH_REAR_RIGHT;
      ch_order[5] = CH_LFE;
  }
};


AUDMEncoder_Faac::~AUDMEncoder_Faac()
{
    if(_handle)
        faacEncClose(_handle);
    _handle=NULL;

    printf("[FAAC] Deleting faac\n");
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
uint8_t AUDMEncoder_Faac::init(ADM_audioEncoderDescriptor *config)
{
unsigned long int samples_input, max_bytes_output;
faacEncConfigurationPtr cfg;
int ret=0;

     _handle = faacEncOpen(_wavheader->frequency,
                                 _wavheader->channels,
                                 &samples_input,
                                &max_bytes_output);
    if(!_handle)
    {
          printf("Cannot open faac with fq=%lu chan=%lu br=%lu\n",
          _wavheader->frequency,_wavheader->channels,config->bitrate);
          return 0;
    }
    printf(" [FAAC] : Sample input:%d, max byte output%d \n",samples_input,max_bytes_output);
    cfg= faacEncGetCurrentConfiguration(_handle);
    
    // Set default conf, same as ffmpeg
    cfg->aacObjectType = LOW;
    cfg->mpegVersion = MPEG4;
    cfg->bandWidth= (_wavheader->frequency*3)/4; // Should be relevant
    cfg->useTns = 0;
    cfg->allowMidside = 0;
    cfg->bitRate = (config->bitrate*1000)/_wavheader->channels; // It is per channel
    cfg->outputFormat = 0; // 0 Raw 1 ADTS
    cfg->inputFormat = FAAC_INPUT_FLOAT;
    cfg->useLfe=0;	
    if (!(ret=faacEncSetConfiguration(_handle, cfg))) 
    {
        printf("[FAAC] Cannot set conf for faac with fq=%lu chan=%lu br=%lu (err:%d)\n",
				_wavheader->frequency,_wavheader->channels,config->bitrate,ret);
	return 0;
    }
     unsigned char *data=NULL;
     unsigned long size=0;
     if((ret=faacEncGetDecoderSpecificInfo(_handle, &data,&size)))
     {
        printf("FAAC: GetDecoderSpecific info failed (err:%d)\n",ret);
        return 0;
     }
     _extraSize=size;
     _extraData=new uint8_t[size];
     memcpy(_extraData,data,size);

    // update
     _wavheader->byterate=(config->bitrate*1000)/8;
//    _wavheader->dwScale=1024;
//    _wavheader->dwSampleSize=0;
    _wavheader->blockalign=4096;
    _wavheader->bitspersample=0;

    _chunk=samples_input;


    printf("[Faac] Initialized :\n");
    
    printf("[Faac]Version        : %s\n",cfg->name);
    printf("[Faac]Bitrate        : %lu\n",cfg->bitRate);
    printf("[Faac]Mpeg2 (1)/4(0) : %u\n",cfg->mpegVersion);
    printf("[Faac]Use lfe      ) : %u\n",cfg->useLfe);
    printf("[Faac]Sample output  : %lu\n",_chunk / _wavheader->channels);
    printf("[Faac]Bitrate        : %lu\n",cfg->bitRate*_wavheader->channels);

    
    return 1;
}

//_____________________________________________
//  Need to multiply the float by 65535, can't use
//  generic fill buffer
//----------------------------------------------
uint8_t AUDMEncoder_Faac::refillBuffer(int minimum)
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

#define FA_BUFFER_SIZE (SIZE_INTERNAL/4)
//______________________________________________
uint8_t	AUDMEncoder_Faac::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  uint32_t count=0;
_again:
        *samples = _chunk/_wavheader->channels;
        *len = 0;

        if(!refillBuffer(_chunk ))
        {
          return 0; 
        }
        ADM_assert(tmptail>=tmphead);
        if(_wavheader->channels > 2)
            reorderChannels(&(tmpbuffer[tmphead]),_chunk);
        *len = faacEncEncode(_handle, (int32_t *)&(tmpbuffer[tmphead]), _chunk, dest, FA_BUFFER_SIZE);
        if(!*len) 
        {
          count++;
          if(count<20)
            goto _again;
          *samples=0;
        }
        tmphead+=_chunk;
        return 1;
}

#endif		
// EOF
