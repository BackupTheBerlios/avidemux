/***************************************************************************
                          ADM_devicePulseSimple.cpp  -  description

  Simple Pulse audio out
                          
    copyright            : (C) 2008 by mean
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
#include "ADM_default.h"
#include "ADM_audiodevice.h"


#include  "ADM_audiodevice.h"
#include  "ADM_audioDeviceInternal.h"
#include  "ADM_devicePulseSimple.h"
#include  "pulse/simple.h"

ADM_DECLARE_AUDIODEVICE(PulseAudioS,pulseSimpleAudioDevice,1,0,0,"PulseAudioSimple audio device (c) mean");
#define INSTANCE  ((pa_simple *)instance)

// By default we use float
//#define ADM_PULSE_INT16
/**
    \fn pulseSimpleAudioDevice
    \brief Constructor

*/
pulseSimpleAudioDevice::pulseSimpleAudioDevice()
{
    instance=NULL;
}
/**
    \fn pulseSimpleAudioDevice
    \brief Returns delay in ms
*/
uint32_t pulseSimpleAudioDevice::getLatencyMs(void)
{
    if(!instance) return 0;
    int er;
    pa_usec_t l=0;
    l=pa_simple_get_latency(INSTANCE, &er);
    printf("[Pulse] Latency :%lu\n",l);
    l/=1000;
    return (uint32_t )l;
}

/**
    \fn stop
    \brief stop & release device

*/

uint8_t  pulseSimpleAudioDevice::stop(void) 
{
int er;
    if(!instance) return 1;
    ADM_assert(instance);
    pa_simple_flush(INSTANCE,&er);
    pa_simple_free(INSTANCE);
    instance=NULL;
    printf("[PulseAudio] Stopped\n");
    return 1;
}

/**
    \fn init
    \brief Take & initialize the device

*/
uint8_t pulseSimpleAudioDevice::init(uint32_t channels, uint32_t fq) 
{

pa_simple *s;
pa_sample_spec ss;
int er;
 
#ifdef ADM_PULSE_INT16
  ss.format = PA_SAMPLE_S16NE;
#else
    ss.format = PA_SAMPLE_FLOAT32NE;//PA_SAMPLE_S16NE; //FIXME big endian
#endif
  ss.channels = channels;
  ss.rate =fq;
 
  instance= pa_simple_new(NULL,               // Use the default server.
                    "Avidemux2",           // Our application's name.
                    PA_STREAM_PLAYBACK,
                    NULL,               // Use the default device.
                    "Sound",            // Description of our stream.
                    &ss,                // Our sample format.
                    NULL,               // Use default channel map
                    NULL ,             // Use default buffering attributes.
                    &er               // Ignore error code.
                    );
  if(!instance)
    {
        printf("[PulseSimple] open failed\n");
        return 0;
    }
 pa_usec_t l=0;
    l=pa_simple_get_latency(INSTANCE, &er);
    printf("[Pulse] Latency :%lu\n",l);

    printf("[PulseSimple] open ok\n");
    return 1;

}

/**
    \fn play
    \brief Playback samples

*/
uint8_t pulseSimpleAudioDevice::play(uint32_t len, float *data)
{
int er;
    if(!instance) return 0;
#ifdef ADM_PULSE_INT16
	dither16(data, len, _channels);
    pa_simple_write(INSTANCE,data,len*2,&er);
#else
    pa_simple_write(INSTANCE,data,len*4,&er);
#endif
	return 1;
}
/**
    \fn setVolume
    \brief Cannot be done with pulse simple
*/
uint8_t pulseSimpleAudioDevice::setVolume(int volume)
{
	return 1;
}
//EOF
