/***************************************************************************
                          ADM_vp3.cpp  -  description
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
#include <stdio.h>
#include <stdlib.h>
#include <ADM_assert.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "ADM_vp32/ADM_vp3.h"

#include "ADM_library/default.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_vp3.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_enter.h"

/*
   	Initialize codec
*/

decoderVP3::decoderVP3(uint32_t w,uint32_t h) :decoders(w,h)
{
           _handle=NULL;
            StartDecoder(&_handle, w, h);
			 _handle->PostProcessEnabled=1;
			 _handle->PostProcessingLevel = 6;
}

decoderVP3::~decoderVP3()
{
 	   StopDecoder(&_handle);

}

void decoderVP3::setParam( void )
{
int value=0;
		value=  _handle->PostProcessingLevel;
	  if(  DIA_GetIntegerValue(&value, 0, 9, "Post Processing level","Enter PostProc"))
	  {
			 	 _handle->PostProcessingLevel =(int)floor(value);
			  printf(" New VP3 postprocessing value : %d\n", _handle->PostProcessingLevel );
			}
     return;
}

/*
   	Uncompress frame, set flags if needed
*/
uint8_t 	decoderVP3::uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag) 		
{
    UNUSED_ARG(flag);
      	if (DecodeFrameToYUV(_handle, (char *)in, len, _w, _h)) 
       {
	    	fprintf(stderr, "DecodeFrameToYUV error\n");
	    	return 0;
		}
		// now unpack to out
//		memset(out+_w*_h,128,(_w*_h)>>1);
		
		uint8_t *ti,*to;
		uint8_t *ti2,*to2;
		
		uint32_t stride;
		
		to=out+(_w*_h)-_w;
		
		if(  _handle->PostProcessingLevel) 
		{
			    	ti=&(_handle-> PostProcessBuffer[_handle->ReconYDataOffset]);
			}
			else
			{
					ti=&(_handle-> LastFrameRecon[_handle->ReconYDataOffset]);
				}

		stride=_handle->Configuration.YStride;
		
		// luma
		for(uint32_t y=_h;y>0;y--)
		{
			memcpy(to,ti,_w);
			to-=_w;
			ti+=stride;							
		}
	
		to2=out+_w*_h;
		to=to2+((_w*_h)>>2);
       memset(to,128,_w>>1);
       memset(to2,128,_w>>1);
		
		to+=((_w*_h)>>2) - (_w>>1);
		to2+=((_w*_h)>>2) - (_w>>1);
		if(  _handle->PostProcessingLevel) 
		{
					ti=&(_handle-> PostProcessBuffer[_handle->ReconUDataOffset]);
					ti2=&(_handle-> PostProcessBuffer[_handle->ReconVDataOffset]);

			}
			else
		{		
		ti=&(_handle-> LastFrameRecon[_handle->ReconUDataOffset]);
		ti2=&(_handle-> LastFrameRecon[_handle->ReconVDataOffset]);
        }
		stride=_handle->Configuration.UVStride;
		
		// chorma u &v
		for(uint32_t y=_h>>1;y>0;y--)
		{
			memcpy(to,ti,_w>>1);
			memcpy(to2,ti2,_w>>1);

			to-=_w>>1;
			ti+=stride;							
			to2-=_w>>1;
			ti2+=stride;							

		}
          
		return 1;	
}


