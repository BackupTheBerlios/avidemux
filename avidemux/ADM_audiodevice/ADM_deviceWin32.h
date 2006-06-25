//
// C++ Interface: ADM_deviceWin32
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifdef CYG_MANGLING
	        class win32AudioDevice : public audioDevice
	 {
		 protected :
					uint8_t				_inUse;
		  public:
		  				win32AudioDevice(void);
		     		virtual uint8_t init(uint8_t channels, uint32_t fq);
	    			virtual uint8_t play(uint32_t len, float *data);
		      		virtual uint8_t stop(void);
		      		virtual uint8_t setVolume(int volume);
		 }     ;

#endif
