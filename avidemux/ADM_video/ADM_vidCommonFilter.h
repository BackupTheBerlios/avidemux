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
 class  AVDMVideoStreamBSMear:public AVDMGenericVideoStream
 {

 protected:
    		CROP_PARAMS          *_param;

 public:

  				AVDMVideoStreamBSMear(  AVDMGenericVideoStream *in,CONFcouple *setup);
  		virtual 	~AVDMVideoStreamBSMear();
          	virtual uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          								ADMImage *data,uint32_t *flags);
          	virtual 	char 		*printConf(void) ;
			  	uint8_t 	configure( AVDMGenericVideoStream *instream);
          			uint8_t	getCoupledConf( CONFcouple **couples);
 }     ;

class  AVDMVideoStreamResize:public AVDMGenericVideoStream
 {

 protected:
    		RESIZE_PARAMS          	*_param;
    		uint8_t				_init;
           	INT 					*Hpattern_luma;
           	INT 					*Hpattern_chroma;
           	INT 					*Vpattern_luma;
           	INT 					*Vpattern_chroma;
           	uint8_t 				*_intermediate_buffer;
		// engine
		void ResizeV(Image * iin, Image * iout, INT *pattern) ;
		void ResizeVFIR4(Image * iin, Image * iout, INT *pattern) ;

		void ResizeH(Image * iin, Image * iout, INT *pattern) ;
 		void ResizeHFIR4(Image * iin, Image * iout, INT  *pattern)	;

 		void precompute(Image * dst, Image * src, uint8_t algo);
           	uint8_t 	zoom(Image * dst, Image * src);
           	void endcompute(void);

 public:

  				AVDMVideoStreamResize(  AVDMGenericVideoStream *in,CONFcouple *setup);
				AVDMVideoStreamResize(	AVDMGenericVideoStream *in,uint32_t x,uint32_t y);
  				virtual 		~AVDMVideoStreamResize();
          virtual 		uint8_t getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	ADMImage *data,uint32_t *flags);
				uint8_t configure( AVDMGenericVideoStream *instream);
	virtual 		char 	*printConf(void) ;

          virtual uint8_t	getCoupledConf( CONFcouple **couples);


 }     ;
