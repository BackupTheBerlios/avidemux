/***************************************************************************
                          ADM_audiodevice.cpp  -  description
                             -------------------
    begin                : Sat Sep 28 2002
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
#ifdef USE_ARTS
#include <artsc.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>


#include "avi_vars.h"
#include "prefs.h"
#ifdef HAVE_AUDIO


#include "ADM_audiodevice/ADM_deviceoss.h"

#ifdef USE_ARTS
#include "ADM_audiodevice/ADM_deviceArts.h"
#endif

#ifdef ALSA_SUPPORT
#include "ADM_audiodevice/ADM_deviceALSA.h"
#endif

#include "gui_action.hxx"
#include "audio_out.h"
/**
		in=0 -> arts1
		in=1 -> alsa
*/
extern void GUI_SetArts( uint8_t in );

audioDevice *device=NULL;
static AUDIO_DEVICE  currentDevice=DEVICE_DUMMY;

void AVDM_audioSave( void )
{
	switch(currentDevice)
	{
		case DEVICE_ALSA:
					prefs->set(DEVICE_AUDIODEVICE, "ALSA");
					break;
		case DEVICE_ARTS:
					prefs->set(DEVICE_AUDIODEVICE, "Arts");
					break;
		default :
					prefs->set(DEVICE_AUDIODEVICE, "Oss");
					break;

	}

}

void AVDM_audioInit(void )
{
uint8_t init=0;
char *name=NULL;
		if(prefs->get(DEVICE_AUDIODEVICE, &name))
		{
		if(!strcmp(name,"Arts"))
			{
				printf("\n Using Arts Mmm ?\n");
				AVDM_switch(DEVICE_ARTS);
				GUI_SetArts(  0);
				init=1;
			}
		else if(!strcmp(name,"ALSA"))
			{
				printf("\n Using ALSA, toying with kernel 2.5 are you :)\n");
				AVDM_switch(DEVICE_ALSA);
				GUI_SetArts(  1);
				init=1;
			}
			free(name);
			name=NULL;
		}
		if(init==0)
		{
		#ifdef OSS_SUPPORT
			AVDM_switch(DEVICE_OSS);
			GUI_SetArts(  2);
			printf("\n Using OSS\n");
		#else
			AVDM_switch(DEVICE_DUMMY);
			printf("\n Using dummy\n");
		#endif
		}

}

void AVDM_switch(AUDIO_DEVICE action)
{
	if(device)
	       {
				delete device;
			}
	device=NULL;
	 currentDevice=DEVICE_DUMMY;
	switch(action)
	{
#ifdef OSS_SUPPORT
		  case  DEVICE_OSS :
								device=new 	 ossAudioDevice;
								currentDevice=DEVICE_OSS;;
								break;
#endif
#ifdef USE_ARTS
		case DEVICE_ARTS:
								device=new 	 artsAudioDevice;
							 	currentDevice=DEVICE_ARTS;
								break;

#endif
#ifdef ALSA_SUPPORT
		case DEVICE_ALSA:
								device=new 	 alsaAudioDevice;
							 	currentDevice=DEVICE_ALSA;
								break;

#endif

		 case  DEVICE_DUMMY:
					default:
								device=new 	 dummyAudioDevice;
								currentDevice=DEVICE_DUMMY;
								break;

	}

}

//_______________________________________________
//
//
//_______________________________________________
void AVDM_AudioClose(void)
{
	device->stop();
}

//_______________________________________________
//
//
//_______________________________________________
uint32_t AVDM_AudioSetup(uint32_t fq, uint32_t channel)
{
	if(channel>2) channel=2;
	return device->init(channel,fq);
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t AVDM_AudioPlay(uint8_t * ptr, uint32_t nb)
{
	return device->play(nb,ptr);
}
#else
void dummy_func_make_gcc_happy( void );
void dummy_func_make_gcc_happy( void )
{
}

#endif
