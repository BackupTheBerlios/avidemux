/***************************************************************************
                        audioouput.h  -  description
                             -------------------
    begin                : Thu Dec 27 2001
    copyright            : (C) 2001 by mean
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

#ifndef AUDIOOUPUT_H
#define AUDIOOUPUT_H
//#include "avi_vars.h"
#include "ADM_audiodevice.h"
#ifdef HAVE_AUDIO

void 		AVDM_audioSave( void );
void 		AVDM_audioInit( void );
void 		AVDM_switch( AUDIO_DEVICE action );

uint8_t 	AVDM_AudioPlay(uint8_t *ptr, uint32_t nb);
uint32_t 	AVDM_AudioSetup(uint32_t fq, uint32_t channel);
void 		AVDM_AudioClose(void);
AUDIO_DEVICE 	AVDM_getCurrentDevice( void);
uint8_t         AVDM_setVolume(int volume);

#endif
#endif
