/***************************************************************************
                          ADM_divxEncode.h  -  description
                             -------------------
    begin                : Fri May 17 2002
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

 #ifndef __DIVX_NCODE_
 #define __DIVX_NCODE_
 	
  class divxEncoder    : public encoder
  	{
     		protected :
      			          		void *_handle;
						uint8_t params( uint32_t fps1000 );
						uint8_t _init;
          public :
          					     divxEncoder(uint32_t width,uint32_t height)  ;
  	                virtual    	uint8_t stopEncoder(void );
	                virtual    	uint8_t  getResult( void *ress);
                  	virtual	uint8_t init( uint32_t val,uint32_t fps1000);
                  	virtual 	uint8_t encode( uint8_t *in, uint8_t *out, uint32_t *len, uint32_t *flags);
     };

     class divxEncoderCQ : public divxEncoder
     {
       protected:
       						uint32_t _q;
       public:
       				divxEncoderCQ(uint32_t width,uint32_t height)   : divxEncoder(width,height) {};;
        			virtual uint8_t init(uint32_t 	q,uint32_t fps1000);
       }  ;

     class divxEncoderCBR : public divxEncoder
     {
       protected:
       						uint32_t _br;
       public:
       				divxEncoderCBR(uint32_t width,uint32_t height)  : divxEncoder(width,height) {};;
        			virtual uint8_t init(uint32_t 	br,uint32_t fps1000);
       }  ;


 #endif

