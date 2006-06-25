/***************************************************************************
                          ADM_deviceAlsa.h  -  description
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

#ifdef ALSA_SUPPORT
	 class alsaAudioDevice : public audioDevice
	 {
		 protected :
				//	0-> no init done
				//	1-> device opened but init failed
				//	2->fully initialized
				uint32_t _init;
		  public:
					alsaAudioDevice(void);
		     		virtual uint8_t init(uint8_t channel,uint32_t fq);
	    			virtual uint8_t play(uint32_t len, float *data);
		      		virtual uint8_t stop(void) ;
				uint8_t setVolume(int volume);
		 }     ;
#endif


