/***************************************************************************
                          ADM_deviceoss.h  -  description
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

 class audioDevice
 {
	  protected:
	  			uint32_t _fq;
	     		uint32_t _rate;
	 
	 public:
	 			audioDevice(void ) { _fq=0;_rate=0;}
	    		virtual uint8_t init( uint32_t channel,uint32_t fq ) { ADM_assert(0);return 0;}
	      		virtual uint8_t stop( void ) { ADM_assert(0);return 0;}
	      
	    		virtual uint8_t play( uint32_t len, uint8_t *data ) { ADM_assert(0);return 0;}					    	 
	 }   ;
	 class dummyAudioDevice : public audioDevice
	 {
		  public:
		  			dummyAudioDevice(void ) {};
		     		virtual uint8_t init( uint32_t channel,uint32_t fq ) { printf("\n Null audio device"); UNUSED_ARG(fq); UNUSED_ARG(channel); return 1;}
	    			virtual uint8_t play( uint32_t len, uint8_t *data ) { UNUSED_ARG(len); UNUSED_ARG(data); return 1;}			
		      		virtual uint8_t stop( void ) { return 1;}
		 
		 }   ;

#ifdef OSS_SUPPORT
	        class ossAudioDevice : public audioDevice
	 {
		 protected :
				 int oss_fd;
		  public:
		  			ossAudioDevice(void) { oss_fd=0;}
		     		virtual uint8_t init( uint32_t channel,uint32_t fq ) ;
	    			virtual uint8_t play( uint32_t len, uint8_t *data ) ;
		      		virtual uint8_t stop( void ) ;
		 }     ;
#endif
#ifdef CONFIG_DARWIN

	        class coreAudioDevice : public audioDevice
	 {
		 protected :
					uint8_t				_inUse;
		  public:
		  			coreAudioDevice(void) ;
		     		virtual uint8_t init( uint32_t channel,uint32_t fq ) ;
	    			virtual uint8_t play( uint32_t len, uint8_t *data ) ;
		      		virtual uint8_t stop( void ) ;
		 }     ;

#endif
