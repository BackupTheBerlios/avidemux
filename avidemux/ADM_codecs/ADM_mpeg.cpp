/***************************************************************************
                          ADM_mpeg.h  -  description
                             -------------------

	Mpeg1/2 decoder interface tp libmpeg2

	We store the system header etc ... as extra data

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
#include "config.h"
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#if  defined( __FreeBSD__ ) || defined(__APPLE__)
#include <sys/types.h>
#endif
#include <stdlib.h>
#include <assert.h>
#include <string.h>
//#include <math.h>
#include "ADM_library/default.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "ADM_library/fourcc.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_mpeg.h"

#define MPEG2DEC  ((mpeg2dec_t *)_decoder)
extern int DIA_getMPParams( int *pplevel, int *ppstrength,int *swap);
extern "C"
{
#include "libMpeg2Dec/video_out.h"
#include "libMpeg2Dec/mpeg2.h"
#include "libMpeg2Dec/mpeg2_internal.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_CODEC
#include "ADM_toolkit/ADM_debug.h"


extern void 			mpeg2_pop(mpeg2dec_t *m);
extern void 			mpeg2_popI(mpeg2dec_t *m);
static vo_instance_t 	*yv12_open (void);
static void 			yv12_close (vo_instance_t * _instance);
static int 			yv12_setup (vo_instance_t * _instance, unsigned int width, 	
					unsigned int height,unsigned  int chroma_w, 
					unsigned int chroma_h,vo_setup_result_t *result);
static void 			yv12_setup_fbuf (vo_instance_t * _instance, uint8_t ** buf, void ** id);
extern int 			mpeg2_cleanup(mpeg2dec_t * mpeg2dec);
};

//static uint8_t *zero=NULL;

decoderMpeg 		*_father=NULL;
static vo_instance_t 		*output;
// avoid exporting our stuff...
typedef struct yv12_instance_s
{
    vo_instance_t 			vo;
   decoderMpeg	 		*father;
    uint32_t				w,h;
} yv12_instance_t;

static uint8_t *iBuff[3];
static uint8_t *oBuff[3];
static int strideTab[3],strideTab2[3];

decoderMpeg::~decoderMpeg ()
{
#warning clean up libmpeg2 here
	kill_codec();
}
//____________________init ____________________

uint8_t decoderMpeg::init_codec (void)
{

  printf ("\n initializing mpeg2 decoder");
  output =yv12_open ();
  _decoder=mpeg2_init();
  printf ("\n done\n");

  return 1;
}

//____________________-un init ____________________
uint8_t decoderMpeg::kill_codec (void)
{
  mpeg2_decoder_t *dec;
  dec=&((MPEG2DEC)->decoder);
  
  if(dec->quant)
  {
  	free(dec->quant);
	dec->quant=NULL;
  }
  mpeg2_close (MPEG2DEC);
  _decoder=NULL;
  yv12_close (output);  
  
  deletePostProc(&_postproc);

  return 1;
}
//________________________________________________
void decoderMpeg::setParam( void )
{
int postproc, strength,uv;
		postproc=_postproc.postProcType;
		strength=_postproc.postProcStrength;
		uv=_swapUV;
		if( DIA_getMPParams( &postproc,&strength,(int *)&_swapUV))
		{
				_postproc.postProcType=postproc;
				_postproc.postProcStrength=strength;				
				printf("\n new PP : %ld %ld \n",postproc,strength);
				updatePostProc(&_postproc );				

		}

        return; 
}	
/*
______________________________________________________________________________________
		Constructor for mpeg decoder
______________________________________________________________________________________
*/
decoderMpeg::decoderMpeg(uint32_t w,uint32_t h,uint32_t extraLen, uint8_t *extraData)
	: decoders(w,h)
{
mpeg2_decoder_t *dec;
uint32_t wmb,hmb;
			_seqLen=extraLen;
			if(extraLen)
			{
				_seqHeader=new uint8_t [extraLen];
				memcpy(_seqHeader,extraData,extraLen);
			}
			else
			{
				_seqHeader=NULL;
			}
			// store for future use
			_seqFound=0;
			postprocessed=NULL;
			// now init libmpeg2
 			printf ("\n initializing mpeg2 decoder %lu x %lu\n",_w,_h);
  			output =yv12_open ();
  			_decoder=mpeg2_init();
			dec=&((MPEG2DEC)->decoder);
			wmb=(_w+15)>>4;;
			hmb=(_h+15)>>4;;
			dec->quant_stride=wmb;
			dec->quant=(int8_t*)malloc( (wmb*hmb)*sizeof(int8_t));
			//
			feedData(extraLen,_seqHeader);
			feedData(extraLen,_seqHeader);
			
			// Post processing settings
			//___________________________
			
			_swapUV=0;
			// Post Proc is disabled by default
			initPostProc(&_postproc,w,h);	    
			updatePostProc(&_postproc);	
  			printf ("\n done\n");

}
/*------------------------------------------------------------------*/
uint8_t 	decoderMpeg::uncompress(uint8_t *in,uint8_t *out,uint32_t len,uint32_t *flag)
{
		if(flag) *flag=0;
		feedData(len,in);

	const mpeg2_info_t  *info ;
	uint8_t *t;

		t=(uint8_t *) MPEG2DEC->fbuf[0]->buf[0];
		mpeg2_cleanup(MPEG2DEC);
		info= mpeg2_info (MPEG2DEC);				
#ifndef ADM_BIG_ENDIAN_ZZ
		if(_postproc.postProcType && _postproc.postProcStrength)
			{ 	// we do postproc !
				// keep

		 		oBuff[0]=out;
		 		oBuff[1]=out+_w*_h;
 		 		oBuff[2]=out+((_w*_h*5)>>2);
				
				iBuff[0]=t;
		 		iBuff[1]=t+_w*_h;
 		 		iBuff[2]=t+((_w*_h*5)>>2);
        			
			        strideTab[0]=strideTab2[0]=_w;
				strideTab[1]=strideTab2[1]=_w>>1;
				strideTab[2]=strideTab2[2]=_w>>1;
/*
void  pp_postprocess(uint8_t * src[3], int srcStride[3],
                 uint8_t * dst[3], int dstStride[3],
                 int horizontalSize, int verticalSize,
                 QP_STORE_T *QP_store,  int QP_stride,
		 pp_mode_t *mode, 
		 pp_context_t *ppContext, 
		 int pict_type);

*/            			
		 		   pp_postprocess(
		      			iBuff,
		        		strideTab,
		          		oBuff,
		         		strideTab2,
		      			_w,
		        		_h,
		          		MPEG2DEC->decoder.quant,
		          		MPEG2DEC->decoder.quant_stride,
		         		_postproc.ppMode,
		          		_postproc.ppContext,
		          		MPEG2DEC->decoder.coding_type);	
					printf("Postprocessed\n");		  	
			}
		else			
#endif		
			memcpy(out,t,(_w*_h*3)>>1);
		// Or we get postprocessing info
		// get it in t, post process it to out
		if(flag)
		{
			switch(MPEG2DEC->decoder.coding_type)
			{
				case I_TYPE:*flag=AVI_KEY_FRAME;break;
				case B_TYPE:*flag=AVI_B_FRAME;break;
				case P_TYPE:*flag=0;break;
				default:
					printf("\n unknown frame type ! %d\n",MPEG2DEC->decoder.coding_type);
					//assert(0);
					//return 0;
					*flag=0;
			}
		}

		return 1;
}
void decoderMpeg::feedData(uint32_t len, uint8_t *data)
{
		decode_mpeg2(data,data+len);

}
void decoderMpeg::setFather( decoderMpeg *f)
{
		_father=f;
}


/**
	decode_mpeg2: Just a copy/paste from mpeg2dec

*/
void  decoderMpeg::decode_mpeg2 (uint8_t * current, uint8_t * end)
{
    const mpeg2_info_t * info;
    int state;
    int loop=0;

    mpeg2_buffer (MPEG2DEC, current, end);

    info = mpeg2_info (MPEG2DEC);
    while (1) {
    	
	state = mpeg2_parse (MPEG2DEC);
	aprintf("Mpeg2dec : %d \n",state);
	switch (state) {
	case STATE_BUFFER:
			loop++;
			if(loop>2)
				{
					aprintf("Mpeg2dec: Running empty...\n");
					return;
				}
			break;
	case -1:
	    return;
	case STATE_SEQUENCE:
	    /* might set nb fbuf, convert format, stride */
	    /* might set fbufs */
	    printf("\n Seq found\n");
	    
	    vo_setup_result_t res;
	    if(!_seqFound)
	    {
	    	if (output->setup (output,
	    			info->sequence->width,
			       	info->sequence->height,
	    			info->sequence->width>>1,
			       	info->sequence->height>>1,
				&res))
			{
					fprintf (stderr, "display setup failed\n");
					exit (1);
	    		}
	 		uint8_t * buf[3];
			void * id;

			output->setup_fbuf (output, buf, &id);  // 3 buffer ...
			mpeg2_set_buf (MPEG2DEC, buf, id);
			output->setup_fbuf (output, buf, &id);
			mpeg2_set_buf (MPEG2DEC, buf, id);
			output->setup_fbuf (output, buf, &id);
			mpeg2_set_buf (MPEG2DEC, buf, id);
			_seqFound=1;
		}
		else
			printf("..already initialized...\n");

	    break;
	case STATE_PICTURE:
	    /* might skip */
	    /* might set fbuf */

	    break;
	case STATE_PICTURE_2ND:
	    /* should not do anything */
	    break;
	case STATE_SLICE:
	case STATE_END:
	    /* draw current picture */
	    /* might free frame buffer */

	    break;
	}
    }
}


/**
		yv12_open : set up the callbacks
*/
vo_instance_t * yv12_open (void)
{
    yv12_instance_t * instance;

    instance = new yv12_instance_t;
    if (instance == NULL)
				return NULL;
    memset(instance,0,sizeof(*instance));

    instance->vo.setup = yv12_setup;
    instance->vo.setup_fbuf = yv12_setup_fbuf;
    instance->vo.set_fbuf = NULL;
    instance->vo.draw = NULL;
    instance->vo.discard = NULL;
    instance->vo.close = yv12_close;
    instance->father=_father;

	   printf(" YV12 open called\n");

    return (vo_instance_t *) instance;
}
/**
		yv12_setup : Nothing to do
*/
int yv12_setup (vo_instance_t * _instance, unsigned int width, unsigned int height,unsigned int chroma_w,unsigned  int chroma_h,vo_setup_result_t *result)
{
	yv12_instance_t *ins;

	ins=(yv12_instance_t *)_instance;
	ins->w=width;
	ins->h=height;
    result->convert=NULL;
    printf(" YV12 setup called\n");
    return 0;
}
/**
	yv12_close : nothing to do
*/
void yv12_close (vo_instance_t * _instance)
{
   UNUSED_ARG(_instance);
   printf(" yv12close called\n");
}
#ifdef HAVE_MEMALIGN
/* some systems have memalign() but no declaration for it */
void * memalign (size_t align, size_t size);
#else
/* assume malloc alignment is sufficient */
#define memalign(align,size) malloc (size)
#endif

/**
	yv12_setup_fbuf : Allocate some buffers
*/
static void yv12_setup_fbuf (vo_instance_t * _instance,			    uint8_t ** buf, void ** id)
{
	uint8_t *all;
	uint32_t page;
    yv12_instance_t * instance = (yv12_instance_t *) _instance;
 printf(" **********YV12 setup fbuf called : %lu x %lu\n",instance->w,instance->h);

 	page=instance->w*instance->h;
	all=(uint8_t *)malloc((page*3)>>1);
	memset(all,0,(page*3)>>1);
    buf[0] =all;
    buf[2] =all+page;;
    buf[1] =all+page+(page>>2);;
    *id=NULL;

}
