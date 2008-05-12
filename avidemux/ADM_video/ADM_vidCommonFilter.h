/***************************************************************************
                          ADM_vidCommonFilter.h  -  description
                             -------------------

The most common filters
    begin                : Tue Apr 13 2003
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

class  AVDMVideoStreamCrop:public AVDMGenericVideoStream
 {

 protected:
    		CROP_PARAMS          *_param;

 public:

				AVDMVideoStreamCrop(  AVDMGenericVideoStream *in,CONFcouple *couples);
 	virtual 		~AVDMVideoStreamCrop();
        virtual uint8_t 	getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          							ADMImage *data,uint32_t *flags);
        virtual char 	*printConf(void) ;
		  uint8_t 	configure( AVDMGenericVideoStream *instream);
          virtual uint8_t	getCoupledConf( CONFcouple **couples);


 }     ;
 

