/***************************************************************************
                          ADM_vp3.h  -  description
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
 #ifndef __VP3__
 #define __VP3__
 

  class decoderVP3: public decoders
{
     protected:
				      PB_INSTANCE *_handle;
   
							    
     public:
     										decoderVP3(uint32_t w,uint32_t h);
         		virtual					~decoderVP3();
    			virtual uint8_t 	uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag=NULL) 		;	
   		       	virtual 			void setParam( void );
	

}   ;
#endif
