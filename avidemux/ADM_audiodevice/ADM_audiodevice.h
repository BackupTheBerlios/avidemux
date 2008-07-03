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

// Converts float to int16_t with dithering
extern void dither16(float *start, uint32_t nb, uint8_t channels);

 class audioDevice
 {
        protected:
                        uint32_t _channels; /// # of channels we want to setup
                        uint32_t _frequency;/// Frequency we want to setup

        public:
                                        audioDevice(void) {};
                        virtual         ~audioDevice() {};
                        virtual uint8_t  init(uint32_t channel, uint32_t fq ) =0;
                        virtual uint8_t  stop(void)=0;
                        virtual uint8_t  play(uint32_t len, float *data) =0;
                        virtual uint8_t  setVolume(int volume) {return 1;}
                        virtual uint32_t getLatencyMs(void) {return 0;}
}   ;
/**
    \class dummyAudioDevice
    \brief this dummy is used when no suitable device have been found.
*/
class dummyAudioDevice : public audioDevice
{
		  public:
                                        dummyAudioDevice(void) ;
                                        ~dummyAudioDevice(void);
                        virtual uint8_t init(uint32_t channels, uint32_t fq);
                        virtual uint8_t play(uint32_t len, float *data);
                        virtual uint8_t stop(void) ;
}   ;

#endif
