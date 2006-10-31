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
#ifdef USE_VORBIS
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
#include "ADM_audiofilter/audioencoder_vorbis_param.h"
#include "ADM_audiofilter/audioencoder_vorbis.h"


#include "vorbis/vorbisenc.h"
#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_osSupport/ADM_debug.h"


#define OPTIONS (twolame_options_struct *)_twolameOptions

#define VD (((vorbisStruct *)_handle)->vd)
#define VI (((vorbisStruct *)_handle)->vi)
#define VB (((vorbisStruct *)_handle)->vb)
#define VC (((vorbisStruct *)_handle)->vc)
typedef struct vorbisStruct
{ 
	vorbis_info 	 vi ;
	vorbis_dsp_state vd ;
	vorbis_block     vb ;
	vorbis_comment   vc ;
}vorbisStruct;
//__________

AUDMEncoder_Vorbis::AUDMEncoder_Vorbis(AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  printf("[Vorbis] Creating Vorbis\n");
  _handle=NULL;
  _wavheader->encoding=WAV_OGG;
  _oldpos=0;
  _handle=(void *)new  vorbisStruct; 
  ch_order[0] = CH_FRONT_LEFT;
  ch_order[1] = CH_FRONT_RIGHT;
  ch_order[2] = CH_REAR_LEFT;
  ch_order[3] = CH_REAR_RIGHT;
  ch_order[4] = CH_FRONT_CENTER;
  ch_order[5] = CH_LFE;
};


AUDMEncoder_Vorbis::~AUDMEncoder_Vorbis()
{
  printf("[Vorbis] Deleting Vorbis\n");
  if(_handle)
  {
    vorbis_block_clear(&VB);
    vorbis_dsp_clear(&VD);
    vorbis_info_clear(&VI);
    delete (vorbisStruct *)_handle;
  }    	
  _handle=NULL;
  
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
uint8_t AUDMEncoder_Vorbis::init(ADM_audioEncoderDescriptor *config)
{
  int ret;
  VORBIS_encoderParam *vorbisConf=(VORBIS_encoderParam *)config->param;
  ADM_assert(config->paramSize==sizeof(VORBIS_encoderParam));

  ogg_packet header1,header2,header3;
  int err;

  
  
  vorbis_info_init(&VI) ;

  switch(vorbisConf->mode)
  {
    case ADM_VORBIS_CBR:
                      err=vorbis_encode_init(&VI,
                              _wavheader->channels,
                              _wavheader->frequency,
                              -1, // Max bitrate      
                              config->bitrate*1000, //long nominal_bitrate,
                              -1 //long min_bitrate))
                            );
                      break;
    case  ADM_VORBIS_VBR :
                    err=vorbis_encode_init_vbr(&VI,
                                _wavheader->channels,
                                _wavheader->frequency,
                                vorbisConf->quality/10
                              );
                    break;
    default:
      ADM_assert(0);
  }
  if (err!=0) 
  {
    printf("[vorbis] init error %d\n",err);
    return 0;
  }
  vorbis_analysis_init(&VD, &VI) ;
  vorbis_block_init(&VD, &VB);
  vorbis_comment_init(&VC);
  vorbis_comment_add_tag(&VC, "encoder", "AVIDEMUX2") ;

  vorbis_analysis_headerout(&VD, &VC, &header1,
                             &header2, &header3);


// Store all headers as extra data
// see ogg vorbis decode for details
// we need 3 packets

  _extraSize=header1.bytes+header2.bytes+header3.bytes+3*sizeof(uint32_t);
  _extraData=new uint8_t[_extraSize];

  uint32_t *ex=(uint32_t *)_extraData;
  uint8_t *d;
  d=_extraData+sizeof(uint32_t)*3;
  ex[0]=header1.bytes;
  ex[1]=header2.bytes;
  ex[2]=header3.bytes;
  memcpy(d,header1.packet,ex[0]);
  d+=ex[0];
  memcpy(d,header2.packet,ex[1]);
  d+=ex[1];
  memcpy(d,header3.packet,ex[2]);
  vorbis_comment_clear(&VC);
			
  printf("\n[Vorbis]Vorbis encoder initialized\n");
  switch(vorbisConf->mode)
  {
    case ADM_VORBIS_CBR:
      printf("[Vorbis]CBR Bitrate:%lu\n",config->bitrate);
      break;
    case ADM_VORBIS_VBR: //FIXME FIXME FIXME
      printf("[Vorbis]VBR Quality:%.1f\n",vorbisConf->quality);
    break;
    default:
      ADM_assert(0);
  }
   
  printf("[Vorbis]Channels  :%lu\n",_wavheader->channels);
  printf("[Vorbis]Frequency :%lu\n",_wavheader->frequency);
  return 1;
}

#define ROUNDMAX 3000

uint8_t	AUDMEncoder_Vorbis::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  uint32_t nbout;
  uint32_t consumed=0;
  float **float_samples;
  ogg_packet op ;

  *len = 0;
  _chunk=1024*_wavheader->channels;
  int count=ROUNDMAX;
// Check that we have packet from previous pass
  while(count--)
  {
    if(!refillBuffer(_chunk ))
    {
      return 0; 
    }
        
    if(tmptail-tmphead<_chunk)
    {
      return 0; 
    }
    
	//printf("Round %d\n",ROUNDMAX-count);
    if(vorbis_analysis_blockout(&VD, &VB) == 1) 
    {
      vorbis_analysis(&VB, NULL);
      vorbis_bitrate_addblock(&VB) ;
	//printf("Blockout\n");
	
      if(vorbis_bitrate_flushpacket(&VD, &op)) 
      {
        if(op.bytes<2) continue; // avoid the 1byte sized packet

        memcpy(dest, op.packet,op.bytes);
        *len=op.bytes;
        *samples=op.granulepos-_oldpos;
        _oldpos=op.granulepos;
        //  aprintf("1st packet :sampl:%lu len :%lu sample:%lu abs:%llu\n",*samples,op.bytes,total,op.granulepos);
        return 1;
      }
    }

    
    uint32_t nbSample=(tmptail-tmphead)/_wavheader->channels;
    if(nbSample>1024) nbSample=1024;
    float_samples=vorbis_analysis_buffer(&VD, nbSample) ;
    int index=tmphead;
    // Put our samples in incoming buffer
    for (int i = 0; i < nbSample; i++)
      for (int j = 0; j < _wavheader->channels; j++) {
      float_samples[j][i] = tmpbuffer[index++];
      if (float_samples[j][i] > 1) float_samples[j][i] = 1;
      if (float_samples[j][i] < -1) float_samples[j][i] = -1;
      }
      reorderChannels(&(tmpbuffer[tmphead]),tmptail-tmphead);
      // Buffer full, go go go
      vorbis_analysis_wrote(&VD, nbSample) ;  
      tmphead+=nbSample*_wavheader->channels;	
  }
  return 0;
	
}
#endif		
// EOF
