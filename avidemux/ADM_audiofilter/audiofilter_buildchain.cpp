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

#include "audioprocess.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"


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

extern int  audioNormalizeMode ;
extern int  audioFreq;
extern int  audioDRC;
extern FILMCONV audioFilmConv;
extern CHANNEL_CONF audioMixing;
extern int audioMP3mode;
extern int audioMP3bitrate;
extern RESAMPLING  audioResampleMode;

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


AVDMProcessAudioStream *buildInternalAudioFilter(AVDMGenericAudioStream *currentaudiostream,
				uint32_t starttime, uint32_t duration)
{

  AUDMAudioFilter *firstFilter = NULL;
  AUDMAudioFilter *lastFilter = NULL;

  printf("\n Audio codec : %d",activeAudioEncoder);
// In fact it is more a decompress filter than a null filter
  //
/* Mean: Ugly hack, in case of Film2Pal filter, the filter will need
  more samples in than out (assuming we changed the fps)
  So we correct the filter length beforehand to avoid the 24/25 too short audio */
	

#if 0
  /*   */
  if(audioFilmConv==FILMCONV_FILM2PAL)	
  {
    double d;
    d=duration;
    d*=25000;
    d/=23976;
    duration=(uint32_t)floor(d);
    duration=(duration+1)&0xfffffffe;
  }
#endif
#if 0        
//_______________________________________________________
    if (audioDelay && audioShift)
{
                printf("\n Time shift activated with %d %d ms", audioShift,audioDelay);
                // Depending on starttime & delay
                // we may have or not to add silence
                int32_t sstart=(int32_t)starttime;
                
                if (sstart>=audioDelay) // just seek in the file
{
                        firstFilter = new AVDMProcessAudio_Null(currentaudiostream,
                                            sstart-audioDelay, duration);
                        filtercount = 0;
                        lastFilter = firstFilter;
                        filters[filtercount++] = firstFilter;
}
                else    // We have to add a silence of -sstart seconds
{
                
                        firstFilter = new AVDMProcessAudio_Null(currentaudiostream,
                                            sstart, duration);
                        filtercount = 0;
                        lastFilter = firstFilter;
                        filters[filtercount++] = firstFilter;
                        // Then the silence
                        AVDMProcessAudio_TimeShift *ts;
                        ts = new AVDMProcessAudio_TimeShift(lastFilter, audioDelay);
                        tshift = (AVDMProcessAudioStream *) ts;
                        lastFilter = tshift;
                        
                        filters[filtercount++] = lastFilter;
}
}
      else // no delay
#endif      
{
  firstFilter = new AUDMAudioFilter_Bridge(NULL,currentaudiostream, starttime);
  filtercount = 0;
  lastFilter = firstFilter;
  filtersFloat[filtercount++] = firstFilter;
}


//_______________________________________________________
 if ( audioNormalizeMode)	// Normalize activated ?
{
  printf("\n  normalize activated...\n");

  AUDMAudioFilterNormalize *normalize = new AUDMAudioFilterNormalize(lastFilter);
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
#if 0	
      switch(audioFilmConv)
{
  default:
		ADM_assert(0);
  case FILMCONV_NONE:
		break;
  case FILMCONV_PAL2FILM:		
	 AVDMProcessAudio_Pal2Film *p2f;
		printf("\n Pal2Film\n");
		
		p2f = new AVDMProcessAudio_Pal2Film(lastFilter);
		lastFilter = p2f;
		filters[filtercount++] = lastFilter;	
		break;
	 	
	
  case FILMCONV_FILM2PAL:
		AVDMProcessAudio_Film2Pal *f2p;
		printf("\n Film2pal\n");
		
		f2p = new AVDMProcessAudio_Film2Pal(lastFilter);
		lastFilter = f2p;
		filters[filtercount++] = lastFilter;	
		break;
	
		
}   
		
#endif	
	
//_______________________________________________________



    currentaudiostream->beginDecompress();
#if 0    
return lastFilter;
#endif
  return NULL;
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
#if 0
        if(audioShift && audioDelay)
{
                if(sstart>audioDelay) sstart-=audioDelay;
                else
{
                        sstart=audioDelay-sstart;
                        lastFilter = new AVDMProcessAudio_Null(currentaudiostream,0, duration);
                        filtercount = 0;
                        filters[filtercount++] = lastFilter;

                        AVDMProcessAudio_TimeShift *ts;
                                ts = new AVDMProcessAudio_TimeShift(lastFilter,    sstart);

                                lastFilter = ts;
                                printf("\n Time shift activated with %d %d ms", audioShift,audioDelay);
                                filters[filtercount++] = lastFilter;
                                return lastFilter;
}
}
#endif
        lastFilter = new AUDMAudioFilter_Bridge(NULL,currentaudiostream,sstart);
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

AVDMProcessAudioStream *buildAudioFilter(AVDMGenericAudioStream *currentaudiostream,
                                         uint32_t starttime, uint32_t duration)
{
  AUDMAudioFilter *lastFilter=NULL;
  AVDMProcessAudioStream *output=NULL;
  AUDMEncoder             *tmpfilter=NULL;
  
	// if audio is set to copy, we just return the first filter
  if(!audioProcessMode())
  {
#if 0          
 			lastFilter = new AVDMProcessAudio_Null(currentaudiostream,	   starttime, duration);
    			filtercount = 0;
    			lastFilter = lastFilter;
    			filters[filtercount++] = lastFilter;
			return lastFilter;
#endif  
            return NULL;
  }



// else we build the full chain
  buildInternalAudioFilter(currentaudiostream,starttime, duration);
  lastFilter=filtersFloat[filtercount-1];
// and add encoder...


//_______________________________________________________
  uint8_t init;
  ADM_audioEncoderDescriptor *descriptor=getAudioDescriptor( activeAudioEncoder);
  
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
/*
*******************************************************************************************************************
// Build a simple filter chain
// That is starting from startTime in ms, has a duration of duration ms and is shifter
// by shift ms

*******************************************************************************************************************
*/


AVDMGenericAudioStream *buildRawAudioFilter( uint32_t startTime, uint32_t duration, int32_t shift)
{
#if 0
AVDMProcessAudioStream *firstFilter = NULL;
AVDMProcessAudioStream *lastFilter = NULL;
 
	// Start from a clean state
	printf("Building raw audio filter with start:%lu duration:%lu shift:%d\n",startTime,duration,shift);
	deleteAudioFilter();	
	lastFilter=new AVDMProcessAudio_RawShift(currentaudiostream,shift,startTime);
	filters[filtercount++] = lastFilter;
	return lastFilter;
	
#endif 
  return NULL;
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



