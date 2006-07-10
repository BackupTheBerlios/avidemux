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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"


#include "ADM_audiofilter/audiofilter_limiter_param.h"

#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioeng_film2pal.h"

#include "ADM_audiofilter/audioeng_ffmp2.h"
#include "ADM_audiofilter/audioeng_libtoolame.h"


#include "ADM_audiofilter/audioencoder.h"
#ifdef USE_FAAC
#include "ADM_audiofilter/audioencoder_faac.h"
#endif
#ifdef HAVE_LIBMP3LAME
#include "lame/lame.h"
#endif
#ifdef USE_VORBIS
#include "audioeng_vorbis.h"
#endif

#include "gui_action.hxx"

#include "ADM_audiocodec/ADM_audiocodeclist.h"
#include "audioeng_lpcm.h"
#include "audioeng_mixer.h"
#include "prefs.h"
#include "ADM_toolkit/ADM_debugID.h"
#include "audioeng_process.h"

#include "audiofilter_bridge.h"
#include "audiofilter_mixer.h"
#include "audiofilter_normalize.h"
#include "audiofilter_limiter.h"


#include "audioencoder_config.h"

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

static DRCparam drcSetup=
{
  1,
  0.001,//double   mFloor;
  0.2, //double   mAttackTime;
  1.0, //double   mDecayTime;
  2.0, //double   mRatio;
  -12.0 ,//double   mThresholdDB;
};

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
  AUDMAudioFilter *normalize = NULL;
  AUDMAudioFilter *downsample = NULL;
  AUDMAudioFilter *resample = NULL;
  AUDMAudioFilter *tshift = NULL;
  AUDMAudioFilter *drc = NULL;


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

  normalize = new AUDMAudioFilterNormalize(lastFilter);
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
#if 0
      switch(audioResampleMode)
{
      	
      	case RESAMPLING_NONE: break;
  case RESAMPLING_DOWNSAMPLING:
	 	if ((lastFilter->getInfo()->frequency != 48000) 
	  		||  (lastFilter->getInfo()->channels != 2)
			)
{
			GUI_Info_HIG(ADM_LOG_IMPORTANT,"Downsample disabled", "Input is not stereo, 48 kHz.");
} 
		else
{
			printf("\n downsample activated....");
			downsample = new AVDMProcessAudio_Resample(lastFilter, 2);
			lastFilter = downsample;
			filters[filtercount++] = lastFilter;
}
		break;
  case RESAMPLING_CUSTOM:
		printf("\n resampling to %d\n",audioFreq);
		resample = new AVDMProcessAudio_SoxResample(lastFilter, audioFreq);
		lastFilter = resample;
		filters[filtercount++] = lastFilter;	
		break;
	
		
}
	
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

  switch(activeAudioEncoder)
  {
#if 0
    case 	AUDIOENC_NONE:
			// If we are dealing with big endian and using raw wav
			// we have to swap the endianness 
#ifdef ADM_BIG_ENDIAN						
 				lastFilter = new AVDMProcessAudio_LEBE(lastFilter);
 				filters[filtercount++] = lastFilter;
#endif
			break;
    case AUDIOENC_LPCM:
{
                                AVDMProcessAudio_Lpcm *lpcm;
                                lpcm = new AVDMProcessAudio_Lpcm(lastFilter);
				lpcm->init();
                                lastFilter = lpcm;
                                filters[filtercount++] = lastFilter;
  }
                break;
#ifdef USE_VORBIS
    case AUDIOENC_VORBIS:
{
				AVDMProcessAudio_Vorbis *vorbis;
				vorbis = new AVDMProcessAudio_Vorbis(lastFilter);
				if(vorbis->init(audioMP3bitrate, audioMP3mode))
{
					lastFilter = vorbis;
					filters[filtercount++] = lastFilter;
  }
 				else
{
					delete vorbis;
					GUI_Error_HIG("Vorbis initialization failed", "Not activated.");
  }
  }
		break;
#endif		
#endif	
#ifdef USE_FAAC
    case AUDIOENC_FAAC:
{
  AUDMEncoder_Faac *faac;
  faac = new AUDMEncoder_Faac(lastFilter);
  if(faac->init(&aacDescriptor))
  {
    output=faac;
                                        
  }
  else
  {
    delete faac;
    GUI_Error_HIG("FAAC initialization failed", "Not activated.");
  }
}
		break;
#endif		
#if 0
#ifdef HAVE_LIBMP3LAME
    case AUDIOENC_MP3:
{
	  		AVDMProcessAudio_Lame *plame = NULL;

		  		plame = new AVDMProcessAudio_Lame(lastFilter);
				printf("\n Init of lame with bitrate %d , mode %d ",
							 audioMP3bitrate, audioMP3mode);
			  	init = plame->initLame(0, 	(uint32_t) audioMP3mode,
			      					(uint32_t) audioMP3bitrate,
								audioMP3preset);

			  	if (init)
{
						lastFilter = plame;
						filters[filtercount++] = lastFilter;
  } else
{
						delete plame;
						GUI_Error_HIG("LAME initialization failed", "Not activated.");
  }
  }
		    break;
#endif
//______________________________________________________

    case  AUDIOENC_MP2:
{
	  		AVDMProcessAudio_FFmp2 *mp2enc = NULL;
			  // First get parameters from user

			  mp2enc = new AVDMProcessAudio_FFmp2(lastFilter);
			  printf("\n Init of FFmpeg with bitrate %d , mode %d ",
				 audioMP3bitrate, audioMP3mode);
			  init = mp2enc->init((uint32_t) audioMP3bitrate);

			  if (init)
{
						lastFilter = mp2enc;
						filters[filtercount++] = lastFilter;
  } else
{
			    		delete mp2enc;
					GUI_Error_HIG("FFmpeg MP2 initialization failed", "Not activated.");
  }
  }
    	  break;
    case  AUDIOENC_2LAME:
{
	  		AVDMProcessAudio_LibToolame *toolame_enc = NULL;
			  toolame_enc = new AVDMProcessAudio_LibToolame(lastFilter);
			  printf("\n Init of toolame with bitrate %d , mode %d ",
				 audioMP3bitrate, audioMP3mode);
			  init = toolame_enc->init((uint32_t)audioMP3mode,
			  			(uint32_t) audioMP3bitrate);

			if (init)
{
				lastFilter = toolame_enc;
				filters[filtercount++] = lastFilter;
  } else
{
				delete toolame_enc;
				GUI_Error_HIG("tooLAME initialization failed", "Not activated.");
  }
  }
    	  break;
    case  AUDIOENC_AC3:
{
	  		AVDMProcessAudio_FFAC3 *ac3enc = NULL;
			  // First get parameters from user

			  ac3enc = new AVDMProcessAudio_FFAC3(lastFilter);
			  printf("\n Init of FFmpeg AC3 with bitrate %d , mode %d ",
				 audioMP3bitrate, audioMP3mode);
			  init = ac3enc->init((uint32_t) audioMP3bitrate);

			  if (init)
{
						lastFilter = ac3enc;
						filters[filtercount++] = lastFilter;
  } else
{
			    		delete ac3enc;
					GUI_Error_HIG("FFmpeg AC3 initialization failed", "Not activated.");
  }
  }
    	  break;
#endif
    default:
      ADM_assert(0);
  }
//_______________________________________________________



  currentaudiostream->beginDecompress();

  return output;
}


/*
*******************************************************************************************************************
     delete audio filters
*******************************************************************************************************************
*/
void deleteAudioFilter(void)
{
  for (uint32_t i = 0; i < filtercount; i++)
  {
    delete filtersFloat[i];
    filtersFloat[i] = NULL;
  }
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



