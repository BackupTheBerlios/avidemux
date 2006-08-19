//
// C++ Interface: ADM_audiodevice
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef ADM_AUDIODEVICE_H
#define ADM_AUDIODEVICE_H

#include <ADM_assert.h>

void dither16(float *start, uint32_t nb, uint8_t channels);

 class audioDevice
 {
        protected:
			uint8_t _channels;

        public:
                                        audioDevice(void);
                        virtual uint8_t init(uint8_t channel, uint32_t fq ) {ADM_assert(0);return 0;}
                        virtual uint8_t stop(void) {ADM_assert(0);return 0;}
                        virtual uint8_t play(uint32_t len, uint8_t *data) {ADM_assert(0);return 0;}//deprecate
                        virtual uint8_t play(uint32_t len, float *data) {ADM_assert(0);return 0;}
                        virtual uint8_t setVolume(int volume) {return 1;}
}   ;

class dummyAudioDevice : public audioDevice
{
		  public:
                                        dummyAudioDevice(void) {};
                        virtual uint8_t init(uint8_t channels, uint32_t fq)
                                {printf("\n Null audio device"); UNUSED_ARG(fq); UNUSED_ARG(channels); return 1;}
                        virtual uint8_t play(uint32_t len, float *data)
                                {UNUSED_ARG(len); UNUSED_ARG(data); return 1;}
                        virtual uint8_t stop(void) {return 1;}
}   ;

#endif
