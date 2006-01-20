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


typedef struct avcc // for avcc atom
{
            uint8_t * sps;
            int       sps_length;
            uint8_t * pps;
            int       pps_length;
} avcc;


// Yes, ugly FIXME
static  x264_param_t          param;
static  myENC_RESULT          x_res;
//********************************************* 
// Set the common stuff prior to codec opening
// ratecontrol stuff is left to the caller
// to initialize
//*********************************************
uint8_t   X264Encoder::preamble(uint32_t fps1000,ADM_x264Param *zparam)
{      
  
  x264_t                *xhandle=NULL;
 
        printf("Opening X264 for %lu x %lu\n",_w,_h);
  
        param.cpu=0; // Will be slow ...
        param.i_width=_w;
        param.i_height=_h;
        param.i_csp=X264_CSP_I420;
        
        param.vui.i_sar_width=_w;       // FIXME
        param.vui.i_sar_height=_h;
        
        param.i_fps_num=fps1000;
        param.i_fps_den=1000;
//        param.i_maxframes=0;
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

        // Create avcc atom as extradata

/*
        UINT8  configurationVersion = 1;
        UINT8  AVCProfileIndication;
        UINT8  profile_compatibility;
        UINT8  AVCLevelIndication;
        bit(6) reserved = 111111b;
        unsigned int(2) lengthSizeMinusOne;
        bit(3) reserved = 111b;
        unsigned int(5) numOfSequenceParameterSets;
        for (i=0; i< numOfSequenceParameterSets;  i++)
       {       

                UINT16 sequenceParameterSetLength ;

                bit(8*sequenceParameterSetLength) sequenceParameterSetNALUnit;

        }
        UINT8  numOfPictureParameterSets;
        for (i=0; i< numOfPictureParameterSets;  i++)
       {
                UINT16 pictureParameterSetLength;
                bit(8*pictureParameterSetLength) pictureParameterSetNALUnit;
        }
*/
/*
           0  0x01,
           1  0x4D, // AVCProfileIndication
           2  0x40, // profile_compatibility
           3  0x1F, // AVCLevelIndication

           4  0xFF,0xE1, // lengthSizeMinusOne=3 (4) / NumSeq=1

           6  0x00,0x14, // sequenceParameterSetLength

                0x27,0x4D,0x40,0x1F,0xA9,0x18,0x0A,0x00,
                0x8B,0x60,0x0D,0x41,0x80,0x41,0x8C,0x2B,
                0x5E,0xF7,0xC0,0x40,

             0x01,               // numOfPictureParameterSets 
             0x00,0x04,          sequenceParameterSetLength
                0x28,0xCE,0x09,0xC8};

*/
        // 1024 bytes should be enough
#define MAX_HEADER_X264 1024 // should be enough
    extraData=new uint8_t[MAX_HEADER_X264];
        
x264_nal_t       * nal;
int                nal_count;
uint32_t           offset=0;;

        extraSize=0;
        x264_encoder_headers( HANDLE, &nal, &nal_count );
        printf("Nb nal :%d\n",nal_count);
    // Fill header
        extraData[0]=1; // Version
        extraData[1]=0x42; // AVCProfileIndication
        extraData[2]=0x00; // profile_compatibility
        extraData[3]=0x0D; // AVCLevelIndication

        extraData[4]=0xFF; // lengthSizeMinusOne 
        extraData[5]=0xE0+1; // nonReferenceDegredationPriorityLow        

        offset=6;

        

        extraData[offset]=(1+nal[1].i_payload)>>8;
        extraData[offset+1]=(1+nal[1].i_payload)&0xff;

        offset+=2;

        extraData[offset++]=0x67;

        memcpy(extraData+offset,nal[1].p_payload,nal[1].i_payload);

        offset+=nal[1].i_payload;

        extraData[offset]=1; // numOfPictureParameterSets
        offset++;

        extraData[offset]=(1+nal[2].i_payload)>>8;
        extraData[offset+1]=(1+nal[2].i_payload)&0xff;

        offset+=2;
        extraData[offset++]=0x68;

        memcpy(extraData+offset,nal[2].p_payload,nal[2].i_payload);

        offset+=nal[1].i_payload;
        extraSize=offset;

        ADM_assert(offset<MAX_HEADER_X264);
        return 1;
  
}
 uint8_t X264Encoder::getExtraData (uint32_t * l, uint8_t ** d)
{
        *l=extraSize;
        *d=extraData;
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
  if(extraData)
   {
        delete [] extraData;
        extraData=NULL;
  }
}
X264Encoder::~X264Encoder()
{
  stopEncoder(); 
}
//*******************************
//************************************
uint8_t         X264Encoder::encode( ADMImage        *in,
                                    uint8_t         *out,
                                    uint32_t        *len,
                                    uint32_t        *flags)
{
  x264_nal_t *nal;
  int nbNal=0;
  int sizemax=0;
  x264_picture_t pic_out;
  
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
        
        if(x264_encoder_encode(HANDLE, &nal, &nbNal, PICS,&pic_out) < 0) 
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
        x_res.is_key_frame=0;
        switch(pic_out.i_type)
        {
          case X264_TYPE_IDR:   //FIXME: Only works if nb ref frame==1
          case X264_TYPE_I:
                        x_res.is_key_frame=1;            
                        if(flags) *flags=AVI_KEY_FRAME;
                        break;
          case X264_TYPE_P:
                        if(flags) *flags=AVI_P_FRAME;
                        break;
          case X264_TYPE_B:
          case X264_TYPE_BREF:
                        if(flags) *flags=AVI_B_FRAME;
                        break;
          default:   
            printf("X264 :Unknown image type:%d\n",pic_out.i_type);
            //ADM_assert(0);
          
        }       
        x_res.out_quantizer=pic_out.i_qpplus1;        
        return 1;  
}
//**************************************
uint8_t         X264Encoder::getResult( void *ress)
{
  myENC_RESULT *res;

  res=(myENC_RESULT *)ress;
  memcpy(res,&x_res,sizeof(myENC_RESULT));
  return 1;

}
//*******************CQ*****************
uint8_t         X264EncoderCQ::init( uint32_t val,uint32_t fps1000,ADM_x264Param *zparam )
{
  memset(&param,0,sizeof(param));
  x264_param_default( &param );

//  param.rc.i_rc_buffer_size=-1;
  param.rc.i_qp_constant=val;  
           // should be ~ the same as CQ mode (?)
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
  param.rc.i_bitrate=val/1000;  
//  param.rc.i_rc_buffer_size=val/1000;
//  param.rc.i_rc_init_buffer=(val/1000)>>1;
  return preamble(fps1000,zparam); 
}
X264EncoderCBR::~X264EncoderCBR()
{
  stopEncoder(); 
}
//*********************Pass1***************
uint8_t         X264EncoderPass1::init( uint32_t val,uint32_t fps1000,ADM_x264Param *zparam )
{
  memset(&param,0,sizeof(param));
  x264_param_default( &param );
  
//  param.rc.i_rc_buffer_size=-1;
  param.rc.i_qp_constant=2;  
  
  param.rc.b_stat_write = 1;
  param.rc.b_stat_read = 0;
  param.rc.psz_stat_out = "/tmp/x264_log.tmp"; //FIXME
  return preamble(fps1000,zparam); 
}
X264EncoderPass1::~X264EncoderPass1()
{
  stopEncoder(); 
}
//*********************Pass1***************
uint8_t         X264EncoderPass2::init( uint32_t val,uint32_t fps1000,ADM_x264Param *zparam )
{
  memset(&param,0,sizeof(param));
  x264_param_default( &param );
  
  param.rc.b_cbr=1;
  param.rc.i_bitrate=val;  
  //param.rc.i_rc_buffer_size=val;
  //param.rc.i_rc_init_buffer=val>>1;
  
  param.rc.b_stat_write = 0;
  param.rc.b_stat_read = 1;
  
  
  param.rc.psz_stat_in = "/tmp/x264_log.tmp"; // FIXME
  return preamble(fps1000,zparam); 
}
X264EncoderPass2::~X264EncoderPass2()
{
  stopEncoder(); 
}

//********
#endif
