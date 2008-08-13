/**
    \file ADM_deviceSDL.h
    \brief SDL audio device plugin

    (C) Mean 2008, fixounet@free.fr
    GPL-v2

*/
#ifndef ADM_deviceSDL_H
#define ADM_deviceSDL_H

class sdlAudioDevice : public audioDevice
 {
     protected :
                uint8_t				_inUse;
                uint8_t  		    *audioBuffer;
                uint32_t 		    frameCount;
                uint32_t			rd_ptr;
                uint32_t			wr_ptr;

      public:
                                sdlAudioDevice(void);
                virtual         ~sdlAudioDevice();
                virtual uint8_t init(uint32_t channels, uint32_t fq);
                virtual uint8_t play(uint32_t len, float *data);
                virtual uint8_t stop(void);
                uint8_t setVolume(int volume);
     public:
                uint8_t callback( Uint8 *stream, int len);
     }     ;
#endif

