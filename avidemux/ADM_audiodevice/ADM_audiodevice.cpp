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


audioDevice *device=NULL;
static AUDIO_DEVICE  currentDevice=DEVICE_DUMMY;
static AUDIO_DEVICE ADM_audioByName(const char *name);
static const char *ADM_audioById(AUDIO_DEVICE id);
extern void DIA_getAudioDevice(AUDIO_DEVICE *id);

void AVDM_audioSave( void )
{
const char *string;
		string=ADM_audioById(currentDevice);
		prefs->set(DEVICE_AUDIODEVICE, string);
	

}
AUDIO_DEVICE ADM_audioByName(const char *name)
{
	if(!name) return (AUDIO_DEVICE)0;
	for(uint32_t i=0;i<sizeof(audioDeviceList)/sizeof(DEVICELIST);i++)
	{
		if(!strcmp(name,audioDeviceList[i].name))
		{
			return audioDeviceList[i].id;
		}	
	}
	printf("Device not found :%s\n",name);
	return (AUDIO_DEVICE)0;

}
const char *ADM_audioById(AUDIO_DEVICE id)
{
	
	for(uint32_t i=0;i<sizeof(audioDeviceList)/sizeof(DEVICELIST);i++)
	{
		if(audioDeviceList[i].id==id)
		{
			return audioDeviceList[i].name;
		}	
	}
	printf("Device not found :%d\n",id);
	return (const char *)"Unknown!";

}

void AVDM_audioPref( void )
{
AUDIO_DEVICE newDevice=currentDevice;

	DIA_getAudioDevice(&newDevice);
	if(newDevice!=currentDevice)
	{
		AVDM_switch(newDevice);
	}
}
void AVDM_audioInit(void )
{
uint8_t init=0;
char *name=NULL;
AUDIO_DEVICE id;

		if(prefs->get(DEVICE_AUDIODEVICE, &name))
		{
		id=ADM_audioByName(name);
		free(name);
		name=NULL;	
		if(!id) id=DEVICE_DUMMY;
		switch(id)
		{

			case DEVICE_ARTS:									
			case DEVICE_ALSA:
			case DEVICE_COREAUDIO:
			case DEVICE_SDL:
			
						printf("Using real audio device\n");
						AVDM_switch(id);
						init=1;	
						break;				
			case DEVICE_DUMMY:
			default:
						printf("Using dummy audio device\n");
						init=1;
						AVDM_switch(id);
						break;
		
		}
		}
		// Fallback
		if(init==0)
		{
		#ifdef OSS_SUPPORT
			AVDM_switch(DEVICE_OSS);			
			printf("\n Using OSS\n");
		#else
			AVDM_switch(DEVICE_DUMMY);
			printf("\n Using dummy\n");
		#endif
		}
}
// Switch the audio device class we are using
//
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
#ifdef CONFIG_DARWIN
		  case  DEVICE_COREAUDIO :
								device=new 	 coreAudioDevice;
								currentDevice=DEVICE_COREAUDIO;;
								printf("Using Darwin coreaudio i/f\n");
								break;

#endif
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
#ifdef USE_SDL
		case DEVICE_SDL:
								

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
