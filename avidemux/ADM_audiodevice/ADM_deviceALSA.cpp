/***************************************************************************
                          ADM_deviceAlsa.cpp  -  description
                             -------------------

	Strongly derivated from code sample from alsa-project.org with some bits
		from mplayer concerning the swparams

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
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <assert.h>

 #include "config.h"

#ifdef ALSA_SUPPORT
#include "ADM_library/default.h"


#include  "ADM_audiodevice/ADM_deviceoss.h"
#include  "ADM_audiodevice/ADM_deviceALSA.h"
#include <alsa/asoundlib.h>

//
//	_____________ ALSA 0.9________________
//

#ifndef ALSA_1_0_SUPPORT
 /* Handle for the PCM device */
    snd_pcm_t *pcm_handle;

    alsaAudioDevice::alsaAudioDevice( void )
    {
		_init=0;
    }

uint8_t alsaAudioDevice::init( uint32_t channel,uint32_t fq )
{
	_freq=fq;
	_channels=channel;
	_init=0;
   /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */
    /* configuration to be used for the PCM stream. */
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;


  static char *pcm_name;

	pcm_name = strdup("plughw:0,0");
 /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_sw_params_alloca(&swparams);
    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, SND_PCM_NONBLOCK) < 0) {
      fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      return(0);
    }
    // past this point we got _init=1 -> partially initialized
    _init=1;
      /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Can not configure this PCM device.\n");
      return(0);
    }
    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "Error setting access.\n");
      return(0);
    }

    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      fprintf(stderr, "Error setting format.\n");
      return(0);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    int exact_rate,dir=0;
    exact_rate = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, fq, &dir);
    if (dir != 0) {
      fprintf(stderr, "The rate %lu Hz is not supported by your hardware.\n  ==> Using %d Hz instead.\n", fq, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channel) < 0) {
      fprintf(stderr, "Error setting channels.\n");
      return(0);
    }
#if 0
    	uint32_t periods=fq*2*channel*10;
	uint32_t periodsize=1;
    /* Set number of periods. Periods used to be called fragments. */
    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
      fprintf(stderr, "Error setting periods.\n");
      return(0);
    }
#else

 	int buffer_time = 800000;

	if ((buffer_time=snd_pcm_hw_params_set_buffer_time_near(pcm_handle, hwparams, buffer_time, 0)) < 0)
	  {
	    printf("Error : hw_params_set_buffer_time\n");
	    return(0);
	  }
	  // unsigned ?
	snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams, buffer_time>>2, 0) ;
#if 0
	if (snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams, buffer_time>>2, 0) < 0)
	  /* original: alsa_buffer_time/ao_data.bps */
	  {
	    printf("Error : hw_params_set_period_time\n");
	    return(0);
	  }
#endif
#endif


/*
If your hardware does not support a buffersize of 2^n, you can use the function snd_pcm_hw_params_set_buffer_size_near. This works similar to snd_pcm_hw_params_set_rate_near. Now we apply the configuration to the PCM device pointed to by pcm_handle. This will also prepare the PCM device.
*/


    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Error setting HW params.\n");
      return(0);
    }

 	if (snd_pcm_sw_params_current(pcm_handle, swparams) < 0)
	    {
	      printf("Error setting SW params.\n");
	      return(0);
	    }

 	// be sure that playback starts immediatly (or near)
	  if (snd_pcm_sw_params_set_avail_min(pcm_handle, swparams, 4) < 0)
	    {
	      printf("Error setting set_avail_min \n");
	      return(0);
	    }

	  if (snd_pcm_sw_params(pcm_handle, swparams) < 0)
	    {
	      printf("Error:snd_pcm_sw_params\n ");
	      return(0);
	    }


      if ( snd_pcm_prepare(pcm_handle) < 0)
	{
	  printf("Error : snd_pcm_prepare\n");
	  return(0);
	}

    printf("\n Alsa successfully initialized\n");

    // 2=fully initialized
    _init=2;
    return 1;
}


uint8_t alsaAudioDevice::play( uint32_t len, uint8_t *data )
{
 	int ret;
     /* Write num_frames frames from buffer data to    */
    /* the PCM device pointed to by pcm_handle.       */
    /* Returns the number of frames actually written. */
    	// convert data in bytes to sample
	if(2!=_init) return 0;
    	len>>=1;
	if(_channels==2) len>>=1;
    	while(1)
	{
        	ret=snd_pcm_writei(pcm_handle, data, len);
		if(ret==(int)len)
		{
			return 1;
		}

		if(ret<0)
		{
			switch(ret)
			{
				case    -EAGAIN :
							//wait a bit to flush datas
							snd_pcm_wait(pcm_handle, 1000);
      							continue;


				default:
						printf("Error : Play %s (len=%lu)\n", snd_strerror(ret),len);
						return 1;
			}


		}
		else
		{
			if(len<2) return 1;
			len-=ret;
		}
	}
	return 1;
}

 uint8_t alsaAudioDevice::stop( void )
 {
 // we have at least a partial initialization
 if(_init)
 {
       /* Stop PCM device and drop pending frames */
    snd_pcm_drop(pcm_handle);

    /* Stop PCM device after pending frames have been played */
    snd_pcm_drain(pcm_handle);
      if (snd_pcm_close(pcm_handle) < 0)
      {
		printf("\n Troubles closing alsa\n");

      }
     }
     _init=0;
     return 1;
}
#else
//___________________________________________________________________________________________________________
//	_____________ ALSA 1.0________________
//
//___________________________________________________________________________________________________________
 /* Handle for the PCM device */
    snd_pcm_t *pcm_handle;

    alsaAudioDevice::alsaAudioDevice( void )
    {
		_init=0;
    }

uint8_t alsaAudioDevice::init( uint32_t channel,uint32_t fq )
{
	int dir=0;
	
	_freq=fq;
	_channels=channel;
	_init=0;
   /* Playback stream */
    snd_pcm_stream_t stream = SND_PCM_STREAM_PLAYBACK;

    /* This structure contains information about    */
    /* the hardware and can be used to specify the  */
    /* configuration to be used for the PCM stream. */
    snd_pcm_hw_params_t *hwparams;
    snd_pcm_sw_params_t *swparams;


  static char *pcm_name;

	pcm_name = strdup("plughw:0,0");
 /* Allocate the snd_pcm_hw_params_t structure on the stack. */
    snd_pcm_hw_params_alloca(&hwparams);
    snd_pcm_sw_params_alloca(&swparams);
    /* Open PCM. The last parameter of this function is the mode. */
    /* If this is set to 0, the standard mode is used. Possible   */
    /* other values are SND_PCM_NONBLOCK and SND_PCM_ASYNC.       */
    /* If SND_PCM_NONBLOCK is used, read / write access to the    */
    /* PCM device will return immediately. If SND_PCM_ASYNC is    */
    /* specified, SIGIO will be emitted whenever a period has     */
    /* been completely processed by the soundcard.                */
    if (snd_pcm_open(&pcm_handle, pcm_name, stream, SND_PCM_NONBLOCK) < 0) {
      fprintf(stderr, "Error opening PCM device %s\n", pcm_name);
      return(0);
    }
    // past this point we got _init=1 -> partially initialized
    _init=1;
      /* Init hwparams with full configuration space */
    if (snd_pcm_hw_params_any(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Can not configure this PCM device.\n");
      return(0);
    }
    /* Set access type. This can be either    */
    /* SND_PCM_ACCESS_RW_INTERLEAVED or       */
    /* SND_PCM_ACCESS_RW_NONINTERLEAVED.      */
    /* There are also access types for MMAPed */
    /* access, but this is beyond the scope   */
    /* of this introduction.                  */
    if (snd_pcm_hw_params_set_access(pcm_handle, hwparams, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) {
      fprintf(stderr, "Error setting access.\n");
      return(0);
    }

    /* Set sample format */
    if (snd_pcm_hw_params_set_format(pcm_handle, hwparams, SND_PCM_FORMAT_S16_LE) < 0) {
      fprintf(stderr, "Error setting format.\n");
      return(0);
    }

    /* Set sample rate. If the exact rate is not supported */
    /* by the hardware, use nearest possible rate.         */
    int exact_rate;
    dir=0;
    exact_rate = snd_pcm_hw_params_set_rate_near(pcm_handle, hwparams, &fq, &dir);
    if (dir != 0) {
      fprintf(stderr, "The rate %lu Hz is not supported by your hardware.\n  ==> Using %d Hz instead.\n", fq, exact_rate);
    }

    /* Set number of channels */
    if (snd_pcm_hw_params_set_channels(pcm_handle, hwparams, channel) < 0) {
      fprintf(stderr, "Error setting channels.\n");
      return(0);
    }
#if 0
    	uint32_t periods=fq*2*channel*10;
	uint32_t periodsize=1;
    /* Set number of periods. Periods used to be called fragments. */
    if (snd_pcm_hw_params_set_periods(pcm_handle, hwparams, periods, 0) < 0) {
      fprintf(stderr, "Error setting periods.\n");
      return(0);
    }
#else

 	unsigned int buffer_time = 800000;
	int er;
	unsigned int buff;
	dir=0;

	if ((er=snd_pcm_hw_params_set_buffer_time_near(pcm_handle, hwparams, &buffer_time, &dir)) < 0)
	  {
	    printf("Error : hw_params_set_buffer_time\n");
	    return(0);
	  }
	  // unsigned ?
	  dir=0;
	  buff=buffer_time>>2;
	snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams, &buff, &dir) ;
#if 0
	if (snd_pcm_hw_params_set_period_time_near(pcm_handle, hwparams, buffer_time>>2, 0) < 0)
	  /* original: alsa_buffer_time/ao_data.bps */
	  {
	    printf("Error : hw_params_set_period_time\n");
	    return(0);
	  }
#endif
#endif


/*
If your hardware does not support a buffersize of 2^n, you can use the function snd_pcm_hw_params_set_buffer_size_near. This works similar to snd_pcm_hw_params_set_rate_near. Now we apply the configuration to the PCM device pointed to by pcm_handle. This will also prepare the PCM device.
*/


    /* Apply HW parameter settings to */
    /* PCM device and prepare device  */
    if (snd_pcm_hw_params(pcm_handle, hwparams) < 0) {
      fprintf(stderr, "Error setting HW params.\n");
      return(0);
    }

 	if (snd_pcm_sw_params_current(pcm_handle, swparams) < 0)
	    {
	      printf("Error setting SW params.\n");
	      return(0);
	    }

 	// be sure that playback starts immediatly (or near)
	  if (snd_pcm_sw_params_set_avail_min(pcm_handle, swparams, 4) < 0)
	    {
	      printf("Error setting set_avail_min \n");
	      return(0);
	    }

	  if (snd_pcm_sw_params(pcm_handle, swparams) < 0)
	    {
	      printf("Error:snd_pcm_sw_params\n ");
	      return(0);
	    }


      if ( snd_pcm_prepare(pcm_handle) < 0)
	{
	  printf("Error : snd_pcm_prepare\n");
	  return(0);
	}

    printf("\n Alsa successfully initialized\n");

    // 2=fully initialized
    _init=2;
    return 1;
}


uint8_t alsaAudioDevice::play( uint32_t len, uint8_t *data )
{
 	int ret;
     /* Write num_frames frames from buffer data to    */
    /* the PCM device pointed to by pcm_handle.       */
    /* Returns the number of frames actually written. */
    	// convert data in bytes to sample
	if(2!=_init) return 0;
    	len>>=1;
	if(_channels==2) len>>=1;
    	while(1)
	{
        	ret=snd_pcm_writei(pcm_handle, data, len);
		if(ret==(int)len)
		{
			return 1;
		}

		if(ret<0)
		{
			switch(ret)
			{
				case    -EAGAIN :
							//wait a bit to flush datas
							snd_pcm_wait(pcm_handle, 1000);
      							continue;


				default:
						printf("Error : Play %s (len=%lu)\n", snd_strerror(ret),len);
						return 1;
			}


		}
		else
		{
			if(len<2) return 1;
			len-=ret;
		}
	}
	return 1;
}

 uint8_t alsaAudioDevice::stop( void )
 {
 // we have at least a partial initialization
 if(_init)
 {
       /* Stop PCM device and drop pending frames */
    snd_pcm_drop(pcm_handle);

    /* Stop PCM device after pending frames have been played */
    snd_pcm_drain(pcm_handle);
      if (snd_pcm_close(pcm_handle) < 0)
      {
		printf("\n Troubles closing alsa\n");

      }
     }
     _init=0;
     return 1;
}

#endif
#else
void dummy_alsa_fun( void);
void dummy_alsa_fun( void)
 {
}


#endif
