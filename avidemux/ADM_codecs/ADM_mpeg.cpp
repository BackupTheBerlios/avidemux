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
#include "ADM_codecs/ADM_mpeg.h"

#define MPEG2DEC  ((mpeg2dec_t *)_decoder)

extern "C"
{
#include "libMpeg2Dec/video_out.h"
#include "libMpeg2Dec/mpeg2.h"
#include "libMpeg2Dec/mpeg2_internal.h"
#include <ADM_assert.h>
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_CODEC
#include "ADM_toolkit/ADM_debug.h"


extern void 			mpeg2_pop(mpeg2dec_t *m);
extern void 			mpeg2_popI(mpeg2dec_t *m);
static vo_instance_t 		*yv12_open (void);
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
    uint8_t			*buffer;
} yv12_instance_t;

static uint8_t *iBuff[3];
static uint8_t *oBuff[3];
static int strideTab[3],strideTab2[3];

decoderMpeg::~decoderMpeg ()
{
#warning clean up libmpeg2 here
	kill_codec();
	delete [] unpackBuffer;
}
//____________________________________
uint8_t decoderMpeg::isMpeg1( void)
{
 mpeg2_decoder_t *dec;
  dec=&((MPEG2DEC)->decoder);
   return (dec->mpeg1);
}
//____________________-un init ____________________
uint8_t decoderMpeg::kill_codec (void)
{
  mpeg2_decoder_t *dec;
  dec=&((MPEG2DEC)->decoder);
  
  if(dec->quant)
  {
  	ADM_dealloc(dec->quant);
	dec->quant=NULL;
  }
  mpeg2_close (MPEG2DEC);
  _decoder=NULL;
  yv12_close (output);     

  return 1;
}
//________________________________________________
void decoderMpeg::setParam( void )
{


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
yv12_instance_t *inst;
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
			inst=(yv12_instance_t *)output;
			unpackBuffer=new uint8_t [(w*h*9)>>1];
			inst->buffer=unpackBuffer;
  			_decoder=mpeg2_init();
			dec=&((MPEG2DEC)->decoder);
			wmb=(_w+15)>>4;;
			hmb=(_h+15)>>4;;
			dec->quant_stride=wmb;
			dec->quant=(int8_t*)ADM_alloc( (wmb*hmb)*sizeof(int8_t));
			//
			feedData(extraLen,_seqHeader);
			feedData(extraLen,_seqHeader);
			
			// Post processing settings
			//___________________________
			
			_swapUV=0;
			// Post Proc is disabled by default
			
  			printf ("\n done\n");

}
/*------------------------------------------------------------------*/
uint8_t 	decoderMpeg::uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flag)
{
		if(flag) *flag=0;
#if defined( REMOVE_PADDING)
		while(*(in+len-1)==0) len--;
#endif		
		if(!len) return 1;
		feedData(len,in);

	const mpeg2_info_t  *info ;
	uint8_t *t;

		t=(uint8_t *) MPEG2DEC->fbuf[0]->buf[0];
		mpeg2_cleanup(MPEG2DEC);
		info= mpeg2_info (MPEG2DEC);
		out->_noPicture=0;    // For mpeg1/2 we ALWAYS have a picture
		if(out->quant && MPEG2DEC->decoder.quant_stride>=out->_qStride)
		{
                        

                        if(dontcopy())
                        {
                             out->_qSize=((_w+15)>>4)*((_h+15)>>4);
                             out->_qStride=(_w+15)>>4;
                             out->quant=(uint8_t *)MPEG2DEC->decoder.quant;                             
                        }
                        else
			     memcpy(out->quant,MPEG2DEC->decoder.quant,out->_qSize);
		
		}
                if(dontcopy())
                {
                        uint32_t plane=_w*_h;

                        out->_planes[0]=t;
                        out->_planes[1]=t+plane;
                        out->_planes[2]=t+((plane*5)>>2);
                        out->_planeStride[0]=_w;
                        out->_planeStride[1]=_w>>1;
                        out->_planeStride[2]=_w>>1;
                }
                else
                {
		      memcpy(out->data,t,(_w*_h*3)>>1);
                }
		
		switch(MPEG2DEC->decoder.coding_type)
			{
				case I_TYPE:out->flags=AVI_KEY_FRAME;break;
				case B_TYPE:out->flags=AVI_B_FRAME;break;
				case P_TYPE:out->flags=0;break;
				default:
					printf("\n unknown frame type ! %d\n",MPEG2DEC->decoder.coding_type);
					//ADM_assert(0);
					//return 0;
					out->flags=0;
			}
		if(flag)
		{
			*flag=out->flags;
		}
		if(MPEG2DEC->ext_state & PIC_FLAG_PROGRESSIVE_FRAME)
			{
				aprintf("Progressive\n");
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
	       printf("Not ready \n");
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
			//mpeg2_custom_fbuf(MPEG2DEC,1);
			output->setup_fbuf (output, buf, (void **)0);  // 3 buffer ...
			mpeg2_set_buf (MPEG2DEC, buf, (void *)0);
			output->setup_fbuf (output, buf, (void **)1);
			mpeg2_set_buf (MPEG2DEC, buf, (void *)1);
			output->setup_fbuf (output, buf, (void **)2);
			mpeg2_set_buf (MPEG2DEC, buf, (void *)2);
			_seqFound=0;
		}
		else
			printf("..already initialized...\n");

	    break;
	case STATE_PICTURE:
	    /* might skip */
	    /* might set fbuf */
		
		_seen=MPEG2DEC->decoder.coding_type;
		//printf("1st %d\n",_seen);
	    break;
	case STATE_PICTURE_2ND:
	    	/* should not do anything */
	    	//printf("2nd\n");
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

/**
	yv12_setup_fbuf : Allocate some buffers
*/
static void yv12_setup_fbuf (vo_instance_t * _instance,			    uint8_t ** buf, void ** id)
{
	uint8_t *all;
	uint32_t page,i,img;
    
	yv12_instance_t * instance = (yv12_instance_t *) _instance;
	i=(uint32_t)id;
 	printf(" **********YV12 setup fbuf called : %lu x %lu (%lu)\n",instance->w,instance->h,i);

 	page=instance->w*instance->h;
	img=(page*3)>>1;

	all=instance->buffer;
	all+=img*i;
	memset(all,0,img);
    buf[0] =all;
    buf[2] =all+page;;
    buf[1] =all+page+(page>>2);;
    //@*id=NULL;

}
