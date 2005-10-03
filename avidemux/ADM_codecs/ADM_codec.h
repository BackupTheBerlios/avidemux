/***************************************************************************
                          ADM_codec.h  -  description
                             -------------------
    begin                : Fri Apr 12 2002
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
 #ifndef __CODECS__
 #define __CODECS__
#include "ADM_library/ADM_image.h"

#define AVI_KEY_FRAME	0x10
#define AVI_B_FRAME	0x4000	 // hopefully it is not used..
#define AVI_P_FRAME   	0x0

#define ADM_QPEL_ON	1
#define ADM_GMC_ON	2
#define ADM_VOP_ON	4

/*
        Bitrate in configuration will always be in **kBITS**

*/

  typedef struct _my_ENC_RESULT_
    {
		int is_key_frame;	// the current frame is encoded as a key frame
		int quantizer;		// the quantizer used for this frame
		int texture_bits;	// amount of bits spent on coding DCT coeffs
		int motion_bits;	// amount of bits spend on coding motion
		int total_bits;		// sum of two previous fields
		int out_quantizer;	// quantizer used for encoding
    }
    myENC_RESULT;

 class decoders
 {
    protected:
    		uint32_t  			_w;
       		uint32_t 			_h;
         	uint8_t			_lastQ;
    public:
    						decoders(uint32_t w,uint32_t h) { _w=w;_h=h;_lastQ=0;}
    			virtual 		~decoders() {};
       			virtual void 	setParam( void ) {};
    			virtual uint8_t 	uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flag=NULL);
     			virtual uint8_t 	getLastQ( uint8_t *q) { *q=_lastQ;return 1;}

			// does this codec *possibly* can have b-frame ?
                        virtual uint8_t dontcopy(void) { return 0;}     // if 1 means the decoder will return reference
                                                                        // no need to copy the datas to ADMimage
			virtual uint8_t bFramePossible(void) { return 0;}
			virtual uint8_t decodeHeaderOnly(void ) { return 0;};
			virtual uint8_t decodeFull(void ) { return 0;}
			virtual uint8_t isDivxPacked( void ){ return 0;};
			virtual uint32_t getSpecificMpeg4Info( void ){ return 0;};
			virtual uint8_t isIndexable( void ){ return 1;};
   }  ;

decoders *getDecoder(uint32_t fcc,uint32_t w, uint32_t h,uint32_t extraLen,uint8_t *extraData);
decoders *getDecoderVopPacked(uint32_t fcc,uint32_t w, uint32_t h,uint32_t extraLen,uint8_t *extraData);

class coders
 {
    protected:
    			uint32_t		_w;
			uint32_t 		_h;
    public:
    						coders(uint32_t w,uint32_t h) { _w=w;_h=h;}		
    			virtual 		~coders() {};	
    			virtual uint8_t 	compress(ADMImage *in,uint8_t *out,uint32_t *len);
   }  ;
   decoders *getCoder(uint32_t fcc,uint32_t w, uint32_t h);
   uint8_t isMpeg4Compatible( uint32_t fourcc);
    uint8_t isMSMpeg4Compatible( uint32_t fourcc);
/*----------------------------------------------------------*/
	class encoder
	{
	protected :
				uint32_t  _w,_h;
	public :
          			encoder(uint32_t width,uint32_t height)
             				{
						_w=width;
						_h=height;
                   			} ;
		virtual		uint8_t stopEncoder(void )=0;
		virtual 	uint8_t init( uint32_t val,uint32_t fps1000)=0;
		virtual		uint8_t  getResult( void *ress)=0;
		virtual 	uint8_t encode(
						ADMImage 	*in,
						uint8_t 	*out,
						uint32_t 	*len,
			       			uint32_t 	*flags)=0;
     };




#endif



