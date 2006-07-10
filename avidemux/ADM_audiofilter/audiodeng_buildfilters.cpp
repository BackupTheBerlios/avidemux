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
#include "avio.hxx"
#include "fourcc.h"
#include "ADM_audiofilter/audiofilter_limiter_param.h"
#include "audioprocess.hxx"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioeng_film2pal.h"

//#include "ADM_gui/GUI_mp3.h"
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
/*
  	Ugly should get ride of it. Temporary fix.
*/
#include "avi_vars.h"
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


#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"
extern void UI_setAProcessToggleStatus( uint8_t status );
extern uint8_t DIA_audioCodec( AUDIOENCODER *codec );
extern int DIA_audioEncoder(int *pmode, int *pbitrate,const char *title);
extern void UI_setAudioCodec( int i);
uint32_t audioFilterGetNbEncoder(void);
const char* audioFilterGetIndexedName(uint32_t i);


typedef struct externalSource
{
 AudioSource type;
 char *name;       
}externalSource;

static const externalSource Sources[]=
{
        {AudioAvi,"VIDEO"},
        {AudioMP3,"MP3"},
        {AudioWav,"WAV"},
        {AudioAC3,"AC3"},
        {AudioNone,"NONE"}
};
typedef struct Mixer_String
{
  char         *name;
  CHANNEL_CONF conf;
};

#define DOME(x) {#x,CHANNEL_##x}
static Mixer_String Mixer_strings[]=
{
  {"NONE",CHANNEL_INVALID},
  DOME(MONO),
  DOME(STEREO),
  DOME(2F_1R),
  DOME(3F),
  DOME(3F_1R),
  DOME(2F_2R),
  DOME(3F_2R),
  DOME(3F_2R_LFE),
  DOME(DOLBY_SURROUND),
  DOME(DOLBY_PROLOGIC),
  DOME(DOLBY_PROLOGIC2)

};



extern int DIA_getLameSettings(int *pmode, int *pbitrate,ADM_LAME_PRESET *preset);
extern int DIA_getVorbisSettings(int *pbitrate, int *mode);
extern void UI_PrintCurrentACodec( const char *s);



AUDIOENCODER  activeAudioEncoder=  AUDIOENC_NONE;
/*----------------------------------*/
int  audioNormalizeMode = 0;
int  audioFreq=48000;
int  audioDRC = 0;
FILMCONV audioFilmConv=FILMCONV_NONE;

static RESAMPLING  audioResampleMode = RESAMPLING_NONE;
int audioMP3mode = 0;
int audioMP3bitrate = 128;
static ADM_LAME_PRESET audioMP3preset=ADM_LAME_PRESET_CBR;
CHANNEL_CONF audioMixing=CHANNEL_INVALID;
// These are globals for the moment
int 	   audioShift = 0;
int	   audioDelay=0;
//**********

const char              *getCurrentMixerString(void)
{
        uint32_t nb=sizeof(Mixer_strings)/sizeof(Mixer_String);
        for(uint32_t i=0;i<nb;i++)
                if(audioMixing==Mixer_strings[i].conf) return Mixer_strings[i].name;
        ADM_assert(0);

}
uint8_t                    setCurrentMixerFromString(const char *name)
{
        uint32_t nb=sizeof(Mixer_strings)/sizeof(Mixer_String);
        for(uint32_t i=0;i<nb;i++)
                if(!strcasecmp(name,Mixer_strings[i].name)) 
                {
                  audioMixing= Mixer_strings[i].conf;
                  return 1;
                }
        return 0;

}
//**********

const char              *audioSourceFromEnum(AudioSource src)
{
        uint32_t nb=sizeof(Sources)/sizeof(externalSource);
        for(uint32_t i=0;i<nb;i++)
                if(src==Sources[i].type) return Sources[i].name;
        ADM_assert(0);

}
AudioSource             audioSourceFromString(const char *name)
{
        uint32_t nb=sizeof(Sources)/sizeof(externalSource);
        for(uint32_t i=0;i<nb;i++)
                if(!strcasecmp(name,Sources[i].name)) return Sources[i].type;
        return (AudioSource)0;

}

//**********
uint8_t audioReset(void )
{
        audioNormalizeMode=0;
        audioResampleMode = RESAMPLING_NONE;
        audioFilmConv=FILMCONV_NONE;
        audioMixing=CHANNEL_INVALID;
        return 1;
}
//************
uint8_t audioGetNormalize(void)
{
        return audioNormalizeMode;

}
uint8_t audioGetDownsample(void)
{
        switch(audioResampleMode)
        {
        case RESAMPLING_CUSTOM:
        case RESAMPLING_NONE: return 0;
        case RESAMPLING_DOWNSAMPLING: return 1;
        default: ADM_assert(0);
        }
        return 0;
        
}
uint32_t audioGetResample(void)
{
      return audioFreq;
        
}
uint32_t audioGetDelay(void)
{
        if(audioShift && audioDelay)
        {
                return audioDelay;
                
        }
        return 0;
}
FILMCONV audioGetFpsConv(void)
{
        return audioFilmConv;
}

uint32_t audioGetBitrate(void)
{
        return audioMP3bitrate;
} 
/*----------------------------------*/
//


void audioCodecChanged(int newcodec)
{
	ADM_assert(newcodec<sizeof(myCodecList) /sizeof(CODECLIST));
	activeAudioEncoder=myCodecList[newcodec].codec;

	if (activeAudioEncoder == AUDIOENC_VORBIS) {
		if (audioMP3mode == 0 && audioMP3bitrate > 11)
			audioMP3bitrate = 4;
	} else {
		if (audioMP3bitrate < 48)
			audioMP3bitrate = 128;
	}
}

uint32_t audioFilterGetNbEncoder(void)
{
	return sizeof(myCodecList) /sizeof(CODECLIST);
}
const char* audioFilterGetIndexedName(uint32_t i)
{
 	ADM_assert(i<sizeof(myCodecList) /sizeof(CODECLIST));
	return myCodecList[i].menuName;
}

void audioFilterNormalize(uint8_t onoff)
{
	audioNormalizeMode=onoff;
}

uint8_t audioFilterDelay(int32_t delay)
{	
	if(delay)
	{
		audioShift=1;
		audioDelay=delay;
	}
	else
	{
		audioShift=audioDelay=0;
	}
	return 1;

}
RESAMPLING  audioGetResampling(void)
{
        return audioResampleMode;
}
uint8_t audioFilterFilm2Pal(uint8_t onoff)
{
	if(onoff) audioFilmConv=FILMCONV_FILM2PAL;
	else audioFilmConv=FILMCONV_NONE;
	return 1;
}
uint8_t audioFilterPal2Film(uint8_t onoff)
{
        if(onoff) audioFilmConv=FILMCONV_PAL2FILM;
        else audioFilmConv=FILMCONV_NONE;
        return 1;
}

void audioFilterDownsample(uint8_t onoff)
{
	if(onoff)
		audioResampleMode=RESAMPLING_DOWNSAMPLING;
	else
		audioResampleMode=RESAMPLING_NONE;
}
void audioFilterResample(uint32_t onoff)
{
	if(onoff)
	{
		audioResampleMode=RESAMPLING_CUSTOM;
		audioFreq=onoff;
	}
	else
		audioResampleMode=RESAMPLING_NONE;

} 
//______________________________
void audioFilter_SetBitrate( int i)
{
	audioMP3bitrate=i;
}
#include "ADM_gui2/GUI_ui.h"
extern  int DIA_getAudioFilter(int *normalized, RESAMPLING *downsamplingmethod, int *tshifted,
  			 int *shiftvalue, int *drc,int *freqvalue,FILMCONV *filmconv,CHANNEL_CONF *channel);

void audioFilter_configureFilters( void )
{
    int olddelay=audioDelay;
    int oldshift=audioShift;
	 DIA_getAudioFilter(&audioNormalizeMode,&audioResampleMode,&audioShift,&audioDelay,&audioDRC,&audioFreq,
	 		&audioFilmConv,&audioMixing );
         if(audioDelay!=olddelay ||oldshift!= audioShift)
         {  // Refresh
             
             UI_setTimeShift(audioShift,audioDelay);
             
         }

}

/*

*/

uint8_t audioCodecSetByName( const char *name)
{
		for(uint32_t i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
		{
			if(!strcasecmp(name,myCodecList[i].name))
			{

				audioCodecSetcodec(myCodecList[i].codec);
				return 1;
			}

		}
		printf("\n Mmmm Select audio codec by name failed...(%s).\n",name);
		return 0;
}
AudioSource audioCodecGetFromName( const char *name)
{
                for(uint32_t i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
                {
                        if(!strcasecmp(name,Sources[i].name))
                        {

                                return Sources[i].type;
                        }

                }
                printf("\n Mmmm Select audio codec by name failed...(%s).\n",name);
                return AudioNone;
}
	#define Read(x) { \
		tmp=name; \
		if((tmp=strstr(name,#x))) \
			{ \
				tmp+=strlen(#x); \
				aprintf("-- %s\n",tmp); \
				sscanf(tmp,"=%d ",&x); \
			} \
			 else \
			{ printf("*** %s not found !***\n",#x);} \
		}
#define Add(x) {sprintf(tmp,"%s=%d ",#x,x);strcat(conf,tmp);}


const char *audioCodecGetName( void )
{
	for(uint32_t i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
	{
		if(activeAudioEncoder==myCodecList[i].codec)
		{
			return myCodecList[i].name;
		}

	}
	printf("\n Mmmm get audio  codec  name failed..\n");
	return NULL;
}
uint8_t audioFilterSetByName( const char *name)
{
	const char *tmp;
	aprintf("-Audio filter by name : %s\n",name);

	Read(audioNormalizeMode);
	Read(audioResampleMode);
	Read(audioDRC);
	Read(audioShift);
	Read(audioDelay);
	Read(audioFreq);
	Read(audioMixing);
	return 1;
}

const char *audioFilterGetName( void )
{
	static char conf[400];
	static char tmp[200];
	conf[0]=0;
	#undef Add
	#define Add(x) {sprintf(tmp,"%s=%d ",#x,x);strcat(conf,tmp);}
	Add(audioNormalizeMode);
	Add(audioResampleMode);
	Add(audioDRC);
	Add(audioShift);
	Add(audioDelay);
	Add(audioFreq);
	Add(audioMixing);
	return conf;

}
void audioPrintCurrentCodec(void)
{

	for(uint32_t i=0;i<sizeof(myCodecList)/sizeof(CODECLIST);i++)
	{
		if(activeAudioEncoder==myCodecList[i].codec)
		{
			UI_setAudioCodec(i);
			return;
		}
	
	}
	ADM_assert(0);
}
 void audioCodecSetcodec(AUDIOENCODER codec)
{

	activeAudioEncoder=codec;
	audioPrintCurrentCodec();

}

void audioCodecSelect( void )
{
	DIA_audioCodec( &activeAudioEncoder );
	audioPrintCurrentCodec();


}
uint32_t audioProcessMode(void)
{
        if(activeAudioEncoder==AUDIOENC_COPY) return 0;
        return 1;
}
/*
	Refresh   activeAudioEncoder value
	depending on what's selected

*/
	 

/* _____________________________________________________
  		Handle the configure signal from GUI

				pop up GUI 

*/
void audioCodecConfigure( void )
{
	int mode,bitrate;
	ADM_LAME_PRESET preset;
	bitrate= audioMP3bitrate;
	preset=audioMP3preset;
	mode=audioMP3mode;
	
	switch(activeAudioEncoder)
	{
                case AUDIOENC_LPCM:
		case AUDIOENC_NONE:
		case AUDIOENC_COPY:
								return;

#ifdef USE_FAAC
		case AUDIOENC_FAAC:
						
							DIA_audioEncoder(&audioMP3mode, &audioMP3bitrate,"AAC parameter");
							return;
						return;
#endif	
#ifdef USE_VORBIS
		case AUDIOENC_VORBIS:
						
							DIA_getVorbisSettings(&audioMP3bitrate, &audioMP3mode);
							return;
						return;
#endif		
#ifdef HAVE_LIBMP3LAME								
		case AUDIOENC_MP3:
						DIA_getLameSettings(&audioMP3mode, &audioMP3bitrate,&audioMP3preset);
						return;
						break;
#endif

		case AUDIOENC_MP2:
							if (DIA_audioEncoder(&mode, &bitrate,"MP2 parameter"))
								{
									audioMP3mode=mode;
									audioMP3bitrate=bitrate;
								}
								return;
		case AUDIOENC_2LAME:
							if (DIA_audioEncoder(&mode, &bitrate,"Toolame parameter"))
								{
									audioMP3mode=mode;
									audioMP3bitrate=bitrate;
								}
								return;								
		case AUDIOENC_AC3:
							if (DIA_audioEncoder(&mode, &bitrate,"AC3 parameter"))
								{
									audioMP3mode=mode;
									audioMP3bitrate=bitrate;										
								}
								return;
		default:
					ADM_assert(0);																				
		
	}
	
}
void audioForceDownSample( void)
{
	audioResampleMode=RESAMPLING_DOWNSAMPLING;
}
void audioSetResample(uint32_t fq)
{

	audioResampleMode=RESAMPLING_CUSTOM;
	audioFreq=fq;
}
uint8_t audioLamePreset(const char *name)
{
#ifdef HAVE_LIBMP3LAME
int nb=sizeof(presetDefinition)/sizeof(ADM_PRESET_DEFINITION);
        for(int i=0;i<nb;i++)   
        {
                if(!strcasecmp(presetDefinition[i].name,name))
                        {
                           audioMP3preset=    presetDefinition[i].preset;
                           return 1; 
                        }
        }
//
        printf("AudioLame : Unknown preset  :%s\n",name);
#endif
        return 0;
}

