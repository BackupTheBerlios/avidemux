/***************************************************************************
                          ADM_divx4.h  -  description
                             -------------------

	Mpeg4 ****decoder******** using divx 5.0.5

    begin                : Wed Sep 25 2002
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
#ifdef USE_DIVX
 class decoderDIVX : public decoders
{
     protected:
					void				*_handle;
     public:
     									decoderDIVX(uint32_t w,uint32_t h);
         		virtual					~decoderDIVX();
    			virtual 			uint8_t 	uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag=NULL) 		;
   		       virtual 			void 	setParam( void );
		       virtual 			uint8_t 	bFramePossible(void) { return 0;}
}   ;
#endif
