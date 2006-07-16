/***************************************************************************
                          audiodeng_buildfilters.cpp  -  description
                             -------------------
    begin                : Mon Dec 2 2002
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
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <ADM_assert.h>
#include "ADM_library/default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_audio/audiomode.hxx"
#include "ADM_audiofilter/audiofilter_limiter_param.h"
#include "ADM_audiofilter/audioencoder_lame_param.h"
#include "ADM_audiofilter/audioencoder_twolame_param.h"
#include "ADM_audiofilter/audioencoder_faac_param.h"
#include "ADM_audiofilter/audioencoder_vorbis_param.h"
#include "ADM_audiofilter/audiofilter_normalize_param.h"

#include "audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audio_raw.h"

/* ************* Encoder *********** */
#include "ADM_audiofilter/audioencoder.h"
#ifdef USE_FAAC
#include "ADM_audiofilter/audioencoder_faac.h"
#endif
#ifdef HAVE_LIBMP3LAME
#include "ADM_audiofilter/audioencoder_lame.h"
#endif
#ifdef USE_VORBIS
#include "ADM_audiofilter/audioencoder_vorbis.h"
#endif
#include "ADM_audiofilter/audioencoder_twolame.h"
#include "ADM_audiofilter/audioencoder_lavcodec.h"


#include "ADM_audiocodec/ADM_audiocodeclist.h"
#include "ADM_audiofilter/audioencoder_pcm.h"

#include "prefs.h"


/* ************ Filters *********** */
#include "audiofilter_bridge.h"
#include "audiofilter_mixer.h"
#include "audiofilter_normalize.h"
#include "audiofilter_limiter.h"
#include "audiofilter_sox.h"
#include "audiofilter_film2pal.h"

/* ************ Conf *********** */
#include "audioencoder_config.h"


#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"

#define MAX_AUDIO_FILTER 10


extern AVDMGenericAudioStream *currentaudiostream;

static AUDMAudioFilter *filtersFloat[MAX_AUDIO_FILTER];
static uint32_t filtercount = 0;

extern uint32_t audioProcessMode(void);

/******************************************************
  Configuration variables for filters
******************************************************/

extern AUDIOENCODER  activeAudioEncoder;

extern GAINparam audioGain;
extern int  audioFreq;
extern int  audioDRC;
extern FILMCONV audioFilmConv;
extern CHANNEL_CONF audioMixing;
extern int audioMP3mode;
extern int audioMP3bitrate;
extern RESAMPLING  audioResampleMode;
extern int   audioShift ;
extern int   audioDelay;

static DRCparam drcSetup=
{
  1,
  0.001,//double   mFloor;
  0.2, //double   mAttackTime;
  1.0, //double   mDecayTime;
  2.0, //double   mRatio;
  -12.0 ,//double   mThresholdDB;
};

static ADM_audioEncoderDescriptor *getAudioDescriptor( AUDIOENCODER encoder);

//
// Build audio filters
// Starttime : starttime in ms
// mode : 0 for playback,  1 for audio save
//_______________________________________


AUDMAudioFilter *buildInternalAudioFilter(AVDMGenericAudioStream *currentaudiostream,uint32_t starttime)
{

  AUDMAudioFilter *firstFilter = NULL;
  AUDMAudioFilter *lastFilter = NULL;
  
  deleteAudioFilter(NULL);
  int32_t timeShiftMs=audioDelay*audioShift;
  
  
  firstFilter = new AUDMAudioFilter_Bridge(NULL,currentaudiostream, starttime,timeShiftMs);
  filtercount = 0;
  lastFilter = firstFilter;
  filtersFloat[filtercount++] = firstFilter;



//_______________________________________________________
      if ( audioGain.mode!=ADM_NO_GAIN)	// Normalize activated ?
      {
        printf("\n  normalize activated..\n");
      
        AUDMAudioFilterNormalize *normalize = new AUDMAudioFilterNormalize(lastFilter,&audioGain);
        lastFilter = normalize;
        filtersFloat[filtercount++] = lastFilter;
      
      }
      
      if( (audioMixing!=CHANNEL_INVALID ))
          {
            AUDMAudioFilter *mixer;
            mixer=new AUDMAudioFilterMixer( lastFilter,audioMixing);
            lastFilter = mixer;
            filtersFloat[filtercount++] = lastFilter;
          }

    if (audioDRC)
          {
            AUDMAudioFilterLimiter *pdrc = NULL;
            printf("\n  DRC activated...\n");
            pdrc = new AUDMAudioFilterLimiter(lastFilter,&drcSetup);
            lastFilter = (AUDMAudioFilter *)pdrc;
            filtersFloat[filtercount++] = lastFilter;
          
          }

      switch(audioResampleMode)
          {
                  
            case RESAMPLING_NONE: break;
            case RESAMPLING_CUSTOM:
            {
                      AUDMAudioFilterSox  *resample=NULL;
                      resample = new AUDMAudioFilterSox(lastFilter, audioFreq);
                      lastFilter = resample;
                      filtersFloat[filtercount++] = lastFilter;	
            }
                      break;

            default:
                      ADM_assert(0);
          }

      switch(audioFilmConv)
        {
          default:
                        ADM_assert(0);
          case FILMCONV_NONE:
                        break;
          case FILMCONV_PAL2FILM:		
                  AUDMAudioFilterPal2Film *p2f;
                        p2f = new AUDMAudioFilterPal2Film(lastFilter);
                        lastFilter = p2f;
                        filtersFloat[filtercount++] = lastFilter;	
                        break;
                        
                
          case FILMCONV_FILM2PAL:
                    AUDMAudioFilterFilm2Pal *f2p;
                        f2p = new AUDMAudioFilterFilm2Pal(lastFilter);
                        lastFilter = f2p;
                        filtersFloat[filtercount++] = lastFilter;	
                        break;
                
                        
        }   
//_______________________________________________________



    currentaudiostream->beginDecompress();
    return lastFilter;
}
/*
*******************************************************************************************************************

*******************************************************************************************************************
*/
AUDMAudioFilter *buildPlaybackFilter(AVDMGenericAudioStream *currentaudiostream, uint32_t starttime, uint32_t duration)
{
  AUDMAudioFilter *lastFilter=NULL;
  int32_t sstart;
  uint32_t channels;

        // Do we need to go back
  sstart=(int32_t)starttime;
  int32_t timeShiftMs=audioDelay*audioShift;
        
  deleteAudioFilter(NULL);
  
  lastFilter = new AUDMAudioFilter_Bridge(NULL,currentaudiostream,sstart,timeShiftMs);
        filtercount = 0;
        filtersFloat[filtercount++] = lastFilter;
        
        
        // Downmix for local playback ?
        
        uint32_t downmix;
        
        if(prefs->get(DOWNMIXING_PROLOGIC,&downmix)!=RC_OK)
        {       
          downmix=0;
        }
        channels=lastFilter->getInfo()->channels;
        if( downmix && channels>2)
        {
          CHANNEL_CONF mix;
          if(downmix==1) 
          {
            printf("Downmixing to prologic\n");
            mix=CHANNEL_DOLBY_PROLOGIC;
          }
          else
          {   
            printf("Downmixing to prologic2\n");  
            mix=CHANNEL_DOLBY_PROLOGIC2;
          }
          AUDMAudioFilterMixer *mixer;
          mixer=new AUDMAudioFilterMixer( lastFilter,mix);
          lastFilter = mixer;
          filtersFloat[filtercount++] = lastFilter;
        }	
        return lastFilter;
}
/*
*******************************************************************************************************************

*******************************************************************************************************************
*/

AVDMGenericAudioStream *buildAudioFilter(AVDMGenericAudioStream *currentaudiostream,  uint32_t starttime)
{
  AUDMAudioFilter         *lastFilter=NULL;
  AVDMGenericAudioStream  *output=NULL;
  AUDMEncoder             *tmpfilter=NULL;
  
	// if audio is set to copy, we just return the first filter
  if(!audioProcessMode())
  {
    int32_t timeShiftMs=audioDelay*audioShift;
    deleteAudioFilter(NULL);
    output = new AVDMProcessAudio_RawShift(currentaudiostream, starttime, timeShiftMs);
    return output;

  }



// else we build the full chain
  lastFilter=buildInternalAudioFilter(currentaudiostream,starttime);
  
// and add encoder...


//_______________________________________________________
  uint8_t init;
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor( activeAudioEncoder);
  
  if(!lastFilter)
  {
    printf(" buildInternalAudioFilter failed\n");
    return 0;
  }
  if(lastFilter->getInfo()->channels > descriptor->maxChannels)
  {
    GUI_Error_HIG(_("Codec Error"),_("The number of channels is greater than what the selected audio codec can do.\n"
        "Either change codec or use the mixer filter to have less channels."));
    deleteAudioFilter(NULL);
    return 0; 
  }
  switch(activeAudioEncoder)
  {

                  case AUDIOENC_LPCM:
                  case AUDIOENC_NONE:
                  {
                    AUDMEncoder_PCM *pcm;
                    uint32_t fourcc,revert=0;
                    
                    if(activeAudioEncoder==AUDIOENC_LPCM) fourcc=WAV_LPCM;
                    else          fourcc=WAV_PCM;
                    
#ifdef ADM_BIG_ENDIAN                    
                    if(fourcc==WAV_PCM)
#else
                    if(fourcc==WAV_LPCM)
#endif
                        revert=1;
                    pcm = new AUDMEncoder_PCM(revert,fourcc,lastFilter);
                    tmpfilter=pcm;
                  }
                  break;
#ifdef USE_VORBIS
             case AUDIOENC_VORBIS:
                {
                  AUDMEncoder_Vorbis *vorbis;
                  vorbis = new AUDMEncoder_Vorbis(lastFilter);
                  tmpfilter=vorbis;
                  break;
                }
#endif		
#ifdef USE_FAAC
    case AUDIOENC_FAAC:
                  {
                      AUDMEncoder_Faac *faac;
                          faac = new AUDMEncoder_Faac(lastFilter);
                          tmpfilter=faac;
                  }
                  break;
#endif		

#ifdef HAVE_LIBMP3LAME
    case AUDIOENC_MP3:
              {
                AUDMEncoder_Lame *plame = NULL;

                            plame = new AUDMEncoder_Lame(lastFilter);
                            tmpfilter=plame;
                }
              break;
#endif
    case AUDIOENC_AC3:
    case AUDIOENC_MP2:
                    {
                      AUDMEncoder_Lavcodec *lavcodec = NULL;
                      uint32_t fourc;
                      
                      if(activeAudioEncoder==AUDIOENC_AC3) fourc=WAV_AC3;
                      else fourc=WAV_MP2;
                    
                      lavcodec = new AUDMEncoder_Lavcodec(fourc,lastFilter);
                      tmpfilter=lavcodec;
                    }
              break;
    case  AUDIOENC_2LAME:
              {
                  AUDMEncoder_Twolame *toolame_enc = NULL;
                          toolame_enc = new AUDMEncoder_Twolame(lastFilter);
                          tmpfilter=toolame_enc;
              }
    	  break;

    default:
      ADM_assert(0);
  }
//_______________________________________________________

  if(!tmpfilter->init(descriptor))
  {
    delete tmpfilter;
    tmpfilter=NULL;
    GUI_Error_HIG("Encoder initialization failed", "Not activated."); 
  }
  output=tmpfilter;

  currentaudiostream->beginDecompress();

  return output;
}


/*
*******************************************************************************************************************
     delete audio filters
*******************************************************************************************************************
*/
void deleteAudioFilter(AVDMGenericAudioStream *in)
{
  for (uint32_t i = 0; i < filtercount; i++)
  {
    delete filtersFloat[i];
    filtersFloat[i] = NULL;
  }
  if(in)
    delete in;
  filtercount = 0;
  if (currentaudiostream)
    currentaudiostream->endDecompress();

}
/**********************************************/
ADM_audioEncoderDescriptor *getAudioDescriptor( AUDIOENCODER encoder)
{
  for(int i=0;i<NB_AUDIO_DESCRIPTOR;i++)
  {
    if(allDescriptors[i]->encoder==encoder)
    {
      return    allDescriptors[i];
    }
  }
  ADM_assert(0);
  return NULL;
}

uint8_t getAudioExtraConf(uint32_t *bitrate,uint32_t *extraDataSize, uint8_t **extradata)
{
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor(activeAudioEncoder);
  
  *bitrate=descriptor->bitrate;
  *extraDataSize=descriptor->paramSize;
  *extradata=(uint8_t *)descriptor->param;
  return 1;
  
}
uint8_t setAudioExtraConf(uint32_t bitrate,uint32_t extraDataSize, uint8_t *extradata)
{
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor(activeAudioEncoder);
  if(extraDataSize!=descriptor->paramSize)
  {
    printf("Invalid descriptor for codec\n");
    return 0; 
  }
  printf("Valid descriptor found for audio codec\n");
  descriptor->bitrate=bitrate;
  memcpy(descriptor->param,extradata,extraDataSize);
  return 1;
  
}

void audioCodecConfigure( void )
{
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor(activeAudioEncoder);
  if(descriptor->configure)
  {
    descriptor->configure(descriptor); 
  }
}
uint32_t audioGetBitrate(void)
{
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor(activeAudioEncoder);
  return descriptor->bitrate;
} 
void audioFilter_SetBitrate( int i)
{
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor(activeAudioEncoder);
  descriptor->bitrate=i;
}




