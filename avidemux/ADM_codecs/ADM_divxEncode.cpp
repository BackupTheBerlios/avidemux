/***************************************************************************
                          divx_encode.cpp  -  description

		Encoder for Divx 5.0.5

                             -------------------
    begin                : Sun Feb 17 2002
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
#include <assert.h>
#include <string.h>
#include <math.h>

#include "config.h"
#include "avi_vars.h"
#include "prototype.h"
#ifdef BIG_ENDIAN
#undef BIG_ENDIAN
#endif
//#include "decore.h"
#include "config.h"
#ifdef USE_DIVX
#include "ADM_codecs/ADM_divxInc.h"
#include "ADM_colorspace/colorspace.h"
#include "ADM_codecs/ADM_divxEncode.h"

#include "ADM_toolkit/toolkit.hxx"

static 	ENC_RESULT 			result;
static   	tagDivXBitmapInfoHeader 	tag;
static 	SETTINGS 				setting;

myENC_RESULT 	res;

divxEncoder::divxEncoder(uint32_t width,uint32_t height) :encoder (width,height)
{
                             _init=0;
			     _handle=NULL;
}
uint8_t     divxEncoder::stopEncoder(void )
{
    int ret;

    assert(_init);
    ret = encore(_handle, ENC_OPT_RELEASE, 0, 0);
    _init = 0;
    if (ret == ENC_OK)
		return 1;
    return 0;
}
//
//	Return encoding result from the last encoded frame
//
uint8_t divxEncoder::getResult( void *ress)
{
   memcpy(ress,&res,sizeof(res));
   return 1;
}

uint8_t divxEncoder::init( uint32_t val,uint32_t fps1000) {
    UNUSED_ARG(val);
    UNUSED_ARG(fps1000);
	assert(0);
}

uint8_t divxEncoder::encode(
				uint8_t * in,
			    	uint8_t * out,
			    	uint32_t * len,
       				uint32_t * flags)
{

    int ret;
ENC_FRAME frame;

    assert(_init);
	memset(&frame,0,sizeof(frame));
	frame.image=in;
	frame.bitstream=out;

    ret = encore(_handle, ENC_OPT_ENCODE, &frame, &result);
    if(ENC_OK!=ret)
    {
		printf("Error encoding frame divx :%d \n",ret);
		return 0;
    }
    *len = (uint32_t) frame.length;
    *flags=0;
    switch(result.cType)
    {
		case '0':
				*flags=0;
				*len=0;
				break;
		case 'I':
				*flags=AVI_KEY_FRAME;
				break;
		case 'P':
				break;
		case 'B':
				*flags=AVI_B_FRAME;
				break;

    }
    return 1;
}
uint8_t divxEncoder::params( uint32_t fps1000 )
{

	if(ENCORE_VERSION != encore(0, ENC_OPT_VERSION, 0, 0))
	//? interfaces are incompatible, encoding can?t be performed
	{
		printf("\n Header & lib mismatch for divx !!!\n");
        	return 0;
	}

	memset(&tag,0,sizeof(tag));
	//tag.biSize=sizeof(tag);
	tag.biWidth=_w;
	tag.biHeight=_h;
	tag.biBitCount=24;
	tag.biCompression=fourCC::get((uint8_t *)"YV12");

  	memset(&setting,0,sizeof(setting));
	setting.use_bidirect=0;
	setting.input_frame_period=fps1000;
	setting.input_clock=fps1000;
	setting.complexity_modulation=0.5;
	setting.quality=5;

	return 1;
}

/**--------------------------------------
	Initialiaze divxencoder
	in constant quantizer mode
------------------------------------------
*/

uint8_t divxEncoderCQ::init(uint32_t q,uint32_t fps1000)
{
    UNUSED_ARG(fps1000);
    _q=q;

	printf(" Compressing %lu x %lu video in qz %lu\n", _w, _h, _q);

// check header and lib match -> good idea !


	if(!params(fps1000)) return 0;

	setting.vbr_mode=RCMODE_1PASS_CONSTANT_Q;
	setting.quantizer=_q;
    	assert(0 == _init);

	if(ENC_OK!=encore((void *)&_handle, ENC_OPT_INIT, &tag, &setting))
	{
		printf("\n error initializing divx !\n");
		return 0;

	}
	printf("\n Divx initialized  OK in CQ mode!\n");
    _init = 1;
    return 1;
}
/**--------------------------------------
	Initialiaze divxencoder
	in constant bitrate mode
------------------------------------------
*/

uint8_t divxEncoderCBR::init(uint32_t q,uint32_t fps1000)
{
    UNUSED_ARG(fps1000);
    _br=q;

	printf(" Compressing %lu x %lu video in CBR %lu\n", _w, _h, _br);

// check header and lib match -> good idea !

	if(!params(fps1000)) return 0;

	setting.vbr_mode=RCMODE_502_1PASS; //RCMODE_VBV_1PASS;
	setting.bitrate=_br;
    	assert(0 == _init);

	if(ENC_OK!=encore((void*)&_handle, ENC_OPT_INIT, &tag, &setting))
	{
		printf("\n error initializing divx !\n");
		return 0;

	}
    _init = 1;
    printf("\n Divx initialized OK in CBR mode  !\n");
    return 1;
}



#endif
