/***************************************************************************
                          ADM_IVTC.h  -  description
                             -------------------
	A brute force IVTC

    begin                : Sat May 23 2003
    copyright            : (C) 2002/3 by mean
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
#ifndef VID_IVTC_
#define VID_IVTC_

 class  ADMVideoIVTC:public AVDMGenericVideoStream
 {

 protected:
    		
		uint8_t					*_uncompressed[6];
		uint32_t					_cacheStart;
     		virtual 	char 			*printConf(void);
				uint32_t      		getMatch( uint8_t *src );
		uint32_t					_old1,_old2,_confidence;
		uint32_t					_skipped;
		uint32_t					_shifted;
 public:


  				ADMVideoIVTC(  AVDMGenericVideoStream *in,CONFcouple *setup);

  	virtual 		~ADMVideoIVTC();
	virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          								ADMImage *data,uint32_t *flags);
	virtual uint8_t configure( AVDMGenericVideoStream *instream) {return 1;};

 }     ;
#endif
