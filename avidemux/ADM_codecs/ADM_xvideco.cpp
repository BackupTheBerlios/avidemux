/***************************************************************************
                          ADM_xvideco.cpp  -  description
                             -------------------
    begin                : Fri Aug 23 2002
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
#include <stdio.h>
#include <stdlib.h>
#include <ADM_assert.h>
#include <string.h>
#include <math.h>

#include "config.h"
#ifdef USE_XX_XVID


#include "ADM_library/default.h"
#include "xvid.h"
#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_xvideco.h"
//#include "ADM_gui/GUI_decodersettings.h"


typedef int(*XVID_PROTO) (void *handle, int opt,  void *param1, void *param2);



static  		XVID_INIT_PARAM xinit;
static        XVID_DEC_PARAM xparam;
extern XVID_PROTO xx_xvid_init;
extern XVID_PROTO xx_xvid_encore;
extern XVID_PROTO xx_xvid_decore;
/*
  class decoderXvid : decoders
{
     protected:
      			DEC_MEM_REQS decMemReqs;
				DEC_PARAM decParam;
			 	DEC_FRAME decFrame;
			    DEC_FRAME_INFO frame_info;
          		uint8_t						first;


     public:
     										decoderXvid(uint32_t w,uint32_t h);
         		virtual					~decoderXvid();
    			virtual uint8_t 	uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag=NULL) 		;	
		       virtual 			void setParam( void );
}   ;
*/
/*
   	Initialize codec
*/

decoderXvid::decoderXvid(uint32_t w,uint32_t h) :decoders(w,h)
{

             int xerr;

		_handle=NULL;

#ifdef USE_MMX
         xinit.cpu_flags=    XVID_CPU_MMX;
#else
		xinit.cpu_flags = 0;
#endif

        xx_xvid_init(NULL, 0, &xinit, NULL);

        xparam.width = w;
        xparam.height = h;

        xerr = xx_xvid_decore(NULL, XVID_DEC_CREATE, &xparam, NULL);
        _handle = xparam.handle;

        ADM_assert(xerr==XVID_ERR_OK);


}

decoderXvid::~decoderXvid()
{
 	if(_handle)
			{
            	   int xerr;
      			  xerr = xx_xvid_decore(_handle, XVID_DEC_DESTROY, NULL, NULL);
                if(xerr!=XVID_ERR_OK) { printf("\n cannot destroy xvid encoder ....\n");}
					_handle=NULL;
			}

}
void decoderXvid::setParam( void )
{


}

/*
   	Uncompress frame, set flags if needed
*/
uint8_t 	decoderXvid::uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag) 		
{



	  int xerr;
        XVID_DEC_FRAME xframe;
		
		// Null frame -> drop
		if(len==0)
					{
                   		if(flag) *flag=0;
							return 1;
					}

        xframe.general =0;
        xframe.bitstream = in;
        xframe.length = len;
        xframe.image = out;
        xframe.stride = _w;
        xframe.colorspace = XVID_CSP_YV12;             // XVID_CSP_USER is fastest (no memcopy involved)

        xerr = xx_xvid_decore(_handle, XVID_DEC_DECODE, &xframe, NULL);

        if(xerr!=XVID_ERR_OK)
			{
            		len=0;
					if(flag) *flag=0;
					return 0;
			}
					if(flag) *flag=0;
		
			return 1;				
}


#endif
