//
// C++ Implementation: ADM_deviceWin32
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
//
// C++ Implementation: ADM_deviceSDL
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"
#ifdef CYG_MANGLING

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>


#include <assert.h>
  
#include "fourcc.h" 
 

#include "windows.h"
#include "winbase.h"


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_audiodevice/ADM_deviceoss.h"
#include "ADM_audiodevice/ADM_deviceWin32.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME  MODULE_ADEVICE
#include "ADM_toolkit/ADM_debug.h"

static HWAVEOUT  mydevice;

//_______________________________________________
//
//_______________________________________________


//_______________________________________________
//
//_______________________________________________

win32AudioDevice::win32AudioDevice(void) 
{
	printf("Creating SDL Audio device\n");
	_inUse=0;
	
}
//_______________________________________________
//
//_______________________________________________
uint8_t  win32AudioDevice::stop(void) 
{
	
	printf("Closing win32 audio\n");
	 waveOutPause(  mydevice);
	 if(MMSYSERR_NOERROR!= waveOutClose(mydevice))
	 {
	 	printf("Close failed\n");
		return 0;
	 }
	
	_inUse=0;    	

	return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t sdlAudioDevice::init(uint32_t channel, uint32_t fq) 
{
	
SDL_AudioSpec spec,result;
PWAVEFORMATEX wav;
		
		printf("Opening Win32 Audio, fq=%d\n",fq);
		if(_inUse) 
		{
			printf("Already running ?\n");
			return 1; // ???
		}
		memset(&wav,0,sizeof(wav));
		wav->dwFrequency=fq;
		wav->dwChannels=channel;
		wav->swbyterate=fq*channel*2;
		
		if(MMSYSERR_NOERROR!= waveOutOpen(&myDevice,WAVE_MAPPER,
  
  				&wav,	//L pwfx,      
  				NULL, //DWORD_PTR      dwCallback,
  				0,	//DWORD_PTR      dwCallbackInstance,
  				WAVE_MAPPED //DWORD          fdwOpen    
			)
			{
				printf("WavoutOpen failed\n");
				return 0;
			}
		
	
    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t sdlAudioDevice::play(uint32_t nb,uint8_t * ptr)
 {
	WAVEHDR hdr;
	
	memset(&hdr,0,sizeof(hdr));
	hdr.lpData=ptr;
	hdr.dwBufferLength=nb;
	
	if( MMSYSERR_NOERROR!=waveOutWrite(
  					myDevice, //HWAVEOUT hwo,  
  					&hdr, //LPWAVEHDR pwh, 
  					sizeof(hdr) //UINT cbwh      
			))
			{
			
				printf("win32Audio write failed\n");
				return 0;
			}
			
	return 1;
}
#else
void dummy_as_funcZ( void);
void dummy_as_funcY( void)
 {
}

#endif

