/***************************************************************************
                          ADM_ffmp43.cpp  -  description
                             -------------------
                             
	Decoder for Divx3/4/..., using ffmpeg
                             
    begin                : Wed Sep 25 2002
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
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef USE_FFMPEG
#include <ADM_assert.h>

#include "ADM_lavcodec.h"
#include "ADM_library/default.h"

#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"

#include "ADM_codecs/ADM_ffmp43.h"
//#include "ADM_gui/GUI_MPP.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME  MODULE_CODEC
#include "ADM_toolkit/ADM_debug.h"
//****************************
#define WRAP_Open(x) \
{\
AVCodec *codec=avcodec_find_decoder(x);\
if(!codec) {GUI_Alert("Internal error finding codec"#x);ADM_assert(0);} \
  if (avcodec_open(_context, codec) < 0)  \
                      { \
                                        printf(" Decoder init: Lavcodec :"#x" video decoder failed!\n"); \
                                        GUI_Alert("Internal error opening "#x); \
                                        ADM_assert(0); \
                                } \
                                else \
                                { \
                                        printf(" Decoder init: lavcodec "#x" video decoder initialized!\n"); \
                                } \
} 
//****************************
extern uint8_t DIA_lavDecoder(uint32_t *swapUv, uint32_t *showU);
extern "C"
{
int av_is_voppacked(AVCodecContext *avctx, int *vop_packed, int *gmc, int *qpel);
};

uint8_t decoderFF::clonePic(AVFrame *src,ADMImage *out)
{
uint32_t u,v;
                                        out->_planes[0]=(uint8_t *)src->data[0];
                                        out->_planeStride[0]=src->linesize[0];
                                        if(_swapUV)
                                        {
                                                u=1;v=2;
                                        }else
                                        {
                                                u=2;v=1;
                                        }
                                        out->_planes[1]=(uint8_t *)src->data[u];
                                        out->_planeStride[1]=src->linesize[u];

                                        out->_planes[2]=(uint8_t *)src->data[v];
                                        out->_planeStride[2]=src->linesize[v];

                                        _lastQ=0; //_context->quality;
                                        out->_Qp=(src->quality*32)/FF_LAMBDA_MAX;
                                        out->flags=frameType();
                                        
                                        // Quant ?
                                        if(src->qstride && src->qscale_table)    
                                        {
                                                out->quant=(uint8_t *)src->qscale_table;
                                                out->_qStride=src->qstride;
                                                out->_qSize=(_w+15)>>4;
                                                out->_qSize*=(_h+15)>>4; // FixME?
                                        }
                                        else
                                        {
                                                out->_qSize=out->_qStride=0;
                                                out->quant=NULL;
                                        }

}

uint8_t  decoderFF::isDivxPacked( void )
{
	int vop, gmc,qpel;
	av_is_voppacked(_context,&vop,&gmc,&qpel);
	return vop;
}
// Fill the bitfields for some mpeg4 specific info
// It is a bit of a hack as we make it a general
// stuff (i.e. shared with all codecs) whereas it is mpeg4 specific
// and should stay within mpeg4 scope FIXME
uint32_t decoderFF::getSpecificMpeg4Info( void )
{
	int vop, gmc,qpel;
	uint32_t out=0;
	av_is_voppacked(_context,&vop,&gmc,&qpel);
	
	if(qpel) out+=ADM_QPEL_ON;
        if(_gmc) out+=ADM_GMC_ON;
	if(vop) out+=ADM_VOP_ON;	
	
	return out;
}

//________________________________________________
void decoderFF::setParam( void )
{
	DIA_lavDecoder(&_swapUV,&_showMv);
        return; // no param for ffmpeg
}

//-------------------------------
decoderFF::decoderFF(uint32_t w,uint32_t h) :decoders(w,h)
{

//				memset(&_context,0,sizeof(_context));
				_allowNull=0;
                                _gmc=0;
				_context=NULL;
                                _refCopy=0;
#if LIBAVCODEC_BUILD >= 4624
   	_context=avcodec_alloc_context();
#else
		_context=new AVCodecContext;
    memset(_context, 0, sizeof(AVCodecContext));
#endif
				ADM_assert(_context);
				memset(&_frame,0,sizeof(_frame));

				_context->max_b_frames=0;

		  		_context->width=_w;
				_context->height=_h;
			  	_context->pix_fmt=PIX_FMT_YUV420P;//PIX_FMT_RGBA32
				//_context->debug=1;

			    _internalBuffer=new uint8_t[w*h*3];
			   
			    _swapUV=0;
				//_context->strict_std_compliance=-1;
				
				_showMv=0;
#define FF_SHOW		(FF_DEBUG_VIS_MV_P_FOR+	FF_DEBUG_VIS_MV_B_FOR+FF_DEBUG_VIS_MV_B_BACK)
//#define FF_SHOW		(FF_DEBUG_VIS_MV_P_FOR)
			    printf("FFMpeg build : %d\n", LIBAVCODEC_BUILD);
			_context->debug_mv 	|=FF_SHOW;
			_context->debug		|=FF_DEBUG_VIS_MB_TYPE+FF_DEBUG_VIS_QP;
}

//_____________________________________________________

decoderFF::~decoderFF()
{
		avcodec_close(_context);
		ADM_dealloc(_context);
		delete [] _internalBuffer;	
		printf("FF base destroyed\n");
}

//-------------------------------
uint32_t  decoderFF::frameType( void )
{
	uint32_t flag=0;

	#define SET(x) flag=x;aprintf(" Frame is %s\n",#x);

	switch(_frame.pict_type)
	{
		case FF_B_TYPE:
				SET(AVI_B_FRAME);
				if(_frame.key_frame)
					aprintf("\n But keyframe is  set\n");
				break;
		
		case FF_I_TYPE:
				SET(AVI_KEY_FRAME);
				if(!_frame.key_frame)
					aprintf("\n But keyframe is not set\n");
				break;
                case FF_S_TYPE:
                                _gmc=1; // No break, just inform that gmc is there
		case FF_P_TYPE:
				SET(AVI_P_FRAME);
				if(_frame.key_frame)
					aprintf("\n But keyframe is  set\n");
				break;
		default:
//				printf("\n OOops XXX frame ?\n");
				break;
		}
	return flag;
}
uint8_t decoderFF::decodeHeaderOnly(void )
{
	_context->hurry_up=5;
	printf("\n FFmpeg: Hurry up\n");
	return 1;
}
uint8_t decoderFF::decodeFull(void )
{
	_context->hurry_up=0;
	printf("\n FFmpeg: full decoding\n");
	return 1;
}

uint8_t     decoderFF::uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flagz)

{
 int got_picture=0;
 uint8_t *oBuff[3];
 int strideTab[3];
 int strideTab2[3];
 int ret=0;
                out->_noPicture=0;
 		if(_showMv) 
		{
			_context->debug_mv 	|= FF_SHOW;
			_context->debug		|=0;//FF_DEBUG_VIS_MB_TYPE;
			//_context->debug	|=FF_DEBUG_VIS_MB_TYPE+FF_DEBUG_VIS_QP;
		}
		else 
		{
			_context->debug_mv 	&=~FF_SHOW;
			_context->debug		&=~(FF_DEBUG_VIS_MB_TYPE+FF_DEBUG_VIS_QP);
		}
			
		if(len==0 && !_allowNull) // Null frame, silently skipped
				{
                                        if(flagz) *flagz=AVI_KEY_FRAME;
                                        printf("\n ff4: null frame\n");
                                        {
                                                // search the last image
                                                if(_context->coded_frame->data)
                                                {
                                                        clonePic(_context->coded_frame,out);
                                                        if(flagz)
                                                               *flagz=out->flags;
                                                }
                                                else                                                
                                                        out->_noPicture=1;                                                
                                        }
                                        return 1;
				}

		ret= avcodec_decode_video(_context,&_frame,&got_picture, in, len);
		out->_qStride=0; //Default = no quant
		if(0>ret && !_context->hurry_up)
		{
					printf("\n error in FFMP43/mpeg4!\n");
					printf("Err: %d, size :%d\n",ret,len);
					return 0;
		}
		if(!got_picture && !_context->hurry_up)
		{
				// Some encoder code a vop header with the 
				// vop flag set to 0
				// it is meant to mean frame skipped but very dubious
				if(len<=8)
					{
						printf("Probably pseudo black frame...\n");
						out->_Qp=2;
						out->flags=0; // assume P ?

                                                clonePic(_context->coded_frame,out);
						if(flagz)
							*flagz=out->flags;
						return 1;
					}
				// allow null means we allow null frame in and so potentially
				// have no frame out for a time
				// in that case silently fill with black and returns it as KF
				if(_allowNull)
				{
					if(flagz) *flagz=AVI_KEY_FRAME;
                                        if(!_refCopy)
                                        {
					   memset(out->data,0,_w*_h);
					   memset(out->data+_w*_h,128,(_w*_h)>>1);
                                        }
                                        else
                                        {
                                           out->_noPicture=1;
                                        }
					printf("\n ignoring got pict ==0\n");
					return 1;

				}
					printf("Err: %d, size :%d\n",ret,len);
					printf("\n error in FFMP43/mpeg4!: got picture \n");
					//GUI_Alert("Please retry with misc->Turbo off");
					//return 1;
					return 0;			 			
		}		  
		if(_context->hurry_up)
		{
			if(flagz)
			{
			*flagz=frameType();
			}
 			return 1;
		}

                switch(_context->pix_fmt)
                {
                case PIX_FMT_YUV411P:
                        out->_colorspace=ADM_COLOR_YUV411;
                        break;

                case PIX_FMT_YUV422P:
                case PIX_FMT_YUVJ422P:
                        out->_colorspace=ADM_COLOR_YUV422;
                        break;

                case PIX_FMT_YUV420P:
                case PIX_FMT_YUVJ420P:
                // Default is YV12 or I420
                // In that case depending on swap u/v
                // we do it or not
                        out->_colorspace=ADM_COLOR_YV12;
                        break;
                break;
                case PIX_FMT_RGBA32:
                        out->_colorspace=ADM_COLOR_RGB32A;
                        break;
                case PIX_FMT_RGB555:
                        out->_colorspace=ADM_COLOR_BGR555;
                        break;
                default:
                                printf("\n Unhandled colorspace:%d\n",_context->pix_fmt);
                                return 0;
                }
                clonePic(&_frame,out);
                if(flagz)
                        *flagz=out->flags;
                return 1;
}



decoderFFDiv3::decoderFFDiv3(uint32_t w,uint32_t h)    :decoderFF(w,h) 
{
                      _refCopy=1; // YUV420 only
                        WRAP_Open(CODEC_ID_MSMPEG4V3);
}              
decoderFFMpeg4VopPacked::decoderFFMpeg4VopPacked(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
// force low delay as avidemux don't handle B-frames

      _refCopy=1; // YUV420 only
	_allowNull=1;
        WRAP_Open(CODEC_ID_MPEG4);
}
decoderFFMpeg4::decoderFFMpeg4(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
// force low delay as avidemux don't handle B-frames

		  _context->flags|=CODEC_FLAG_LOW_DELAY;
                  _refCopy=1; // YUV420 only
 		//  _context->flags|=FF_DEBUG_VIS_MV;
       WRAP_Open(CODEC_ID_MPEG4);
}
decoderFFDV:: decoderFFDV(uint32_t w,uint32_t h,uint32_t l,uint8_t *d)        :decoderFF(w,h)
{
      _context->extradata=(void *)d;
      _context->extradata_size=(int)l;
      WRAP_Open(CODEC_ID_DVVIDEO);
	
}
decoderFFMP42::decoderFFMP42(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
      _refCopy=1; // YUV420 only
        WRAP_Open(CODEC_ID_MSMPEG4V2);

}
decoderFFMpeg12::decoderFFMpeg12(uint32_t w,uint32_t h,uint32_t extraLen,uint8_t *extraData)
	    :decoderFF(w,h)
{
int got_picture=0;
  	_context->flags|=CODEC_FLAG_LOW_DELAY;
        _refCopy=1; // YUV420 only
      WRAP_Open(CODEC_ID_MPEG2VIDEO);
}
decoderFFSVQ3::decoderFFSVQ3(uint32_t w,uint32_t h,uint32_t extraLen,uint8_t *extraData)
	    :decoderFF(w,h)
{
int got_picture=0;
	
  	_context->flags|=CODEC_FLAG_LOW_DELAY;
	 _context->extradata=(void *)extraData;
      _context->extradata_size=(int)extraLen;
     WRAP_Open(CODEC_ID_SVQ3);
}

decoderFFH263::decoderFFH263(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
      _refCopy=1; // YUV420 only
      WRAP_Open(CODEC_ID_H263);

}
decoderFFV1::decoderFFV1(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
      _refCopy=1; // YUV420 only
      WRAP_Open(CODEC_ID_FFV1);
}
decoderFF_ffhuff::decoderFF_ffhuff(uint32_t w,uint32_t h,uint32_t l,uint8_t *d)       :decoderFF(w,h) 
{
      _context->extradata=(void *)d;
      _context->extradata_size=(int)l;
      printf("FFhuff: We have :%d bytes of extra data\n",l);
     WRAP_Open(CODEC_ID_FFVHUFF);
        
}
decoderFFH264::decoderFFH264(uint32_t w,uint32_t h)  :decoderFF(w,h) 
{
  _refCopy=1; // YUV420 only
  printf("Initializing lavcodec H264 decoder\n");
 WRAP_Open(CODEC_ID_H264);
  
}
decoderFFhuff::decoderFFhuff(uint32_t w,uint32_t h,uint32_t l,uint8_t *d)       :decoderFF(w,h) 
{
      _context->extradata=(void *)d;
      _context->extradata_size=(int)l;
      
     WRAP_Open(CODEC_ID_HUFFYUV);
}
decoderFFWMV2::decoderFFWMV2(uint32_t w,uint32_t h,uint32_t l,uint8_t *d)       :decoderFF(w,h)
{
      _context->extradata=(void *)d;
      _context->extradata_size=(int)l;
      
     WRAP_Open(CODEC_ID_WMV2);
	
}
decoderFFcyuv::decoderFFcyuv(uint32_t w,uint32_t h,uint32_t l,uint8_t *d)       :decoderFF(w,h)
{
      _context->extradata=(void *)d;
      _context->extradata_size=(int)l;

      WRAP_Open(CODEC_ID_CYUV);
}
decoderFFMJPEG::decoderFFMJPEG(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
            WRAP_Open(CODEC_ID_MJPEG);
}
decoderSnow::decoderSnow(uint32_t w,uint32_t h)       :decoderFF(w,h)
{
           WRAP_Open(CODEC_ID_SNOW);
}
decoderCamtasia::decoderCamtasia(uint32_t w,uint32_t h,uint32_t bpp)       :decoderFF(w,h)
{
           _context->bits_per_sample=bpp;
           WRAP_Open(CODEC_ID_TSCC);
}

#endif
