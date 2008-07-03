/***************************************************************************
                          ADM_deviceoss.cpp  -  description
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

#include "ADM_default.h"

#include  "ADM_audiodevice.h"
#include  "ADM_audioDeviceInternal.h"

#include <errno.h>

#include <fcntl.h>
#include <sys/ioctl.h>

#if defined(ADM_BSD_FAMILY) && !defined(__FreeBSD__) && !defined(__OpenBSD__)
	#include <soundcard.h>
	const char *dsp = DEVOSSAUDIO;;

#elif defined(__OpenBSD__)
	#include <soundcard.h>
	const char *dsp = "/dev/audio";
	const char *device_mixer = "/dev/mixer";

#else
	#include <sys/soundcard.h>
	const char *dsp = "/dev/dsp";
        const char *device_mixer = "/dev/mixer";
#endif

#include  "ADM_deviceoss.h"

ADM_DECLARE_AUDIODEVICE(Oss,ossAudioDevice,1,0,0,"Oss audio device (c) mean");

//_______________________________________________
//
//
//_______________________________________________
uint8_t  ossAudioDevice::setVolume(int volume) 
{
        int fd;
	int ret;
	uint32_t which_vol = 0;

	//prefs->get(FEATURE_AUDIOBAR_USES_MASTER,&which_vol);
        fd=open(device_mixer,O_RDONLY);
        if(!fd)
        {
                printf("[OSSS]: cannot open mixer\n");
                return 0;
        }
        printf("[OSSS]: New %s volume %d\n",(which_vol?"master":"pcm"),volume);
        // Assuming stereo
        volume=volume+(volume<<8);
	if( which_vol ){
        	ret = ioctl(fd, MIXER_WRITE(SOUND_MIXER_VOLUME), &volume);
	}else{
        	ret = ioctl(fd, MIXER_WRITE(SOUND_MIXER_PCM   ), &volume);
	}
        close(fd);

	if( ret ){
		if( errno == EBADF ){
			printf("[OSSS]set mixer failed: %u (possible access issue)\n",errno);
		}else{
			printf("[OSSS]set mixer failed: %u\n",errno);
		}
	}
        return 1;

}

//_______________________________________________
//
//
//_______________________________________________
uint8_t  ossAudioDevice::stop(void) {
    if (oss_fd > 0) {
        close(oss_fd);
        oss_fd = 0;
    }
    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t ossAudioDevice::init(uint32_t channels, uint32_t fq) 
{
	_channels = channels;
 
    printf("[OSSS]: %lu Hz, %lu channels\n", fq, channels);
    // open OSS device
    oss_fd = open(dsp, O_WRONLY | O_NONBLOCK);
    if (oss_fd == -1) {
/*
	if( errno == EACCES )
	{
          GUI_Error_HIG(QT_TR_NOOP("Could not open OSS audio device"), QT_TR_NOOP("Check the permissions for /dev/dsp."));
	  }
	else
*/
           printf("[OSSS] Error initializing OSS: Error : %d\n", errno);
        return 0;
    }
    // seems ok, set up audio 
    if (ioctl (oss_fd, SNDCTL_DSP_SPEED, &fq) < 0) {
        printf("[OSSS] Error setting up OSS(SPEED): Error : %d\n", errno);
        return 0;
    }
    if (channels > 2) {
        if (ioctl (oss_fd, SNDCTL_DSP_CHANNELS, &channels) < 0) {
	    printf("[OSSS] Error setting up OSS(CHANNELS): Error : %d\n", errno);
	    return 0;
        }
    } else {
        int chan = channels - 1;
        if (ioctl (oss_fd, SNDCTL_DSP_STEREO, &chan) < 0) {
	    printf("[OSSS] Error setting up OSS(STEREO): Error : %d\n", errno);
	    return 0;
        }
    }
#ifdef ADM_BIG_ENDIAN    
    int fmt = AFMT_S16_BE;
#else
    int fmt = AFMT_S16_LE;
#endif    
    if (ioctl (oss_fd, SNDCTL_DSP_SETFMT, &fmt) < 0) {
        printf("[OSSS] Error setting up OSS(FORMAT): Error : %d\n", errno);
        return 0;
    }

    return 1;
}

//_______________________________________________
//
//
//_______________________________________________
uint8_t ossAudioDevice::play(uint32_t len, float *data)
{
	uint32_t w;

	if (!oss_fd)
		return 0;

	dither16(data, len, _channels);

	w = write(oss_fd, data, len*2);
        if(w!=len*2)
        {
          printf("[OSS] Warning : %u / %u\n",w,len*2); 
        }
	return 1;
}
//EOF
