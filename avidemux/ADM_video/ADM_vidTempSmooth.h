/***************************************************************************
                          ADM_vidTempSmooth.h  -  description
                             -------------------
    begin                : Sun Jul 28 2002
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

 typedef struct
 {
	   	uint32_t radius,chroma_threshold,luma_threshold;
   }TEMPSMOOTH_PARAMS;


    class  AVDMVideoTempSmooth:public AVDMGenericVideoStream
 {

 protected:

    			uint8_t							*_unpack;
           virtual 	char 								*printConf(void);
    			TEMPSMOOTH_PARAMS				*_param;
			uint8_t							**_ptr_to_screen;
			uint8_t							*_screen;
 public:



						AVDMVideoTempSmooth(  AVDMGenericVideoStream *in,CONFcouple *setup);
  			virtual 		~AVDMVideoTempSmooth();
		        virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags);
			virtual uint8_t configure( AVDMGenericVideoStream *instream) ;
			virtual uint8_t	getCoupledConf( CONFcouple **couples)		;
 }     ;

 AVDMGenericVideoStream *tempsmooth_create(AVDMGenericVideoStream *in, void *param);
