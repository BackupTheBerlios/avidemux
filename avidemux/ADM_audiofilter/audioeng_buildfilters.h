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

#include "ADM_audiofilter/audioprocess.hxx"

 AVDMProcessAudioStream *buildAudioFilter(AVDMGenericAudioStream *currentaudiostream,
				uint32_t starttime, uint32_t size);
AVDMProcessAudioStream *buildPlaybackFilter(AVDMGenericAudioStream *currentaudiostream,
				uint32_t starttime, uint32_t size);

AVDMProcessAudioStream *buildInternalAudioFilter(AVDMGenericAudioStream *currentaudiostream,
				uint32_t starttime, uint32_t size);				

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

void audioCodecSetcodec(AUDIOENCODER codec);

/*  for job/workspace stuff  */
uint8_t audioCodecSetByName( const char *name);
const char *audioCodecGetName( void );

const char *audioFilterGetName( void );
uint8_t audioFilterSetByName( const char *name);

const char *audioCodecGetConf( void );
uint8_t audioCodecSetConf(char *conf );

AVDMGenericAudioStream *mpt_getAudioStream(double *mypcm,uint8_t silent=0);

void audioFilterNormalize(uint8_t onoff);
void audioFilterDownsample(uint8_t onoff);
void audioFilterResample(uint32_t onoff);
uint8_t audioFilterDelay(int32_t delay);
uint8_t audioFilterFilm2Pal(uint8_t onoff);

void audioFilter_SetBitrate( int i);

