/***************************************************************************
                          Base class for Mpeg Demuxer
                             -------------------
		
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

 #ifndef DMX_DMX
 #define DMX_DMX
 
#include "dmx_io.h"
 
#define ADM_NO_PTS 0xFFFFFFFFFFFFFFFFLL

class dmx_demuxer
 {
	  protected : 
	          
		uint64_t  _size;
		uint8_t   _lastErr;   // Very important : The derivated signals an error using that!
		
	  public:
	  		   dmx_demuxer();
		virtual    ~dmx_demuxer();	       
		
		virtual    uint8_t   	open(char *name)=0;
	         
		
		virtual uint8_t 	forward(uint32_t f)=0;
	        virtual uint8_t         stamp(void)=0; 
                virtual uint8_t         getStamp(uint64_t *abs,uint64_t *rel)=0;
                virtual uint64_t        elapsed(void)=0;
                
                virtual uint8_t         getPos( uint64_t *abs,uint64_t *rel)=0;
                virtual uint8_t         setPos( uint64_t abs,uint64_t  rel)=0;
                
                virtual uint64_t        getSize( void) { return _size;}          
                virtual uint8_t         sync( uint8_t *stream);
            	
                virtual uint8_t         read(uint8_t *w,uint32_t len)=0;
   	        virtual uint8_t		read8i(void)=0;
   	        virtual uint16_t	read16i(void)=0;
		virtual uint32_t	read32i(void)=0;
		
};	
	

#endif
