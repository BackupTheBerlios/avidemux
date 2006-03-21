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
#include "ADM_toolkit/ADM_cpuCap.h"
#include <ADM_assert.h>

#define HANDLE ((x264_t *)_handle)
#define PICS ((x264_picture_t *)_pic)


typedef struct avcc		// for avcc atom
{
  uint8_t *sps;
  int sps_length;
  uint8_t *pps;
  int pps_length;
} avcc;


// Yes, ugly FIXME
static x264_param_t param;

/*
uint32_t X264Encoder::getPTS_FrameNum(void)
{
    if( param.i_bframe) return ptsFrame+2;
    return ptsFrame;
}
*/
//********************************************* 
// Set the common stuff prior to codec opening
// ratecontrol stuff is left to the caller
// to initialize
//*********************************************
uint8_t X264Encoder::preamble (uint32_t fps1000, ADM_x264Param * zparam)
{

  x264_t *
    xhandle = NULL;

  printf ("Opening X264 for %lu x %lu\n", _w, _h);

  param.cpu = 0;
#define ADX_CHECK(x)  if(CpuCaps::has##x()) param.cpu|=X264_CPU_##x;
  ADX_CHECK (MMX);
  ADX_CHECK (SSE);
  ADX_CHECK (SSE2);
  ADX_CHECK (MMXEXT);
  ADX_CHECK (3DNOW);

  param.i_threads = zparam->nbThreads;
  param.i_width = _w;
  param.i_height = _h;
  param.i_csp = X264_CSP_I420;

  param.vui.i_sar_width = 1;	// FIXME
  param.vui.i_sar_height = 1;

  param.i_fps_num = fps1000;
  param.i_fps_den = 1000;
// ?

//        param.i_maxframes=0;
  if (zparam)
    {
        /*
      param.i_frame_reference = 1;	//  ref frame like mpeg1/2/4
      param.i_keyint_max = zparam->maxKf;	// Max intra distance
      param.i_keyint_min = zparam->minKf;	// Min intra distance
      param.i_bframe = zparam->nbBframe;	// 2 bframe
      param.b_cabac = zparam->cabac;	// Arith coding on/off
      if (zparam->globalHeader)
	param.b_repeat_headers = 0;	// Global header
        */
    }


  xhandle = x264_encoder_open (&param);
  if (!xhandle)
    return 0;

  _handle = (void *) xhandle;
  _pic = (void *) new
    x264_picture_t;
  printf ("X264 init ok (atom mode : %d)\n", zparam->globalHeader);
  if (param.i_threads > 1)
    printf ("X264 using %d threads\n", param.i_threads);
  if (zparam->globalHeader)
    return createHeader ();
  else
    return 1;


}
uint8_t X264Encoder::getExtraData (uint32_t * l, uint8_t ** d)
{
  *l = extraSize;
  *d = extraData;
  return 1;
}
// should never be called (pure)
//*******************************
uint8_t
  X264Encoder::init (uint32_t val, uint32_t fps1000, ADM_x264Param * param)
{
  ADM_assert (0);
}
//*******************************
uint8_t X264Encoder::stopEncoder (void)
{
  if (_handle)
    {
      x264_encoder_close (HANDLE);
      _handle = NULL;
    }
  if (_pic)
    {
      // picture_clean ?
      delete
	PICS;
      _pic = NULL;
    }
  if (extraData)
    {
      delete[]extraData;
      extraData = NULL;
    }
}
X264Encoder::~X264Encoder ()
{
  stopEncoder ();
}
//*******************************
//************************************
uint8_t X264Encoder::encode (ADMImage * in, ADMBitstream * out)
{
  x264_nal_t *
    nal;
  int
    nbNal = 0;
  int
    sizemax = 0;
  x264_picture_t
    pic_out;

  out->flags = 0;

  memset (_pic, 0, sizeof (x264_picture_t));
  PICS->img.i_csp = X264_CSP_I420;
  PICS->img.i_plane = 3;

  PICS->img.plane[0] = in->data;	// Y
  PICS->img.plane[2] = in->data + _w * _h;	// u
  PICS->img.plane[1] = in->data + ((_w * _h * 5) >> 2);	// v
  PICS->img.i_stride[0] = _w;
  PICS->img.i_stride[1] = _w >> 1;
  PICS->img.i_stride[2] = _w >> 1;

  PICS->i_type = X264_TYPE_AUTO;
  PICS->i_pts = curFrame++;
  if (x264_encoder_encode (HANDLE, &nal, &nbNal, PICS, &pic_out) < 0)
    {
      printf ("Error encoding with x264\n");
      return 0;
    }

  // Write

  uint32_t
    size = 0, thisnal = 0;
  uint8_t *
    dout = out->data;
  for (uint32_t i = 0; i < nbNal; i++)
    {
      sizemax = 0xfffffff;;
      if (!param.b_repeat_headers)
	size += 4;
      thisnal =
	x264_nal_encode (dout + size, &sizemax, param.b_repeat_headers,
			 &nal[i]);
      if (!param.b_repeat_headers)
	{			// Need to put size (assuming nal_size=4)
	  dout[size + 0 - 4] = (thisnal >> 24) & 0xff;
	  dout[size + 1 - 4] = (thisnal >> 16) & 0xff;
	  dout[size + 2 - 4] = (thisnal >> 8) & 0xff;
	  dout[size + 3 - 4] = (thisnal >> 0) & 0xff;

	}
      size += thisnal;
    }


  out->len = size;
  if(param.i_bframe)
    out->ptsFrame = (uint32_t) pic_out.i_pts+2;	// In fact it is the picture number in out case
  else
    out->ptsFrame = (uint32_t) pic_out.i_pts;	// In fact it is the picture number in out case
  //printf("Frame :%lld \n",pic_out.i_pts);
  switch (pic_out.i_type)
    {
    case X264_TYPE_IDR:
      out->flags = AVI_KEY_FRAME;
      break;
    case X264_TYPE_I:
      out->flags = AVI_P_FRAME;

      break;
    case X264_TYPE_P:
      out->flags = AVI_P_FRAME;
      break;
    case X264_TYPE_B:
    case X264_TYPE_BREF:
      out->flags = AVI_B_FRAME;
      break;
    default:
      printf ("X264 :Unknown image type:%d\n", pic_out.i_type);
      //ADM_assert(0);

    }
  out->out_quantizer = pic_out.i_qpplus1;
  return 1;
}
//*******************CQ*****************
uint8_t
  X264EncoderCQ::init (uint32_t val, uint32_t fps1000, ADM_x264Param * zparam)
{
  printf ("X264 CQ\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);

//  param.rc.i_rc_buffer_size=-1;
  param.rc.i_qp_constant = val;
  // should be ~ the same as CQ mode (?)
  return preamble (fps1000, zparam);
}
X264EncoderCQ::~X264EncoderCQ ()
{
  stopEncoder ();
}
//*********************CBR***************
uint8_t
  X264EncoderCBR::init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * zparam)
{
  printf ("X264 pass CBR\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);
  param.rc.b_cbr = 1;
  param.rc.i_bitrate = val / 1000;
//  param.rc.i_rc_buffer_size=val/1000;
//  param.rc.i_rc_init_buffer=(val/1000)>>1;
  return preamble (fps1000, zparam);
}
X264EncoderCBR::~X264EncoderCBR ()
{
  stopEncoder ();
}
//*********************Pass1***************
uint8_t
  X264EncoderPass1::init (uint32_t val, uint32_t fps1000,
			  ADM_x264Param * zparam)
{
  printf ("X264 pass 1\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);

//  param.rc.i_rc_buffer_size=-1;
  param.rc.i_qp_constant = 2;

  param.rc.b_stat_write = 1;
  param.rc.b_stat_read = 0;
  if (!zparam->logfile)
    param.rc.psz_stat_out = "/tmp/x264_log.tmp";
  else
    param.rc.psz_stat_out = zparam->logfile;
  printf ("x264 codec using %s as statfile\n", param.rc.psz_stat_out);
  return preamble (fps1000, zparam);
}
X264EncoderPass1::~X264EncoderPass1 ()
{
  stopEncoder ();
}
//*********************Pass1***************
uint8_t
  X264EncoderPass2::init (uint32_t val, uint32_t fps1000,
			  ADM_x264Param * zparam)
{
  printf ("X264 pass 2\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);

  param.rc.b_cbr = 1;
  param.rc.i_bitrate = val;
  //param.rc.i_rc_buffer_size=val;
  //param.rc.i_rc_init_buffer=val>>1;

  param.rc.b_stat_write = 0;
  param.rc.b_stat_read = 1;


  if (!zparam->logfile)
    param.rc.psz_stat_in = "/tmp/x264_log.tmp";
  else
    param.rc.psz_stat_in = zparam->logfile;
  printf ("x264 codec using %s as statfile\n", param.rc.psz_stat_in);

  return preamble (fps1000, zparam);
}
X264EncoderPass2::~X264EncoderPass2 ()
{
  stopEncoder ();
}
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
#define MAX_HEADER_X264 1024	// should be enough
/*_____________________________________________________________

        Try to create the avcC atom and set it as extraData
_______________________________________________________________*/

#define H264_NAL_TYPE_SEQ_PARAM 0x7
#define H264_NAL_TYPE_PIC_PARAM 0x8

uint8_t X264Encoder::createHeader (void)
{
  x264_nal_t *
    nal;
  int
    nal_count;
  uint32_t
    offset = 0;;
  uint8_t
    buffer[MAX_HEADER_X264];
  uint8_t
    picParam[MAX_HEADER_X264];
  uint8_t
    seqParam[MAX_HEADER_X264];
  int
    picParamLen = 0, seqParamLen = 0, len;
  int
    sz;

  // 1024 bytes should be enough
  extraData = new uint8_t[MAX_HEADER_X264];
  extraSize = 0;


  if (x264_encoder_headers (HANDLE, &nal, &nal_count))
    {
      printf ("X264 : Cannot create header\n");
      return 0;
    }
  printf ("Nb nal :%d\n", nal_count);

  // Now encode them
  for (int i = 0; i < nal_count; i++)
    {
      if (nal[i].i_type == H264_NAL_TYPE_SEQ_PARAM)
	{
	  sz = x264_nal_encode (seqParam, &seqParamLen, 0, &nal[i]);
	}
      else if (nal[i].i_type == H264_NAL_TYPE_PIC_PARAM)
	{
	  sz = x264_nal_encode (picParam, &picParamLen, 0, &nal[i]);
	}
      else
	{
	  printf ("?? type :%d in nal %d\n", nal[i].i_type, i);
	  sz = x264_nal_encode (buffer, &len, 0, &nal[i]);
	}
      if (sz <= 0)
	{
	  printf ("X264 : Cannot encode nal  header %d\n", i);
	  return 0;
	}
    }
  // Now that we got all the nals encoded, time to build the avcC atom
  // Check we have everything we want
  if (!picParamLen || !seqParamLen)
    {
      printf ("X264 : Seqparam or PicParam not found\n");
      return 0;
    }



  // Fill header
  extraData[0] = 1;		// Version
  extraData[1] = seqParam[1];	//0x42; // AVCProfileIndication
  extraData[2] = seqParam[2];	//0x00; // profile_compatibility
  extraData[3] = seqParam[3];	//0x0D; // AVCLevelIndication

  extraData[4] = 0xFC + 3;	// lengthSizeMinusOne 
  extraData[5] = 0xE0 + 1;	// nonReferenceDegredationPriorityLow        

  offset = 6;



  extraData[offset] = seqParamLen >> 8;
  extraData[offset + 1] = seqParamLen & 0xff;

  offset += 2;

  memcpy (extraData + offset, seqParam, seqParamLen);
  offset += seqParamLen;

  extraData[offset] = 1;	// numOfPictureParameterSets
  offset++;

  extraData[offset] = (picParamLen) >> 8;
  extraData[offset + 1] = (picParamLen) & 0xff;

  offset += 2;


  memcpy (extraData + offset, picParam, picParamLen);
  offset += picParamLen;


  extraSize = offset;

  ADM_assert (offset < MAX_HEADER_X264);
  return 1;
//********
}
#endif
