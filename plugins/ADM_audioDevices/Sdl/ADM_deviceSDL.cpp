/**
    \file ADM_deviceSDL.cpp 
    \brief SDL audio device plugin

    (C) Mean 2008, fixounet@free.fr
    GPL-v2

*/

#include "ADM_default.h"
#include "SDL.h"

#include  "ADM_audiodevice.h"
#include  "ADM_audioDeviceInternal.h"

#include "ADM_deviceSDL.h"


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


/**
    \fn sdlAudioDevice
    \brief constructor

*/
sdlAudioDevice::sdlAudioDevice(void) 
{
	printf("[SDLAUDIO]Creating SDL Audio device\n");
	_inUse=0;
	
}
/**
    \fn sdlAudioDevice
    \brief destructor

*/
sdlAudioDevice::~sdlAudioDevice(void) 
{
   stop();
}

/**
    \fn stop
    \brief stop audio playback + cleanup buffers

*/
uint8_t  sdlAudioDevice::stop(void) 
{
	
    SDL_PauseAudio(1); // First pause it
    SDL_CloseAudio();
	if(audioBuffer)
	{
		delete [] audioBuffer;
		audioBuffer=NULL;
	}

    SDL_QuitSubSystem(SDL_INIT_AUDIO);

	printf("[SDLAUDIO]Closing SDL audio\n");
	return 1;
}
void SDL_callback(void *userdata, Uint8 *stream, int len)
{
    sdlAudioDevice *me=(sdlAudioDevice *)userdata;
    me->callback(stream,len);
}
/**
    \fn callback
    \brief callback invoked by SDL when it is time to put more datas
*/
uint8_t sdlAudioDevice::callback( Uint8 *stream, int len)
{	
	
	uint32_t left=0;
	uint8_t *in,*out;
    uint32_t wrCopy=wr_ptr;
    uint32_t avail=wr_ptr-rd_ptr;
    if(avail<len)
    {
        printf("[SDLAudio] underflow wanted :%u got %u\n",len,avail);
        memset(stream+avail,0,len-avail);
        len=avail;
    }
    memcpy(stream,audioBuffer+rd_ptr,len);
    rd_ptr+=len;
    return 1;
   
}
/**
    \fn init
    \brief Initialize SDL audio data pump

*/
uint8_t sdlAudioDevice::init(uint32_t channels, uint32_t fq) 
{
SDL_AudioSpec spec,result;
_channels = channels;
		
		printf("[SDL] Opening audio, fq=%d\n",fq);
		
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
		spec.samples=4*1024; // nb samples in the buffer
		spec.callback=SDL_callback;
		spec.userdata=this;
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
    wr_ptr=rd_ptr=0;
    audioBuffer=new uint8_t[BUFFER_SIZE];
	frameCount=0;
    return 1;
}

/**
    \fn play
    \brief Put datas in the shared buffer

*/
uint8_t sdlAudioDevice::play(uint32_t len, float *data)
 {
 	// First put stuff into the buffer
	uint8_t *dest;
	uint32_t left;
    int needStart=0;

	dither16(data, len, _channels);
    len*=2; // in bytes!

    // Shrink ?
    SDL_LockAudio();
    if(wr_ptr>BUFFER_SIZE/2 && rd_ptr>BUFFER_SIZE/8)
    {   
        memmove(audioBuffer,audioBuffer+rd_ptr,(wr_ptr-rd_ptr));
        wr_ptr-=rd_ptr;
        rd_ptr=0;
    }
    
    if(wr_ptr+len>BUFFER_SIZE)
    {
        printf("[SDLAUDIO] Overflow wr=%u len=%u\n",wr_ptr,len);
        SDL_UnlockAudio();
        return 0;
    }
    memcpy(audioBuffer+wr_ptr,data,len);
    if(!wr_ptr) needStart=1;
    wr_ptr+=len;
	SDL_UnlockAudio();
    if(needStart) SDL_PauseAudio(0);
	return 1;
}
/**
    \fn setVolume
    \brief Not supported by SDL aFaiK

*/
uint8_t sdlAudioDevice::setVolume(int volume)
{
	return 1;
}



