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
// C++ Implementation: ADM_deviceWin32
// Use MM layer to output sound
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
#ifdef ADM_WIN32

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>



  
#include "fourcc.h" 
 

#include "windows.h"
#include "winbase.h"


#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_audiodevice.h"
#include "ADM_audiodevice/ADM_deviceWin32.h"
#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME  MODULE_ADEVICE
#include "ADM_osSupport/ADM_debug.h"

#define BUCKET_SIZE 25000*4   // 1 sec
#define NB_BUCKET 30

typedef struct audioBucket
{
    int         fre;
    WAVEHDR     hdr;
}audioBucket;
static void CALLBACK waveOutProc(
  HWAVEOUT hwo,      
  UINT uMsg,         
  DWORD_PTR dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
);
static HWAVEOUT  myDevice;
static MMRESULT    myError;
static void 	handleMM(MMRESULT  err);
static int      first=0;

audioBucket buckets[NB_BUCKET];	
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
	first=0;
	memset(buckets,0,sizeof(buckets));
	
}
//_______________________________________________
//
//_______________________________________________
uint8_t  win32AudioDevice::stop(void) 
{
    if(!_inUse) return 0;  	
	printf("Closing win32 audio\n");
	 waveOutPause(  myDevice);
	 myError=waveOutClose(myDevice);
	 first=0;
	 if(MMSYSERR_NOERROR!= myError)
	 {
	 	printf("Close failed\n");
	 	handleMM(myError);
		return 0;
	 }
	
	_inUse=0;    	
	for(uint32_t i=0;i<NB_BUCKET;i++)
	{
		 delete []    buckets[i].hdr.lpData;
	}
	return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t win32AudioDevice::init(uint8_t channels, uint32_t fq) 
{
WAVEFORMATEX wav;
		
	_channels = channels;

		printf("Opening Win32 Audio, fq=%d\n",fq);

		if(_inUse) 
		{
			printf("Already running ?\n");
			return 1; // ???
		}
		first=0;
/*
	WORD wFormatTag;
	WORD nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD wBitsPerSample;
	WORD cbSize;
*/		
		memset(&wav,0,sizeof(wav));
		
		wav.wFormatTag=WAVE_FORMAT_PCM;
		wav.nSamplesPerSec=fq;
		wav.nChannels=channels;
		wav.nBlockAlign=2*channels;
		wav.nAvgBytesPerSec=2*channels*fq;		
		wav.wBitsPerSample=16;
		
		myError=waveOutOpen(&myDevice,WAVE_MAPPER,
  
  				&wav,	//L pwfx,      
  				(DWORD_PTR)waveOutProc , //0, //DWORD_PTR      dwCallback,
  				0,	//DWORD_PTR      dwCallbackInstance,
  				CALLBACK_FUNCTION //CALLBACK_NULL //WAVE_MAPPED //DWORD          fdwOpen    
			);
		if(MMSYSERR_NOERROR!=myError )
			{
				printf("WavoutOpen failed\n");
			 	handleMM(myError);
				return 0;
			}
    WAVEHDR *hdr;
			
	for(uint32_t i=0;i<NB_BUCKET;i++)
	{
		 buckets[i].fre=1;
		 hdr=&(buckets[i].hdr);
		 waveOutPrepareHeader(myDevice,hdr,sizeof(*hdr));	
         hdr->lpData=(CHAR *)new uint8_t [BUCKET_SIZE];
         hdr->dwBufferLength=0;
	}

	
    return 1;
}

uint8_t  win32AudioDevice::setVolume(int volume) 
{
    DWORD value;
    value=0xffff;
    value*=volume;
    value/=10;
    value=value+(value<<16);
    waveOutSetVolume(myDevice,value);
	return 1;
}
//_______________________________________________
//
//
//_______________________________________________
uint8_t win32AudioDevice::play(uint32_t len, float *data)
 {
 uint32_t av=0;
	WAVEHDR *hdr;

	dither16(data, len, _channels);
	len = len * 2;
   ADM_assert(len<BUCKET_SIZE);
   // First do clean up
#if 0   
      for(uint32_t i=0;i<NB_BUCKET;i++)
      {
            if(!buckets[i].fre)
            {
                        if(buckets[i].hdr.dwFlags & WHDR_DONE)
                        {
                                                buckets[i].fre=1;
                                                buckets[i].hdr.dwFlags=0; 
                                                printf("Freeing buffer %d  \n",i);   
                                                av++;                                            
                        }            
            }
            else
                        av++;
      }
      printf("Free buffer:%d %d\n",av,NB_BUCKET);
#endif      
      // look up a free buffer
      
   for(uint32_t i=0;i<NB_BUCKET;i++)
   {
      if(buckets[i].fre)
      {
            memcpy(buckets[i].hdr.lpData,data,len);
            buckets[i].hdr.dwBufferLength=len;
            buckets[i].fre=0;
            hdr=&(buckets[i].hdr);
            myError=waveOutWrite(
  					myDevice, //HWAVEOUT hwo,  
  					hdr, //LPWAVEHDR pwh, 
  					sizeof(*hdr) //UINT cbwh      
			  					);
			if( MMSYSERR_NOERROR!=myError)
			{
			    handleMM(myError);
			    return 0;
			}
			return 1;
      }
      
   
   }
    printf("All buckets full !\n");
	return 0;
}
void CALLBACK waveOutProc(
  HWAVEOUT hwo,      
  UINT uMsg,         
  DWORD_PTR dwInstance,  
  DWORD dwParam1,    
  DWORD dwParam2     
)
{
    if(uMsg==WOM_DONE)
    {

        audioBucket *bucket;
                        bucket=(audioBucket *)(dwParam1-sizeof(int));
                        bucket->fre=1;
    }

}
void 	handleMM(MMRESULT  err)
{
 #define ERMM(x) if(err==x) printf(#x"\n");
    ERMM(MMSYSERR_ALLOCATED);
    ERMM(MMSYSERR_BADDEVICEID);
    ERMM(MMSYSERR_NODRIVER);
    ERMM(WAVERR_BADFORMAT);
    ERMM(WAVERR_SYNC);
    

}
#else
void dummy_as_funcZ( void);
void dummy_as_funcY( void)
 {
}

#endif

