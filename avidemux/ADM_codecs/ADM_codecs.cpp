/***************************************************************************
                          ADM_codecs.cpp  -  description
                             -------------------
    begin                : Fri Apr 12 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr

    see here : http://www.webartz.com/fourcc/

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "config.h"

 #include "ADM_vp32/ADM_vp3.h"
 
#ifdef USE_FFMPEG
extern "C" {	
#include "ADM_lavcodec.h"
} ;
#endif
#include "ADM_library/default.h"
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif


#include "ADM_colorspace/colorspace.h"
#ifdef USE_XX_XVID
#include "xvid.h"
#endif


#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_mjpeg.h"
#include "ADM_codecs/ADM_codecNull.h"
#include "ADM_codecs/ADM_rgb16.h"
#include "ADM_codecs/ADM_uyvy.h"
#include "ADM_codecs/ADM_xvideco.h"

#include "ADM_library/fourcc.h"

#ifdef USE_FFMPEG
#include "ADM_codecs/ADM_ffmp43.h"
#endif

#ifdef USE_DIVX
	#include "ADM_codecs/ADM_divx4.h"
#endif

#ifdef USE_THEORA
#include "ADM_codecs/ADM_theora_dec.h"
#endif
#include "ADM_codecs/ADM_mpeg.h"
#include "ADM_codecs/ADM_vp3.h"
#include "ADM_toolkit/toolkit.hxx"
extern uint8_t GUI_Question(char *);
extern  uint8_t use_fast_ffmpeg;

uint8_t isMpeg4Compatible( uint32_t fourcc)
{
     #define CHECK(x) if(fourCC::check(fourcc,(uint8_t *)x)) \
						{divx4=1; }

    uint8_t divx4=0;

    		CHECK("DIVX");
      		CHECK("divx");
	       CHECK("DX50");
	       CHECK("xvid");
	       CHECK("XVID");
          CHECK("BLZ0");

          return divx4;

      #undef CHECK
}
uint8_t isMSMpeg4Compatible( uint32_t fourcc)
{
     #define CHECK(x) if(fourCC::check(fourcc,(uint8_t *)x)) \
						{divx3=1; }

    uint8_t divx3=0;

    		CHECK("MP43");
    		CHECK("mp43");
   		 CHECK("div3");
    		CHECK("DIV3");
    		CHECK("DIV4");
    		CHECK("div4");
    		CHECK("COL1");

          return divx3;

      #undef CHECK
}

uint8_t decoders::uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag) {
    UNUSED_ARG(in);
    UNUSED_ARG(out);
    UNUSED_ARG(len);
    UNUSED_ARG(flag);
	return 0;
}
decoders *getDecoderVopPacked(uint32_t fcc,uint32_t w, uint32_t h,uint32_t extraLen,uint8_t *extraData)
{
    UNUSED_ARG(fcc);
    UNUSED_ARG(extraLen);
    UNUSED_ARG(extraData);
   	return(decoders *)( new decoderFFMpeg4VopPacked(w,h));

}
 decoders *getDecoder(uint32_t fcc,uint32_t w, uint32_t h,uint32_t extraLen,uint8_t *extraData)
 {
				
			if(isMSMpeg4Compatible(fcc)==1)
			{
				// For div3, no problem we take ffmpeg

							return(decoders *)( new decoderFFDiv3(w,h));
			}
			
#ifdef USE_FFMPEG	  
    if(fourCC::check(fcc,(uint8_t *)"dvsd")
      || fourCC::check(fcc,(uint8_t *)"DVDS")
      || fourCC::check(fcc,(uint8_t *)"CDVC")
       )
          {
				
			  				     	return(decoders *)( new decoderFFDV(w,h,extraLen,extraData));
			 }
#endif	
#ifdef USE_FFMPEG	  
    if(fourCC::check(fcc,(uint8_t *)"MP42"))
          {
				
			  				     	return(decoders *)( new decoderFFMP42(w,h));
			    }
#endif	
#ifdef USE_FFMPEG	  
    if(fourCC::check(fcc,(uint8_t *)"H263"))
          {
				
			  				     	return(decoders *)( new decoderFFH263(w,h));
			    }
    if(fourCC::check(fcc,(uint8_t *)"HFYU"))
          {

			  				     	return(decoders *)( new decoderFFhuff(w,h,extraLen,extraData));
	   }

    if(fourCC::check(fcc,(uint8_t *)"SVQ3"))
          {

			  				     	return(decoders *)( new decoderFFSVQ3(w,h,extraLen,extraData));
	   }

    if(fourCC::check(fcc,(uint8_t *)"WMV2"))
          {

			  				     	return(decoders *)( new decoderFFWMV2(w,h,extraLen,extraData));
	   }
 	if(fourCC::check(fcc,(uint8_t *)"FFV1"))
          {

			  				     	return(decoders *)( new decoderFFV1(w,h));
	   }
#endif

/*
	Could be either divx5 packed crap or xvid or ffmpeg
	For now we return FFmpeg and later will switch to divx5 if available
		(ugly hack for ugly hack....)
*/

 	if(isMpeg4Compatible(fcc)==1)
    	{
					          	return(decoders *)( new decoderFFMpeg4(w,h));
						  //	return(decoders *)( new decoderXvid(w,h));
						  //	return(decoders *)( new decoderDIVX(w,h));
	}

          if(fourCC::check(fcc,(uint8_t *)"MJPG") || fourCC::check(fcc,(uint8_t *)"mjpa"))
          {
#if  0
 //#ifdef USE_MJPEG
                      			printf("\n using mjpeg codec\n");
			  				     	return(decoders *)( new decoderMjpeg(w,h));
#else
                      			printf("\n using FF mjpeg codec\n");
			  				     	return(decoders *)( new decoderFFMJPEG(w,h));
#endif

           }
          if(fourCC::check(fcc,(uint8_t *)"YV12"))
          {
            			printf("\n using null codec\n");
			       	return(decoders *)( new decoderNull(w,h));
           }
	if(fourCC::check(fcc,(uint8_t *)"UYVY"))
          {
            			printf("\n using uyvy codec\n");
			       	return(decoders *)( new decoderUYVY(w,h));
           }

	if((fcc==0)   ||   fourCC::check(fcc,(uint8_t *)"RGB "))
  			{
            // RGB 16 Codecs
            			printf("\n using RGB codec\n");
			       	return(decoders *)( new decoderRGB16(w,h));

       	}
#ifdef USE_THEORA

         if(fourCC::check(fcc,(uint8_t *)"VP31"))
         {
							printf("\n using Theora codec\n");
			       	return(decoders *)( new decoderTheora(w,h));
			    }
#else
   if(fourCC::check(fcc,(uint8_t *)"VP31"))
         {
							printf("\n using VP3 codec\n");
			       	return(decoders *)( new decoderVP3(w,h));
			    }
#endif          
         if(fourCC::check(fcc,(uint8_t *)"MPEG")
		|| fourCC::check(fcc,(uint8_t *)"mpg1")
	 )
         {
				printf("\n using Mpeg1/2 codec (libmpeg2)\n");
			    	return(decoders *)( new decoderMpeg(w,h,extraLen,extraData));
			     // 	return(decoders *)( new decoderFFMpeg12(w,h,extraLen,extraData));
			    }

        // default : null decoder
  			printf("\n using invalid codec for \n");
     		fourCC::print(fcc);

       	return(decoders *)( new decoders(w,h));


    	
   }

uint8_t coders::compress(uint8_t *in,uint8_t *out,uint32_t *len){
    UNUSED_ARG(in);
    UNUSED_ARG(out);
    UNUSED_ARG(len);
	return 0;
}
