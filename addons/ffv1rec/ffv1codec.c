/***************************************************************************
                        ffv1codec

	Interface to lavcodec codec family
                             -------------------
    begin                : Fri Jul 28 2003
    copyright            : (C) 2003 by mean
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
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <xvid.h>
#include "avcodec.h"

#include "ffv1.h"

static int			_w,_h;
AVCodecContext  *_context;
AVFrame		_frame;
int				pagesize=0;
void				YU_YV12_mmx(unsigned char *in,unsigned char *out, int w,int h);
/**********Xvid **********/
static XVID_ENC_FRAME 	fr;
static XVID_ENC_PARAM 	xparam;
static XVID_ENC_STATS 	xstats;
static void *xvid_handle=0;
static int nextIsKF=0;
static v4linfo	vinfo;
static int const divx4_motion_presets[7] = {
	0,
	PMV_EARLYSTOP16,
	PMV_EARLYSTOP16 | PMV_ADVANCEDDIAMOND16,
	PMV_EARLYSTOP16 | PMV_HALFPELREFINE16,
	PMV_EARLYSTOP16 | PMV_HALFPELREFINE16 | PMV_EARLYSTOP8 |
		PMV_HALFPELREFINE8,
	PMV_EARLYSTOP16 | PMV_HALFPELREFINE16 | PMV_EARLYSTOP8 |
		PMV_HALFPELREFINE8,
	PMV_EARLYSTOP16 | PMV_HALFPELREFINE16 | PMV_EXTSEARCH16 | PMV_EARLYSTOP8 |
		PMV_HALFPELREFINE8
};
static int const divx4_general_presets[7] = {
	0,
	0,
	0,
	0 | XVID_HALFPEL,
	0 | XVID_INTER4V | XVID_HALFPEL,
	0 | XVID_INTER4V | XVID_HALFPEL,
	0 | XVID_INTER4V | XVID_HALFPEL
};



/*----------------------*/
typedef enum
{
	FF_HUFF=1,
	FF_FFV1=2,
	FF_MJPEG=3,
	FF_MPEG4=4,
	FF_XVID=5
} FFCODEC;

typedef struct codecName
{
	FFCODEC codec;
	char *name;
}codecName;

codecName listOfCodec[]=
{
{FF_HUFF,"HUFFYUV"},
{FF_FFV1,"FFV1"},
{FF_MJPEG,"MJPEG"},
{FF_MPEG4,"MPEG4"},
{FF_XVID,"XVID"},
};

FFCODEC codec=FF_FFV1;
/****************************************

***************************************/

int FFV1_selectByName( char *name)
{
unsigned int i;
	for(i=0;i<sizeof(listOfCodec)/sizeof(codecName);i++)
	{
		if(!strcmp(name,listOfCodec[i].name))
		{
			codec=listOfCodec[i].codec;
			return 1;
		}

	}
	return 0;
}
/****************************************

***************************************/


int FFV1_XvidCompress(unsigned char *in,unsigned char *out,uint32_t  *outlen)
{
	XVID_ENC_FRAME xframe;

 	int xerr;
	memset(&xframe,0,sizeof(xframe));

	xframe.general = divx4_general_presets[vinfo.me];
	xframe.motion = divx4_motion_presets[vinfo.me];

	xframe.general |= XVID_H263QUANT;// + XVID_INTERLACING;

	xframe.bitstream = out;
	xframe.length = -1; 	// this is written by the routine

	xframe.image = in;
    	xframe.colorspace = XVID_CSP_I420;	// defined in <xvid.h>

	if(nextIsKF)
	{
    		xframe.intra = 1; // let the codec decide between I-frame (1) and P-frame (0)
		nextIsKF=0;
	}
	else
	{
    		xframe.intra = -1; // let the codec decide between I-frame (1) and P-frame (0)
	}
	xframe.quant = vinfo.quality;
//        xframe.quant = QUANTI;	// is quant != 0, use a fixed quant (and ignore bitrate)


	xerr =xvid_encore(xvid_handle, XVID_ENC_ENCODE, &xframe, &xstats);
	assert(XVID_ERR_OK==xerr);
	*outlen = xframe.length;
 	
	
	return 1;

}
/*-------------------------------------------------------------------------*/
int FFV1_Compress(unsigned char *in,unsigned char *out,uint32_t *outlen)
{
uint32_t sz;
unsigned char *src;

	if(codec==FF_XVID)
	{
		return FFV1_XvidCompress(in,out,outlen);
	}
#ifdef CONVERT_YUV
static char tmp[720*576*3];
#endif

	_frame.linesize[0]=_w;
       	_frame.linesize[1]=_w>>1;
       	_frame.linesize[2]=_w>>1;
	_frame.quality=vinfo.quality;

//	printf("\n in Qual : %f \n",_frame.quality);

#ifdef CONVERT_YUV
	src=tmp;
	YU_YV12_mmx(in,tmp, _w,_h);
#else
	src=in;
#endif

    	_frame.data[0]=(uint8_t *)src;
	_frame.data[1]=(uint8_t *)src+pagesize;
	_frame.data[2]=(uint8_t *)src+pagesize+(pagesize>>2);

	if(nextIsKF)
	{
        	_frame.key_frame = 1;
       		_frame.pict_type=FF_I_TYPE;
		nextIsKF=0;
	}
	else
	{
       		_frame.pict_type=0;
        	_frame.key_frame = 0;
	}
	if( (sz=avcodec_encode_video( 	_context,
					(uint8_t *)out,
				 	_w*_h*3,
	                         	&_frame))<0)
		{

			printf("error encoding !\n");
			exit(3);
		}
	//printf("\n out Qual : %f \n",_context->coded_frame->quality);

	*outlen=sz;
	return 1;
}
/*

__________________________________________
*/
int FFV1_XvidInit(v4linfo *info)
{
int err;
	printf("Initializing Xvid with width =%d, height = %d in YUV420P format\n",info->width,info->height);

    	XVID_INIT_PARAM xinit;

     	memset(&xinit,0,sizeof(xinit));
	memset(&fr, 0, sizeof(fr));

   	xinit.cpu_flags = XVID_CPU_MMX;
	xvid_init(NULL, 0, &xinit, NULL);

   	xparam.fincr = 1000;
	if(info->ntsc)
   		xparam.fbase =29970;
	else
   		xparam.fbase =25000;
	xparam.width 			= info->width;
	xparam.height 			= info->height;
	xparam.min_quantizer  	= info->quality;
	xparam.max_quantizer 	= info->quality;
	xparam.max_key_interval = info->keydist;
	err = xvid_encore(NULL, XVID_ENC_CREATE, &xparam, NULL);
 	if( XVID_ERR_OK!=err)
  	{
      		printf("\n Error initializing xvid !!!\n");
       		return 0;
     	}
	xvid_handle=xparam.handle;
	printf("Xvid successfully initialized\n");
 	return 1;

}

int FFV1_Init(v4linfo *info)
{
int res;
	_w=info->width;
	_h=info->height;
	memcpy(&vinfo,info,sizeof(vinfo));

	printf("Quality : %d \n",vinfo.quality);
	printf("Keydist : %d \n",vinfo.keydist);
	printf("ME      : %d \n",vinfo.me);
	if(codec==FF_XVID)
		return FFV1_XvidInit(info);

	printf("Initializing ffv1 with width =%d, height = %d in YUV420P format\n",info->width,info->height);
	_context=avcodec_alloc_context();
	memset(&_frame,0,sizeof(_frame));
	if(info->ntsc)
		_context->frame_rate         	= 29970;
	else
		_context->frame_rate         	= 25000;
	_context->width=info->width;
	_context->height=info->height;
	_context->frame_rate_base=1000;
	_context->strict_std_compliance=-1;
	
	pagesize=info->width*info->height;
	_context->pix_fmt		=  PIX_FMT_YUV420P; //PIX_FMT_YV12;
	_context->flags		= CODEC_FLAG_QSCALE;

	switch(codec)
	{
		case FF_HUFF: 	res=avcodec_open(_context,&huffyuv_encoder);break;
		case FF_FFV1: 	res=avcodec_open(_context,&ffv1_encoder);break;
		case FF_MJPEG: res=avcodec_open(_context,&mjpeg_encoder);break;
		case FF_MPEG4: res=avcodec_open(_context,&mpeg4_encoder);break;
		default: assert(0);
	}

	if(res<0)
		{
			printf("cannot init ffencoder");
			return 0;

		}
	_w=info->width;
	_h=info->height;
	printf("FFV1 init successfull\n");
	return 1;
}
/*---------------------------------------------------------*/
unsigned long int getVideoFourCC( void )
{
	switch(codec)
	{
		case FF_HUFF: 		return 'UYFH';
		case FF_FFV1: 		return '1VFF';
		case FF_MJPEG: 		return 'GPJM';
		case FF_MPEG4: 		return 'XVID';
		case FF_XVID: 		return 'DIVX';
		default: assert(0);
	}
	return 0;
}
/*---------------------------------------------------------*/
unsigned long int getVideoData( char **data )
{
	if(codec==FF_XVID) return 0;
	*data=(char *)_context->extradata;
         return _context->extradata_size;

}
void	FFV1_videoForceKeyFrame( void )
{
	nextIsKF=1;
}
// eof
