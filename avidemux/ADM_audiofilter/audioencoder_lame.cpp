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
#ifdef HAVE_LIBMP3LAME
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

#include "lame/lame.h"
#include "ADM_audiofilter/audioencoder_lame_param.h"
#include "ADM_audiofilter/audioencoder_lame.h"



#define MYFLAGS (lame_global_flags *)lameFlags

AUDMEncoder_Lame::AUDMEncoder_Lame(AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  printf("[Lame] Creating lame\n");
  lameFlags=NULL;
  _wavheader->encoding=WAV_MP3;
};

AUDMEncoder_Lame::~AUDMEncoder_Lame()
{
  printf("[Lame] Deleting lame\n");
  if(lameFlags)
  {
    lame_close(MYFLAGS);
  }
  lameFlags=NULL;
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
uint8_t AUDMEncoder_Lame::init(ADM_audioEncoderDescriptor *config)
{
  int ret;
  MPEG_mode_e mmode;
  uint32_t frequence;
  LAME_encoderParam *lameConf=(LAME_encoderParam *)config->param;
      ADM_assert(config->paramSize==sizeof(LAME_encoderParam));

      lameFlags = lame_init();
      if (lameFlags == NULL)
          return 0;
      
      if(_incoming->getInfo()->channels>2)
      {
        printf("Too many channels\n");
        return 0; 
      }

	// recompute output length
	
      
      ret = lame_set_in_samplerate(MYFLAGS, _wavheader->frequency);
      ret = lame_set_num_channels(MYFLAGS, _wavheader->channels);

    
      frequence = _wavheader->frequency;
    printf("\n output frequency : %lu\n", frequence);
    ret = lame_set_out_samplerate(MYFLAGS, frequence);

    ret = lame_set_quality(MYFLAGS, 2);
    if (_wavheader->channels == 2)
      {
        switch (lameConf->mode)
	    {
	    case ADM_STEREO:
		mmode = STEREO;
		break;
	    case ADM_JSTEREO:
		mmode = JOINT_STEREO;
		break;
	    default:
		printf("\n **** unknown mode ***\n");
		mmode = STEREO;
		break;

	    }
    } else
    {
		mmode = MONO;
     	printf("\n mono audio mp3");
  	}

        ret = lame_set_brate(MYFLAGS, config->bitrate);
        ret = lame_set_mode(MYFLAGS, mmode);	// 0 stereo 1 jstero
        ret = lame_set_quality(MYFLAGS, lameConf->quality);	// 0 stereo 1 jstero
        printf("[Lame]Using quality of %d\n",lame_get_quality(MYFLAGS));
        ret = lame_init_params(MYFLAGS);
    if (ret == -1)
	return 0;
    // update bitrate in header
    _wavheader->byterate = (config->bitrate >> 3) * 1000;
   
    // configure CBR/ABR/...
    _preset=lameConf->preset;
    switch(_preset)
    {
    	default:
    	case ADM_LAME_PRESET_CBR: break;
	case ADM_LAME_PRESET_ABR:
	  
          lame_set_preset( MYFLAGS, config->bitrate);
	  _wavheader->blockalign=1152;
	 break;
	case ADM_LAME_PRESET_EXTREME: 
	  _wavheader->blockalign=1152;
          lame_set_preset( MYFLAGS, EXTREME);	
	break;
    
    
    }

    lame_print_config(MYFLAGS);
    lame_print_internals(MYFLAGS);
    _chunk=1152*_wavheader->channels;
    return 1;
}
uint8_t	AUDMEncoder_Lame::isVBR(void )
{
	if(_preset==ADM_LAME_PRESET_CBR) return 0;
	return 1;

}

uint8_t	AUDMEncoder_Lame::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  int32_t nbout;
  
        *samples = 1152; //FIXME
        *len = 0;

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
          nbout = lame_encode_buffer(MYFLAGS, (int16_t *)&(tmpbuffer[tmphead]),(int16_t *)&(tmpbuffer[tmphead]), _chunk, dest, 16 * 1024);
          
        }
        else
        {
          nbout = lame_encode_buffer_interleaved(MYFLAGS, (int16_t *)&(tmpbuffer[tmphead]), _chunk/2, dest, 16 * 1024);
        }
        tmphead+=_chunk;
        if (nbout < 0) {
          printf("\n Error !!! : %ld\n", nbout);
          return 0;
        }
        *len=nbout;
        return 1;
}
/**
      \fn DIA_getLameSettings
      \brief Dialog to set lame settings
      @return 1 on success, 0 on failure

*/
#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
int DIA_getLameSettings(ADM_audioEncoderDescriptor *descriptor)
  {
    int ret=0;
    char string[400];
    uint32_t mmode,ppreset;
#define SZT(x) sizeof(x)/sizeof(diaMenuEntry )
#define PX(x) &(lameParam->x)
    
    
    LAME_encoderParam *lameParam;
    ADM_assert(sizeof(LAME_encoderParam)==descriptor->paramSize);
  
    lameParam=(LAME_encoderParam*)descriptor->param;
    mmode=lameParam->mode;
    ppreset=lameParam->preset;
    diaMenuEntry channelMode[]={
                             {ADM_STEREO,      _("Stereo"),NULL},
                             {ADM_JSTEREO,   _("Joint stereo"),NULL},
                             {ADM_MONO,      _("Mono"),NULL}};
          
    diaElemMenu menuMode(&mmode,   _("Channel Mode"), SZT(channelMode),channelMode);
    
    diaMenuEntry encodingMode[]={
                             {ADM_LAME_PRESET_CBR,      _("CBR"),NULL},
                             {ADM_LAME_PRESET_ABR,   _("ABR"),NULL},
                             {ADM_LAME_PRESET_EXTREME,      _("Extreme"),NULL}}; 
    diaElemMenu Mode(&ppreset,   _("Mode"), SZT(encodingMode),encodingMode);
#define BITRATE(x) {x,_(#x)}
    diaMenuEntry bitrateM[]={
                              BITRATE(56),
                              BITRATE(64),
                              BITRATE(80),
                              BITRATE(96),
                              BITRATE(112),
                              BITRATE(128),
                              BITRATE(160),
                              BITRATE(192),
                              BITRATE(224)
                          };
    diaElemMenu bitrate(&(descriptor->bitrate),   _("Bitrate"), SZT(bitrateM),bitrateM);
    
    
    
    
    diaElemUInteger quality(PX(quality),_("Quality"),0,9);
  
      diaElem *elems[]={&menuMode,&Mode,&quality,&bitrate};
    
  if( diaFactoryRun("Lame Settings",4,elems))
  {
    lameParam->mode=(ADM_mode)mmode; 
    lameParam->preset=(ADM_LAME_PRESET)ppreset;
    return 1;
  }
  return 0;
}  
#endif
