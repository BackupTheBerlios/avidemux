//
// C++ Implementation: ADM_deviceSDL
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004-2008
//
// Copyright: See COPYING file that comes with this distribution
//
//


#include "ADM_default.h"
#include "SDL.h"

#include  "ADM_audiodevice.h"
#include  "ADM_audioDeviceInternal.h"

#include "ADM_deviceSDL.h"

static int16_t  		*audioBuffer=NULL;
static uint32_t 		frameCount=0;
static uint32_t			rd_ptr=0;
static uint32_t			wr_ptr=0;

extern "C"
{
static void SDL_callback(void *userdata, Uint8 *stream, int len);
}

#ifdef __APPLE__
#define BUFFER_SIZE (500*48000)
#else
#define BUFFER_SIZE (2*48000)
#endif

ADM_DECLARE_AUDIODEVICE(Sdl,sdlAudioDevice,1,0,0,"Sdl audio device (c) mean");

#define aprintf(...) {}
//_______________________________________________
//
//_______________________________________________


//_______________________________________________
//
//_______________________________________________

sdlAudioDevice::sdlAudioDevice(void) 
{
	printf("Creating SDL Audio device\n");
	_inUse=0;
	wr_ptr=rd_ptr=0;
}
//_______________________________________________
//
//_______________________________________________
uint8_t  sdlAudioDevice::stop(void) 
{
	if(audioBuffer)
	{
		delete [] audioBuffer;
		audioBuffer=NULL;
	}
	
		SDL_PauseAudio(1); // First pause it
		SDL_CloseAudio();
		SDL_QuitSubSystem(SDL_INIT_AUDIO);
	
	// Clean up
	//CloseComponent(theOutputUnit);
	_inUse=0;
    	wr_ptr=rd_ptr=0;
	printf("Closing SDL audio\n");
	return 1;
}
//_______________________________________________
//  We have to fill len data to stream pointer
//_______________________________________________
void SDL_callback(void *userdata, Uint8 *stream, int len)
{


	
	uint32_t nb_sample=len>>1;
	uint32_t left=0;
	uint8_t *in,*out;

	in=(uint8_t *)&audioBuffer[rd_ptr];
	out=(uint8_t *)stream;
	aprintf("sdl : Fill : rd %lu wr:%lu nb asked:%lu \n",rd_ptr,wr_ptr,nb_sample);
	if(wr_ptr>rd_ptr)
	{
		left=wr_ptr-rd_ptr-1;	
		if(left>nb_sample)
		{
			memcpy(out,in,nb_sample*2);
			rd_ptr+=nb_sample;
		}
		else
		{
			memcpy(out,in,left*2);
			memset(out+left*2,0,(nb_sample-left)*2);
			rd_ptr+=left;
		}
	}
	else
	{
		// wrap
		left=BUFFER_SIZE-rd_ptr-1;
		if(left>nb_sample)
		{
			memcpy(out,in,nb_sample*2);
			rd_ptr+=nb_sample;
		}
		else
		{
			memcpy(out,in,left*2);
			out+=left*2;
			rd_ptr=0;
			in=(uint8_t *)&audioBuffer[0];
			nb_sample-=left;
			if(nb_sample>wr_ptr-1) nb_sample=wr_ptr-1;
			memcpy(out,in,nb_sample*2);
			rd_ptr=nb_sample;	
		}
	}
	

}

//_______________________________________________
//
//
//_______________________________________________
uint8_t sdlAudioDevice::init(uint32_t channels, uint32_t fq) 
{
SDL_AudioSpec spec,result;
_channels = channels;
		
		printf("[SDL] Opening audio, fq=%d\n",fq);

		if(_inUse) 
		{
			printf("[SDL] Already running ?\n");
			return 1; // ???
		}
		
		if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) 
		{
			printf("[SDL] FAILED initialising Audio subsystem\n");
			printf("[SDL] ERROR: %s\n", SDL_GetError());
			return 0;
		}
		memset(&spec,0,sizeof(spec));
		memset(&result,0,sizeof(result));
		spec.freq=fq;
		spec.channels=channels;
		spec.samples=65536>>4; // 1 second worth of audio
		spec.callback=SDL_callback;
		spec.userdata=NULL;
		spec.format=AUDIO_S16;
	
		int res=SDL_OpenAudio(&spec,&result);
		if(res<0)
		{
			printf("[SDL] Audio device FAILED to open\n");
			printf("[SDL] ERROR: %s\n", SDL_GetError());

			printf("fq   %d \n",spec.freq);
			printf("chan %d \n", spec.channels);
			printf("samples %d \n",spec.samples);
			printf("format %d \n",spec.format);
			
			printf("fq   %d \n",result.freq);
			printf("chan %d \n", result.channels);
			printf("samples %d \n",result.samples);
			printf("format %d \n",result.format);

			return 0;
		}
		
	frameCount=0;
	
	audioBuffer=new int16_t[BUFFER_SIZE]; // between hald a sec and a sec should be enough :)
	
    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t sdlAudioDevice::play(uint32_t len, float *data)
 {
 	// First put stuff into the buffer
	uint8_t *src;
	uint32_t left;

	dither16(data, len, _channels);
	
	// Check we have room left
	if(wr_ptr>=rd_ptr)
	{
		left=BUFFER_SIZE-(wr_ptr-rd_ptr);
	}
	else
	{
		left=rd_ptr-wr_ptr;
	}
	if(len+1>left)
	{
		printf("AudioCore:Buffer full!\n");
		
		return 0;
	}

	// We have room left, copy it
	src=(uint8_t *)&audioBuffer[wr_ptr];
	
	SDL_LockAudio();
	
	if(wr_ptr+len<BUFFER_SIZE)
	{
		memcpy(src,data,len*2);
		wr_ptr+=len;
	}
	else
	{
		left=BUFFER_SIZE-wr_ptr-1;
		memcpy(src,data,left*2);
		memcpy(audioBuffer,data+left*2,(len-left)*2);
		wr_ptr=len-left;	
	}
	
	//aprintf("AudioSDL: Putting %lu bytes rd:%lu wr:%lu \n",len*2,rd_ptr,wr_ptr);
	SDL_UnlockAudio();
 	if(!frameCount)
	{
		_inUse=1;
		SDL_PauseAudio(0);;
	}
	
	return 1;
}

uint8_t sdlAudioDevice::setVolume(int volume){
#ifdef OSS_SUPPORT
        ossAudioDevice dev;
        dev.setVolume(volume);
#else
#ifdef ALSA_SUPPORT
        alsaAudioDevice dev;
        dev.setVolume(volume);
#endif
#endif
	return 1;
}



