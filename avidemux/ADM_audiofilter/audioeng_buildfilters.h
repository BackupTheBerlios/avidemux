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


void audioCodecConfigure( void );
void audioCodecSelect( void );

typedef enum AUDIOENCODER
{
	AUDIOENC_NONE,
	AUDIOENC_MP3,
	AUDIOENC_MP2,
	AUDIOENC_AC3,
	AUDIOENC_2LAME,
	AUDIOENC_DUMMY
}AUDIOENCODER;

typedef enum RESAMPLING
{
	RESAMPLING_NONE=0,
	RESAMPLING_DOWNSAMPLING=1, // Need to hardcode downsampling to 2 for automation
	RESAMPLING_CUSTOM=2,
	RESAMPLING_LAST
}RESAMPLING;

void audioCodecSetcodec(AUDIOENCODER codec);

/*  for job/workspace stuff  */
uint8_t audioCodecSetByName( const char *name);
const char *audioCodecGetName( void );

const char *audioFilterGetName( void );
uint8_t audioFilterSetByName( const char *name);

const char *audioCodecGetConf( void );
uint8_t audioCodecSetConf(char *conf );

