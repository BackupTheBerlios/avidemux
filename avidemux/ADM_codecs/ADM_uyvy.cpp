//
// C++ Implementation: ADM_uyvy
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"
#include <stdio.h>

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if  defined( __FreeBSD__ ) || defined(__APPLE__)
#include <sys/types.h>
#endif
#include <stdlib.h>

#include <string.h>
//#include <math.h>
#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "ADM_library/fourcc.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_uyvy.h"
#include <ADM_assert.h> 
 uint8_t decoderUYVY::uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flag)
{

	if(len<_w*_h*2)
	{
		printf("in:%d expected%d\n",len,_w*_h*2);
		return 1;
	}
	uint8_t *ptrY,*ptrU,*ptrV,*ptr;
	
	ptr=in;
	ptrY=out->data;
	ptrU=out->data+_w*_h;
	ptrV=ptrU+((_w*_h)>>2);
	
	for(uint32_t y=0;y<_h;y++)
		for(uint32_t x=0;x<(_w>>2);x++)
		{
			if(!(y&1))
			{
			*ptrU++=(*(ptr)+*(ptr+_w))>>1;
			ptr++;
			*ptrY++=*ptr++;
			*ptrV++=(*(ptr)+*(ptr+_w))>>1;
			ptr++;
			*ptrY++=*ptr++;
			}
			else
			{
				*ptrY++=*(++ptr);
				*ptrY++=*(ptr+2);
				ptr++;
			}
		}

		if(flag) *flag=AVI_KEY_FRAME;
	return 1;

}
uint8_t decoderYUY2::uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flag)
{

	if(len<_w*_h*2)
	{
		printf("in:%d expected%d\n",len,_w*_h*2);
		return 1;
	}
	uint8_t *ptrY,*ptrU,*ptrV,*ptr;
	
	ptr=in;
	ptrY=out->data;
	ptrV=out->data+_w*_h;
	ptrU=out->data+((_w*_h*5)>>2);
	
	
	for(uint32_t y=0;y<_h;y++)
		for(uint32_t x=0;x<(_w>>1);x++)
		{
			if(!(y&1))
			{
			*ptrY++=*ptr++;
			*ptrU++=(*(ptr)+*(ptr+_w*2))>>1;
			ptr++;
			*ptrY++=*ptr++;
			*ptrV++=(*(ptr)+*(ptr+_w*2))>>1;
			ptr++;
			
			}
			else
			{
				*ptrY++=*(ptr);
				*ptrY++=*(ptr+2);
				ptr+=4;
			}
		}

		if(flag) *flag=AVI_KEY_FRAME;
	return 1;

}

