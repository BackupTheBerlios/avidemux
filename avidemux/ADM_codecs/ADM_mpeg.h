/***************************************************************************
                          ADM_mpeg.h  -  description
                             -------------------

	Mpeg1/2 decoder based on libmpeg2

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

#include "ADM_pp.h"
  class decoderMpeg: public decoders
{
     protected:
     					uint8_t		_seen;
				      uint8_t 		*_seqHeader;
				      uint32_t		_seqLen;
				      uint8_t		_seqFound;
				      uint8_t		*postprocessed;

				      uint8_t		init_codec( void );
				      uint8_t		kill_codec( void );
				      void		*_decoder;
				      void 		setFather( decoderMpeg *f);
					void  		decode_mpeg2 (uint8_t * current, uint8_t * end);
					void		feedData(uint32_t len, uint8_t *data);
					uint32_t	_swapUV;
					
					ADM_PP		_postproc;
				 

				
     public:
     							decoderMpeg(uint32_t w,uint32_t h,
								uint32_t extraLen, uint8_t *extraData);
         		virtual				~decoderMpeg();
    			virtual 	uint8_t 	uncompress(uint8_t *in,uint8_t *out,
								uint32_t len,uint32_t *flag=NULL);
			virtual 	uint8_t	 	bFramePossible(void) { return 1;}
			virtual 	void 		setParam( void );

}   ;

