//
// C++ Implementation: X264 encoder
//
// Description: 
//
//
// Author: Mean (fixounet at free dot fr)
// I'm afraid we will have to borrow one more time from mplayer
// to understand how it works
// At least, the interface/x264.h file is clean and simple
// That's a beginning.
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include "ADM_codecs/ADM_codec.h"
#ifdef USE_X264

#include "ADM_codecs/ADM_x264.h"
extern "C"
{
        #include "x264.h"
};        

#include <ADM_assert.h>

#define HANDLE ((x264_t *)_handle)                     
#define PICS ((x264_picture_t *)_pic)                     

// Yes, ugly FIXME
static  x264_param_t          param;

//********************************************* 
// Set the common stuff prior to codec opening
// ratecontrol stuff is left to the caller
// to initialize
//*********************************************
uint8_t   X264Encoder::preamble(uint32_t fps1000,ADM_x264Param *zparam)
{      
  
  x264_t                *xhandle=NULL;
 
        printf("Opening X264\n");
  
        param.cpu=0; // Will be slow ...
        param.i_width=_w;
        param.i_height=_h;
        param.i_csp=X264_CSP_I420;
        
        param.vui.i_sar_width=_w;       // FIXME
        param.vui.i_sar_height=_h;
        
        param.i_fps_num=fps1000;
        param.i_fps_den=1000;
        param.i_maxframes=0;
        if(zparam)
        {
                param.i_frame_reference=1;              //  ref frame like mpeg1/2/4
                param.i_keyint_max=zparam->maxKf;       // Max intra distance
                param.i_keyint_min=zparam->minKf;       // Min intra distance
                param.i_bframe=zparam->nbBframe;        // 2 bframe
                param.b_cabac=zparam->cabac;            // Arith coding on/off
        }
        
        
        xhandle=x264_encoder_open(&param);
        if(!xhandle) return 0;
        
        _handle=(void *)xhandle;
        _pic=(void *)new x264_picture_t;
        printf("X264 init ok\n");
        return 1;
  
}
// should never be called (pure)
//*******************************
uint8_t         X264Encoder::init( uint32_t val,uint32_t fps1000,ADM_x264Param *param )
{
  ADM_assert(0);
}
//*******************************
uint8_t         X264Encoder::stopEncoder(void )
{
  if(_handle)
  {
    x264_encoder_close(HANDLE);
    _handle=NULL;
  }
  if(_pic)    
  {
    // picture_clean ?
    delete PICS;
    _pic=NULL;
  }
  
}
X264Encoder::~X264Encoder()
{
  stopEncoder(); 
}
//*******************************
uint8_t         X264Encoder::getResult( void *ress)
{
  return 1;         
}

//************************************
uint8_t         X264Encoder::encode( ADMImage        *in,
                                    uint8_t         *out,
                                    uint32_t        *len,
                                    uint32_t        *flags)
{
  x264_nal_t *nal;
  int nbNal=0;
  int sizemax=0;
  
        if(flags) *flags=0;
        
        memset(_pic,0,sizeof(x264_picture_t));
        PICS->img.i_csp=X264_CSP_I420;
        PICS->img.i_plane=3;
        
        PICS->img.plane[0]=in->data;             // Y
        PICS->img.plane[2]=in->data+_w*_h;       // u
        PICS->img.plane[1]=in->data+((_w*_h*5)>>2); // v
        PICS->img.i_stride[0]=_w;
        PICS->img.i_stride[1]=_w>>1;
        PICS->img.i_stride[2]=_w>>1;
        
        PICS->i_type=X264_TYPE_AUTO;
        
        if(x264_encoder_encode(HANDLE, &nal, &nbNal, PICS,NULL) < 0) 
        {
          printf("Error encoding with x264\n");
          return 0; 
        }

        // Write
        uint32_t size=0;
        for(uint32_t i=0;i<nbNal;i++)
        {
          sizemax=0xfffffff;;
          size+= x264_nal_encode(out + size, &sizemax, 1, &nal[i]);          
        }
        *len=size;
        if(flags)
        {
                if(PICS->i_type==X264_TYPE_IDR) *flags=AVI_KEY_FRAME;
        }
        return 1;
      
        
        
  
}

//*******************CQ*****************
uint8_t         X264EncoderCQ::init( uint32_t val,uint32_t fps1000,ADM_x264Param *zparam )
{
  memset(&param,0,sizeof(param));
  x264_param_default( &param );

  param.rc.i_qp_constant=val;           // should be ~ the same as CQ mode (?)
  return preamble(fps1000,zparam); 
}
X264EncoderCQ::~X264EncoderCQ()
{
  stopEncoder(); 
}
//*********************CBR***************
uint8_t         X264EncoderCBR::init( uint32_t val,uint32_t fps1000,ADM_x264Param *zparam )
{
  memset(&param,0,sizeof(param));
  x264_param_default( &param );
  param.rc.b_cbr=1;
  param.rc.i_bitrate=val;  
  return preamble(fps1000,zparam); 
}
X264EncoderCBR::~X264EncoderCBR()
{
  stopEncoder(); 
}
//********
#endif
