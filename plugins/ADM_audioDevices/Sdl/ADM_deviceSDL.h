//
// C++ Interface: ADM_deviceSDL
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//


    class sdlAudioDevice : public audioDevice
	 {
		 protected :
					uint8_t				_inUse;
		  public:
		  				sdlAudioDevice(void);
		     		virtual uint8_t init(uint32_t channels, uint32_t fq);
	    			virtual uint8_t play(uint32_t len, float *data);
		      		virtual uint8_t stop(void);
				uint8_t setVolume(int volume);
		 }     ;

