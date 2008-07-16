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

#include "ADM_default.h"
#include "DIA_factory.h"
#include "DIA_coreToolkit.h"
#include <lame/lame.h>
#include "audioencoder.h"
#include "audioencoderInternal.h"
#include "audioencoder_lame_param.h"
#include "audioencoder_lame.h"
static uint8_t      getConfigurationData(uint32_t *l, uint8_t **d);
static uint8_t      setConfigurationData(uint32_t l, uint8_t *d);


static AUDMEncoder *create(AUDMAudioFilter *head)
{
    return new AUDMEncoder_Lame(head);
}
static void destroy(AUDMEncoder *in)
{
    AUDMEncoder_Lame *z=(AUDMEncoder_Lame *)in;
    delete z;
}
static uint8_t configure(void);


static LAME_encoderParam myLameParam=
{
  128,
  ADM_LAME_PRESET_CBR, // preset;
  ADM_STEREO, //ADM_mode        mode;
  2, //uint32_t        quality;
  0, //uint32_t        disableReservoir; // usefull for strict CBR (FLV)
};

static ADM_audioEncoder lameDesc={
    ADM_AUDIO_ENCODER_API_VERSION, //uint32_t     apiVersion;            // const
    create,                        // AUDMEncoder *(*create)(AUDMAudioFilter *head);  
    destroy,                       // void         (*destroy)(AUDMEncoder *codec);
    configure,                     // int          (*configure)(void);    
    "LAME",
    "MP3 (Lame)",
    "Lame encoding plugin (c) 2008 Mean",
    2,     // Max Channel
    1,0,0, //Major minor patch
    WAV_MP3,                // const Avi fourcc
    200,              // const Higher means the codec is prefered and should appear first in the list
    getConfigurationData, // Get the encoder private conf
    setConfigurationData,

    NULL,  // GetBitrate
    NULL,  // SetBitrate

    NULL, // Set Option

    NULL              // Hide stuff in here
};

uint8_t      getConfigurationData(uint32_t *l, uint8_t **d) // Get the encoder private conf
{
    *l=sizeof(lameDesc);
    *d=(uint8_t *)&lameDesc;
    return 1;
}
uint8_t      setConfigurationData(uint32_t l, uint8_t *d) // Get the encoder private conf
{
    if(sizeof(lameDesc) != l) 
    {
        GUI_Error_HIG("Audio Encoder","The configuration size does not match the codec size");
        return 0;
    }
    memcpy(&lameDesc,d,l);
    return 1;
}


extern "C" ADM_audioEncoder *getInfo(void)
{
    return &lameDesc;
}

#define MYFLAGS (lame_global_flags *)lameFlags

AUDMEncoder_Lame::AUDMEncoder_Lame(AUDMAudioFilter * instream)  :AUDMEncoder    (instream)
{
  printf("[Lame] Creating lame\n");
  lameFlags=NULL;
  _wavheader->encoding=WAV_MP3;
};

AUDMEncoder_Lame::~AUDMEncoder_Lame()
{
#if 0
  printf("[Lame] Deleting lame\n");
  if(lameFlags)
  {
    lame_close(MYFLAGS);
  }
  lameFlags=NULL;
  cleanup();
#endif
};


//________________________________________________
//   Init lame encoder
// frequence    : Impose frequency , 0 means reuse the incoming fq
// mode                         : ADM_STEREO etc...
// bitrate              : Bitrate in kbps (96,192...)
// return 0 : init failed
//                              1 : init succeeded
//_______________________________________________

uint8_t AUDMEncoder_Lame::initialize(void)
{
#if 0
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
        ret = lame_set_disable_reservoir(MYFLAGS,lameConf->disableReservoir);
        printf("[Lame]Using quality of %d\n",lame_get_quality(MYFLAGS));
        ret = lame_init_params(MYFLAGS);
    if (ret == -1)
	return 0;
    // update bitrate in header
    _wavheader->byterate = (config->bitrate >> 3) * 1000;
#define BLOCK_SIZE 1152
    // configure CBR/ABR/...
    _preset=lameConf->preset;
    switch(_preset)
    {
    	default:
    	case ADM_LAME_PRESET_CBR: 
          break;
	case ADM_LAME_PRESET_ABR:
	  
          lame_set_preset( MYFLAGS, config->bitrate);
	  _wavheader->blockalign=BLOCK_SIZE;
	 break;
	case ADM_LAME_PRESET_EXTREME: 
	  _wavheader->blockalign=BLOCK_SIZE;
          lame_set_preset( MYFLAGS, EXTREME);	
	break;
    
    
    }

    lame_print_config(MYFLAGS);
    lame_print_internals(MYFLAGS);
    _chunk=BLOCK_SIZE*_wavheader->channels;
#endif
    return 1;
}
uint8_t	AUDMEncoder_Lame::isVBR(void )
{
	if(_preset==ADM_LAME_PRESET_CBR) return 0;
	return 1;

}

uint8_t	AUDMEncoder_Lame::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
#if 0
  int32_t nbout;
  
        *samples = BLOCK_SIZE; //FIXME
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
        if(!*len) *samples=0;
        //printf("Audio packet : size %u, sample %u\n",*len,*samples);
#endif
        return 1;
}
/**
      \fn configure
      \brief Dialog to set lame settings
      @return 1 on success, 0 on failure

*/
#define QT_TR_NOOP(x) x

 uint8_t configure(void)
  {
    int ret=0;
    char string[400];
    uint32_t mmode,ppreset;
#define SZT(x) sizeof(x)/sizeof(diaMenuEntry )
#define PX(x) &(lameParam->x)
    
    LAME_encoderParam *lameParam=&myLameParam;
    mmode=lameParam->mode;
    ppreset=lameParam->preset;
    diaMenuEntry channelMode[]={
                             {ADM_STEREO,      QT_TR_NOOP("Stereo"),NULL},
                             {ADM_JSTEREO,   QT_TR_NOOP("Joint stereo"),NULL},
                             {ADM_MONO,      QT_TR_NOOP("Mono"),NULL}};
          
    diaElemMenu menuMode(&mmode,   QT_TR_NOOP("C_hannel mode:"), SZT(channelMode),channelMode);
    
    diaMenuEntry encodingMode[]={
                             {ADM_LAME_PRESET_CBR,      QT_TR_NOOP("CBR"),NULL},
                             {ADM_LAME_PRESET_ABR,   QT_TR_NOOP("ABR"),NULL},
#if 0
                             {ADM_LAME_PRESET_EXTREME,      QT_TR_NOOP("Extreme"),NULL}
#endif
    }; 
    diaElemMenu Mode(&ppreset,   QT_TR_NOOP("Bit_rate mode:"), SZT(encodingMode),encodingMode);
#define BITRATE(x) {x,QT_TR_NOOP(#x)}
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
    uint32_t fixme;
    diaElemMenu bitrate(&fixme,   QT_TR_NOOP("_Bitrate:"), SZT(bitrateM),bitrateM);
    
    
    
    
    diaElemUInteger quality(PX(quality),QT_TR_NOOP("_Quality:"),0,9);
    diaElemToggle reservoir(PX(disableReservoir),QT_TR_NOOP("_Disable reservoir:"));
  
    diaElem *elems[]={&menuMode,&Mode,&quality,&bitrate,&reservoir};
    
  if( diaFactoryRun(QT_TR_NOOP("LAME Configuration"),5,elems))
  {
    return 1;
  }
  return 0;
}  

// EOF

