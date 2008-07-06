/***************************************************************************
                          ADM_deviceEsd.cpp  -  description

  ESD support as output audio device
                          
    copyright            : (C) 2005 by mean
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

#include  "ADM_deviceEsd.h"
#include <esd.h>
ADM_DECLARE_AUDIODEVICE(Esd,esdAudioDevice,1,0,0,"Esd audio device (c) mean");
/**
        \fn getLatencyMs
        \brief Returns device latency in ms
*/
uint32_t esdAudioDevice::getLatencyMs(void)
{
    return latency;
}
//_______________________________________________
//
//
//_______________________________________________
uint8_t  esdAudioDevice::stop(void) {
    if (esdDevice > 0) {
        esd_close(esdDevice);
        esdDevice = 0;
    }
    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t esdAudioDevice::init(uint32_t channels, uint32_t fq) 
{
esd_format_t format;
_channels = channels;
latency=0;

    format=ESD_STREAM | ESD_PLAY | ESD_BITS16;
    if(channels==1) format|=ESD_MONO;
        else format|=ESD_STEREO;

    printf("[ESD]  : %lu Hz, %lu channels\n", fq, channels);
    esdDevice=esd_play_stream(format,fq,NULL,"avidemux");
    if(esdDevice<=0) 
    {
        printf("[ESD] open failed\n");
        return 0;
    }
    printf("[ESD] open succeedeed\n");
    /*
#ifdef ADM_BIG_ENDIAN    
    int fmt = AFMT_S16_BE;
#else
    int fmt = AFMT_S16_LE;
#endif    
*/
    float f=0; // FIXE Cause a freeze with esdcompat =(float)esd_get_latency(esdDevice);
    f=f/44.1;
    latency=(uint32_t)f;
    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t esdAudioDevice::play(uint32_t len, float *data)
{
	dither16(data, len, _channels);
	write(esdDevice, data, len*2);
	return 1;
}

uint8_t esdAudioDevice::setVolume(int volume){
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

//EOF
