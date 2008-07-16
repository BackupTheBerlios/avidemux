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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_default.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"
//
#include "audioencoder_twolame.h"
#include "audioencoder_twolame_param.h"

extern "C"
{
#include "ADM_libraries/ADM_libtwolame/twolame.h"
}
#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_osSupport/ADM_debug.h"


#define OPTIONS (twolame_options_struct *)_twolameOptions

// Ctor: Duplicate
//__________

AUDMEncoder_Twolame::AUDMEncoder_Twolame(AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  printf("[TwoLame] Creating Twolame\n");
  _twolameOptions=NULL;
  _wavheader->encoding=WAV_MP2;
};


AUDMEncoder_Twolame::~AUDMEncoder_Twolame()
{
  printf("[TwoLame] Deleting TwoLame\n");
  if(_twolameOptions)
  {
    twolame_close((twolame_options_struct **)&_twolameOptions);
  }
  _twolameOptions=NULL;
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
uint8_t AUDMEncoder_Twolame::init(ADM_audioEncoderDescriptor *config)
{
  int ret;
  TWOLAME_MPEG_mode mmode;    	
  uint32_t frequence;
  TWOLAME_encoderParam *lameConf=(TWOLAME_encoderParam *)config->param;
  ADM_assert(config->paramSize==sizeof(TWOLAME_encoderParam));

  _twolameOptions = twolame_init();
  if (_twolameOptions == NULL)
    return 0;
      
  if(_wavheader->channels>2)
  {
    printf("[TwoLame]Too many channels\n");
    return 0; 
  }
  _wavheader->byterate=(config->bitrate*1000)>>3;         
      
 
  _chunk = 1152*_wavheader->channels;

 
  printf("[TwoLame]Incoming :fq : %lu, channel : %lu bitrate: %lu \n",
        _wavheader->frequency,_wavheader->channels,config->bitrate);
		
 
  twolame_set_in_samplerate(OPTIONS, _wavheader->frequency);
  twolame_set_out_samplerate (OPTIONS, _wavheader->frequency);
  twolame_set_num_channels(OPTIONS, _wavheader->channels);
  if(_wavheader->channels==1) mmode=TWOLAME_MONO;
  else
    switch (lameConf->mode)
  {
    case ADM_STEREO:
      mmode = TWOLAME_STEREO;
      break;
    case ADM_JSTEREO:
      mmode = TWOLAME_JOINT_STEREO;
      break;
    case ADM_MONO:
      mmode=TWOLAME_MONO;
      break;
				
    default:
      printf("\n **** unknown mode, going stereo ***\n");
      mmode = TWOLAME_STEREO;
      break;

  }
  twolame_set_mode(OPTIONS,mmode);
  twolame_set_error_protection(OPTIONS,TRUE);	
    	//toolame_setPadding (options,TRUE);
  twolame_set_bitrate (OPTIONS,config->bitrate);
  twolame_set_verbosity(OPTIONS, 2);
  if(twolame_init_params(OPTIONS))
  {
    printf("[TwoLame]Twolame init failed\n");
    return 0;
  }
	
 

  printf("[TwoLame]Libtoolame successfully initialized\n");
  return 1;       
}

uint8_t	AUDMEncoder_Twolame::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  int nbout;
  
  *samples = 1152; //FIXME
  *len = 0;
  ADM_assert(tmptail>=tmphead);
  if(!refillBuffer(_chunk ))
  {
    return 0; 
  }
        
  if(tmptail-tmphead<_chunk)
  {
    return 0; 
  }

  dither16(&(tmpbuffer[tmphead]),_chunk,_wavheader->channels);

  ADM_assert(tmptail>=tmphead);
  if (_wavheader->channels == 1)
  {
    nbout =twolame_encode_buffer(OPTIONS, (int16_t *)&(tmpbuffer[tmphead]),(int16_t *)&(tmpbuffer[tmphead]), _chunk, dest, 16 * 1024);
  }
  else
  {
    nbout = twolame_encode_buffer_interleaved(OPTIONS, (int16_t *)&(tmpbuffer[tmphead]), _chunk/2, dest, 16 * 1024);
  }
  tmphead+=_chunk;
  ADM_assert(tmptail>=tmphead);
  if (nbout < 0) {
    printf("\n Error !!! : %ld\n", nbout);
    return 0;
  }
  *len=nbout;
  return 1;
}


// EOF
