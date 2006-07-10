/**************************************************************************
                          audioeng_buildfilters.h  -  description
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
#ifndef ADM_AUDIO_BUILDFILTER_H
#define  ADM_AUDIO_BUILDFILTER_H

#include "ADM_audiofilter/audioeng_process.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_audiofilter/audioeng_process.h"



 AVDMProcessAudioStream *buildAudioFilter(AVDMGenericAudioStream *currentaudiostream,
				uint32_t starttime, uint32_t duration);
 AUDMAudioFilter *buildPlaybackFilter(AVDMGenericAudioStream *currentaudiostream,
				uint32_t starttime, uint32_t duration);

void deleteAudioFilter(void);								
void audioFilter_configureFilters( void );

// Build a simple filter chain
// That is starting from startTime in ms, has a duration of duration ms and is shifter
// by shift ms
AVDMGenericAudioStream *buildRawAudioFilter( uint32_t startTime, uint32_t duration, int32_t shift);				


void audioCodecConfigure( void );
void audioCodecSelect( void );

typedef enum AUDIOENCODER
{
	AUDIOENC_NONE,
	AUDIOENC_MP3,
	AUDIOENC_MP2,
	AUDIOENC_AC3,
	AUDIOENC_2LAME,
	AUDIOENC_FAAC,
	AUDIOENC_VORBIS,
        AUDIOENC_COPY,
        AUDIOENC_LPCM,
	AUDIOENC_DUMMY
}AUDIOENCODER;

typedef enum RESAMPLING
{
	RESAMPLING_NONE=0,
	RESAMPLING_DOWNSAMPLING=1, // Need to hardcode downsampling to 2 for automation
	RESAMPLING_CUSTOM=2,
	RESAMPLING_LAST
}RESAMPLING;

typedef enum FILMCONV
{
	FILMCONV_NONE=0,
	FILMCONV_FILM2PAL=1,
	FILMCONV_PAL2FILM=2,
	FILMCONV_LAST
}FILMCONV;

typedef enum
{
        AudioInvalid,
        AudioAvi=1,
        AudioMP3,
        AudioWav,
        AudioAC3,
        AudioNone

}AudioSource;

void audioCodecSetcodec(AUDIOENCODER codec);
uint8_t audioReset(void);
/*  for job/workspace stuff  */
uint8_t audioCodecSetByName( const char *name);
AudioSource audioCodecGetFromName( const char *name);
const char *audioCodecGetName( void );

const char *audioFilterGetName( void );
uint8_t audioFilterSetByName( const char *name);

const char *audioCodecGetConf( void );
uint8_t audioCodecSetConf(char *conf );

AVDMGenericAudioStream *mpt_getAudioStream(void);

/* -- Set filter --*/
void audioFilterNormalize(uint8_t onoff);
void audioFilterDownsample(uint8_t onoff);
void audioFilterResample(uint32_t onoff);
uint8_t audioFilterDelay(int32_t delay);
uint8_t audioFilterFilm2Pal(uint8_t onoff);
uint8_t audioFilterPal2Film(uint8_t onoff);

uint8_t audioFilterMono2Stereo(uint8_t onoff);
uint8_t audioFilterStereo2Mono(uint8_t onoff);

void audioFilter_SetBitrate( int i);
/* -- Get filter -- */
uint32_t audioGetBitrate(void);
uint8_t audioGetNormalize(void);
uint8_t audioGetDownsample(void);
uint32_t audioGetResample(void);
uint32_t audioGetDelay(void);
FILMCONV audioGetFpsConv(void);
RESAMPLING  audioGetResampling(void);
/*-----*/
uint8_t                 A_changeAudioStream(AVDMGenericAudioStream *newaudio,AudioSource so,char *name);
AudioSource             getCurrentAudioSource(char **name);
const char              *audioSourceFromEnum(AudioSource src);
AudioSource             audioSourceFromString(const char *name);
const char              *getCurrentMixerString(void);
uint8_t                 setCurrentMixerFromString(const char *string);
uint8_t                 audioLamePreset(const char *name);
//
#endif

