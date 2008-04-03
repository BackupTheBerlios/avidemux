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

#ifdef USE_X264

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_default.h"
#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_x264.h"
#include "ADM_assert.h"
#include "avi_vars.h"

#define HANDLE ((x264_t *) _handle)
#define PICS ((x264_picture_t *) _pic)

typedef struct avcc		// for avcc atom
{
	uint8_t *sps;
	int sps_length;
	uint8_t *pps;
	int pps_length;
} avcc;

// Yes, ugly FIXME
static x264_param_t param;
static ADM_x264Param admParam;

void X264Encoder::printParam(x264_param_t *x264Param)
{
	printf("[x264] b_repeat_headers = %d\n", x264Param->b_repeat_headers);
	printf("[x264] i_log_level = %d\n", x264Param->i_log_level);
	printf("[x264] i_threads = %d", x264Param->i_threads);

	if (x264Param->i_threads == 0)
		printf (" (auto)");
	if (x264Param->i_threads == 1)
		printf (" (disabled)");

	printf("\n[x264] i_width = %d, i_height = %d\n", x264Param->i_width, x264Param->i_height);
	printf("[x264] i_csp = %d\n", x264Param->i_csp);	
	printf("[x264] i_fps_num = %d, i_fps_den = %d\n", x264Param->i_fps_num, x264Param->i_fps_den);
	printf("[x264] rc.i_rc_method = %d\n", x264Param->rc.i_rc_method);
	printf("[x264] rc.i_bitrate = %d\n", x264Param->rc.i_bitrate);
	printf("[x264] rc.f_rf_constant = %f\n", x264Param->rc.f_rf_constant);
	printf("[x264] rc.i_qp_constant = %d\n", x264Param->rc.i_qp_constant);	
	printf("[x264] analyse.i_subpel_refine = %d\n", x264Param->analyse.i_subpel_refine);
	printf("[x264] analyse.b_bframe_rdo = %d\n", x264Param->analyse.b_bframe_rdo);
	printf("[x264] analyse.i_me_method = %d\n", x264Param->analyse.i_me_method);
	printf("[x264] analyse.i_me_range = %d\n", x264Param->analyse.i_me_range);
	printf("[x264] analyse.i_mv_range = %d\n", x264Param->analyse.i_mv_range);
	printf("[x264] analyse.i_mv_range_thread = %d\n", x264Param->analyse.i_mv_range_thread);
	printf("[x264] analyse.i_direct_mv_pred = %d\n", x264Param->analyse.i_direct_mv_pred);
	printf("[x264] analyse.i_direct_8x8_inference = %d\n", x264Param->analyse.i_direct_8x8_inference);
	printf("[x264] analyse.b_weighted_bipred = %d\n", x264Param->analyse.b_weighted_bipred);
	printf("[x264] analyse.b_transform_8x8 = %d\n", x264Param->analyse.b_transform_8x8);
	printf("[x264] analyse.inter = %d\n", x264Param->analyse.inter);
	printf("[x264] b_cabac = %d\n", x264Param->b_cabac);
	printf("[x264] b_interlaced = %d\n", x264Param->b_interlaced);
	printf("[x264] b_deblocking_filter = %d\n", x264Param->b_deblocking_filter);
	printf("[x264] i_deblocking_filter_alphac0 = %d\n", x264Param->i_deblocking_filter_alphac0);
	printf("[x264] i_deblocking_filter_beta = %d\n", x264Param->i_deblocking_filter_beta);
	printf("[x264] i_bframe = %d\n", x264Param->i_bframe);
	printf("[x264] i_bframe_bias = %d\n", x264Param->i_bframe_bias);
	printf("[x264] i_frame_reference = %d\n", x264Param->i_frame_reference);
	printf("[x264] b_bframe_pyramid = %d\n", x264Param->b_bframe_pyramid);
	printf("[x264] b_bframe_adaptive = %d\n", x264Param->b_bframe_adaptive);
	printf("[x264] i_keyint_max = %d\n", x264Param->i_keyint_max);
	printf("[x264] i_keyint_min = %d\n", x264Param->i_keyint_min);
	printf("[x264] i_scenecut_threshold = %d\n", x264Param->i_scenecut_threshold);
	printf("[x264] b_pre_scenecut = %d\n", x264Param->b_pre_scenecut);
	printf("[x264] analyse.b_mixed_references = %d\n", x264Param->analyse.b_mixed_references);
	printf("[x264] analyse.b_chroma_me = %d\n", x264Param->analyse.b_chroma_me);
	printf("[x264] analyse.b_bidir_me = %d\n", x264Param->analyse.b_bidir_me);
	printf("[x264] analyse.i_trellis = %d\n", x264Param->analyse.i_trellis);
	printf("[x264] analyse.b_fast_pskip = %d\n", x264Param->analyse.b_fast_pskip);
	printf("[x264] analyse.b_dct_decimate = %d\n", x264Param->analyse.b_dct_decimate);
	printf("[x264] analyse.i_noise_reduction = %d\n", x264Param->analyse.i_noise_reduction);
	printf("[x264] analyse.i_luma_deadzone[0] = %d\n", x264Param->analyse.i_luma_deadzone[0]);
	printf("[x264] analyse.i_luma_deadzone[1] = %d\n", x264Param->analyse.i_luma_deadzone[1]);
	printf("[x264] i_cqm_preset = %d\n", x264Param->i_cqm_preset);

	printf("[x264] cqm_4iy = ");
	printCqm(x264Param->cqm_4iy, sizeof(x264Param->cqm_4iy));
	printf("\n[x264] cqm_4ic = ");
	printCqm(x264Param->cqm_4ic, sizeof(x264Param->cqm_4ic));
	printf("\n[x264] cqm_4py = ");
	printCqm(x264Param->cqm_4py, sizeof(x264Param->cqm_4py));
	printf("\n[x264] cqm_4pc = ");
	printCqm(x264Param->cqm_4pc, sizeof(x264Param->cqm_4pc));
	printf("\n[x264] cqm_8iy = ");
	printCqm(x264Param->cqm_8iy, sizeof(x264Param->cqm_8iy));
	printf("\n[x264] cqm_8py = ");
	printCqm(x264Param->cqm_8py, sizeof(x264Param->cqm_8py));

	printf("\n[x264] rc.i_qp_min = %d\n", x264Param->rc.i_qp_min);
	printf("[x264] rc.i_qp_max = %d\n", x264Param->rc.i_qp_max);
	printf("[x264] rc.i_qp_step = %d\n", x264Param->rc.i_qp_step);
	printf("[x264] rc.f_rate_tolerance = %f\n", x264Param->rc.f_rate_tolerance);
	printf("[x264] rc.f_ip_factor = %f\n", x264Param->rc.f_ip_factor);
	printf("[x264] rc.f_pb_factor = %f\n", x264Param->rc.f_pb_factor);
	printf("[x264] analyse.i_chroma_qp_offset = %d\n", x264Param->analyse.i_chroma_qp_offset);
	printf("[x264] rc.f_qcompress = %f\n", x264Param->rc.f_qcompress);
	printf("[x264] rc.f_complexity_blur = %f\n", x264Param->rc.f_complexity_blur);
	printf("[x264] rc.f_qblur = %f\n", x264Param->rc.f_qblur);
	printf("[x264] rc.i_vbv_max_bitrate = %d\n", x264Param->rc.i_vbv_max_bitrate);
	printf("[x264] rc.i_vbv_buffer_size = %d\n", x264Param->rc.i_vbv_buffer_size);
	printf("[x264] rc.f_vbv_buffer_init = %f\n", x264Param->rc.f_vbv_buffer_init);

	printf("[x264] i_level_idc = %d\n", x264Param->i_level_idc);
	printf("[x264] i_sps_id = %d\n", x264Param->i_sps_id);
	printf("[x264] b_deterministic = %d\n", x264Param->b_deterministic);
	printf("[x264] b_aud = %d\n", x264Param->b_aud);
	printf("[x264] analyse.b_psnr = %d\n", x264Param->analyse.b_psnr);
	printf("[x264] analyse.b_ssim = %d\n", x264Param->analyse.b_ssim);
	printf("[x264] vui.i_overscan = %d\n", x264Param->vui.i_overscan);
	printf("[x264] vui.i_vidformat = %d\n", x264Param->vui.i_vidformat);
	printf("[x264] vui.i_colorprim = %d\n", x264Param->vui.i_colorprim);
	printf("[x264] vui.i_transfer = %d\n", x264Param->vui.i_transfer);
	printf("[x264] vui.i_colmatrix = %d\n", x264Param->vui.i_colmatrix);
	printf("[x264] vui.i_chroma_loc = %d\n", x264Param->vui.i_chroma_loc);
	printf("[x264] vui.b_fullrange = %d\n", x264Param->vui.b_fullrange);
}

void X264Encoder::printCqm(const uint8_t cqm[], uint8_t size)
{
	for (int index = 0; index < size; index++)
		printf("%d ", cqm[index]);
}

//**********************************************************
// Do the translation avidemux parameters->x264 parameters
//**********************************************************
uint8_t X264Encoder::preamble(uint32_t fps1000, ADM_x264Param *zparam)
{
	x264_t *xhandle = NULL;

	if (zparam->globalHeader)
		param.b_repeat_headers = 0;
	else
		param.b_repeat_headers = 1;

	param.i_log_level = X264_LOG_INFO;
	param.i_threads = zparam->nbThreads;
	param.i_width = _w;
	param.i_height = _h;
	param.i_csp = X264_CSP_I420;
	param.i_fps_num = fps1000;
	param.i_fps_den = 1000;

	if (zparam->AR_AsInput)
	{
		param.vui.i_sar_width = video_body->getPARWidth();
		param.vui.i_sar_height = video_body->getPARHeight();
	}
	else
	{
		param.vui.i_sar_width = zparam->AR_Num;
		param.vui.i_sar_height = zparam->AR_Den;
	}

	param.analyse.i_subpel_refine = zparam->PartitionDecision;
	param.analyse.b_bframe_rdo = zparam->RDO;
	param.analyse.i_me_method = zparam->Method;
	param.analyse.i_me_range = zparam->Range;
	param.analyse.i_mv_range = zparam->mv_range_thread;
	param.analyse.i_mv_range_thread = zparam->mv_range_thread;
	param.analyse.i_direct_mv_pred = zparam->DirectMode;
	param.analyse.i_direct_8x8_inference = zparam->direct_8x8_inference;
	param.analyse.b_weighted_bipred = zparam->Weighted;
	param.analyse.b_transform_8x8 = zparam->_8x8;
	param.analyse.inter = 0;

	if (zparam->_8x8P)
		param.analyse.inter |= X264_ANALYSE_PSUB16x16;

	if (zparam->_8x8B)
		param.analyse.inter |= X264_ANALYSE_BSUB16x16;

	if (zparam->_4x4)
		param.analyse.inter |= X264_ANALYSE_PSUB8x8;

	if (zparam->_8x8I)
		param.analyse.inter |= X264_ANALYSE_I8x8;

	if (zparam->_4x4I)
		param.analyse.inter |= X264_ANALYSE_I4x4;

	param.b_cabac = zparam->CABAC;
	param.b_interlaced = zparam->interlaced;
	param.b_deblocking_filter = zparam->DeblockingFilter;
	param.i_deblocking_filter_alphac0 = zparam->Strength;
	param.i_deblocking_filter_beta = zparam->Threshold;
	param.i_bframe = zparam->MaxBFrame;
	param.i_bframe_bias = zparam->Bias;
	param.i_frame_reference = zparam->MaxRefFrames;
	param.b_bframe_pyramid = zparam->BasReference;
	param.b_bframe_adaptive = zparam->Adaptative;
	param.i_keyint_max = zparam->MaxIdr;
	param.i_keyint_min = zparam->MinIdr;
	param.i_scenecut_threshold = zparam->SceneCut;
	param.b_pre_scenecut = zparam->pre_scenecut;
	param.analyse.b_mixed_references = zparam->MixedRefs;
	param.analyse.b_chroma_me = zparam->ChromaME;
	param.analyse.b_bidir_me = zparam->BidirME;
	param.analyse.i_trellis = zparam->Trellis;
	param.analyse.b_fast_pskip = zparam->fastPSkip;
	param.analyse.b_dct_decimate = zparam->DCTDecimate;
	param.analyse.i_noise_reduction = zparam->NoiseReduction;
	param.analyse.i_luma_deadzone[0] = zparam->interLumaDeadzone;
	param.analyse.i_luma_deadzone[1] = zparam->intraLumaDeadzone;
	param.i_cqm_preset = zparam->cqmPreset;

	memcpy(param.cqm_4iy, zparam->intra4x4Luma, sizeof(param.cqm_4iy));
	memcpy(param.cqm_4ic, zparam->intraChroma, sizeof(param.cqm_4ic));
	memcpy(param.cqm_4py, zparam->inter4x4Luma, sizeof(param.cqm_4py));
	memcpy(param.cqm_4pc, zparam->interChroma, sizeof(param.cqm_4pc));
	memcpy(param.cqm_8iy, zparam->intra8x8Luma, sizeof(param.cqm_8iy));
	memcpy(param.cqm_8py, zparam->inter8x8Luma, sizeof(param.cqm_8py));

	param.rc.i_qp_min = zparam->MinQp;
	param.rc.i_qp_max = zparam->MaxQp;
	param.rc.i_qp_step = zparam->QpStep;
	param.rc.f_rate_tolerance = (float)zparam->rateTolerance / 100;
	param.rc.f_ip_factor = zparam->quantiserIpRatio;
	param.rc.f_pb_factor = zparam->quantiserPbRatio;
	param.analyse.i_chroma_qp_offset = zparam->chromaQuantiserOffset;
	param.rc.f_qcompress = (float)zparam->BitrateVariability / 100;
	param.rc.f_complexity_blur = zparam->quantiserComplexityBlur;
	param.rc.f_qblur = zparam->quantiserBlur;
	param.rc.i_vbv_max_bitrate = zparam->vbv_max_bitrate;
	param.rc.i_vbv_buffer_size = zparam->vbv_buffer_size;
	param.rc.f_vbv_buffer_init = (float)zparam->vbv_buffer_init / 100;

	if (zparam->idc)	// Used to default to zero so still need to cater for it
		param.i_level_idc = zparam->idc;

	param.i_sps_id = zparam->spsId;
	param.b_deterministic = zparam->deterministic;
	param.b_aud = zparam->accessUnitDelimiters;
	param.analyse.b_psnr = zparam->psnrComputation;
	param.analyse.b_ssim = zparam->ssimComputation;

	param.vui.i_overscan = zparam->overscan;
	param.vui.i_vidformat = zparam->videoFormat;
	param.vui.i_colorprim = zparam->colourPrimaries;
	param.vui.i_transfer = zparam->transferCharacteristics;
	param.vui.i_colmatrix = zparam->colourMatrix;
	param.vui.i_chroma_loc = zparam->chromaSampleLocation;
	param.vui.b_fullrange = zparam->fullRangeSamples;

	printParam(&param);

	xhandle = x264_encoder_open(&param);

	if (!xhandle)
		return 0;

	_handle = (void*)xhandle;
	_pic = (void*)new x264_picture_t;

	printf("[x264] init ok (atom mode: %d)\n", zparam->globalHeader);

	if (zparam->globalHeader)
		return createHeader();
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
  if(_seiUserData)
  {
	  delete [] _seiUserData;
	  _seiUserData=NULL;
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
  x264_nal_t *    nal;
  int    nbNal = 0;
  int    sizemax = 0;
  x264_picture_t    pic_out;

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
      printf ("[x264] Error encoding\n");
      return 0;
    }

  // Write

  uint32_t    size = 0, thisnal = 0;
  uint8_t *    dout = out->data;
  for (uint32_t i = 0; i < nbNal; i++)
    {
      sizemax = 0xfffffff;;
      if (!param.b_repeat_headers)
    	  size += 4;
      thisnal =	x264_nal_encode (dout + size, &sizemax, param.b_repeat_headers, &nal[i]);
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
  out->ptsFrame = (uint32_t) pic_out.i_pts;	// In fact it is the picture number in out case
// Delay for b-pyramide
    
    if(admParam.BasReference)
    {
    //  printf("%u +=%u\n",out->ptsFrame,admParam.MaxBFrame);
      out->ptsFrame+=admParam.MaxBFrame;
    }
  //printf("Frame :%lld \n",pic_out.i_pts);
  switch (pic_out.i_type)
    {
    case X264_TYPE_IDR:
      out->flags = AVI_KEY_FRAME;
      /* First frame ?*/
      if(!param.b_repeat_headers && _seiUserData && !pic_out.i_pts)
      {
    	  // Put our SEI front...
    	  // first a temp location...
    	  uint8_t tmpBuffer[size];
    	  memcpy(tmpBuffer,out->data,size);
    	  // Put back out SEI and add Size
    	  dout[0]=(_seiUserDataLen>>24)&0xff;
    	  dout[1]=(_seiUserDataLen>>16)&0xff;
    	  dout[2]=(_seiUserDataLen>>8)&0xff;
    	  dout[3]=(_seiUserDataLen>>0)&0xff;
    	  memcpy(dout+4,_seiUserData,_seiUserDataLen);
    	  memcpy(dout+4+_seiUserDataLen,tmpBuffer,size);
    	  size+=4+_seiUserDataLen;
    	  out->len = size; // update total size
      }
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
      printf ("[x264] Unknown image type: %d\n", pic_out.i_type);
      //ADM_assert(0);
    }
    
    //
  out->out_quantizer = pic_out.i_qpplus1;
  return 1;
}
//*******************CQ*****************
uint8_t
  X264EncoderCQ::init (uint32_t val, uint32_t fps1000, ADM_x264Param * zparam)
{
  printf ("[x264] CQ\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);
  memcpy(&admParam,zparam,sizeof(admParam));
#if X264_BUILD >=48
  param.rc.i_rc_method=X264_RC_CQP;
#endif

//  param.rc.i_rc_buffer_size=-1;
  param.rc.i_qp_constant = val;
  // should be ~ the same as CQ mode (?)
  return preamble (fps1000, &admParam);
}
X264EncoderCQ::~X264EncoderCQ ()
{
  stopEncoder ();
}
//*******************AQ*****************
uint8_t
  X264EncoderAQ::init (uint32_t val, uint32_t fps1000, ADM_x264Param * zparam)
{
  printf ("[x264] AQ\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);
  memcpy(&admParam,zparam,sizeof(admParam));

//  param.rc.i_rc_buffer_size=-1;
#if X264_BUILD >=48
  param.rc.i_rc_method=X264_RC_CRF;
#endif
#if X264_BUILD <54
  param.rc.i_rf_constant = val;
#else
  param.rc.f_rf_constant = val;
#endif
  // should be ~ the same as CQ mode (?)
  return preamble (fps1000, &admParam);
}
X264EncoderAQ::~X264EncoderAQ ()
{
  stopEncoder ();
}
//*********************CBR***************
uint8_t
  X264EncoderCBR::init (uint32_t val, uint32_t fps1000,
			ADM_x264Param * zparam)
{
  printf ("[x264] pass CBR\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);
  memcpy(&admParam,zparam,sizeof(admParam));
#if X264_BUILD >=48
  param.rc.i_rc_method=X264_RC_ABR;
#else
  param.rc.b_cbr = 1;
#endif
  param.rc.i_bitrate = val ;
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
  printf ("[x264] pass 1\n");
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);
  memcpy(&admParam,zparam,sizeof(admParam));

  // in case of pass 1 we switch off some options
  // As mplayer does it
  admParam.PartitionDecision = 1;
  admParam.Method = X264_ME_DIA;
  
  admParam._8x8 = 0;
  admParam._8x8P = 0;
  admParam._8x8B = 0;
  admParam._8x8I = 0;
  admParam._4x4I = 0;
  admParam.Weighted = 0;
  admParam.Trellis = 0;
  
//  param.rc.i_rc_buffer_size=-1;
#if X264_BUILD >=48
  param.rc.i_rc_method=X264_RC_CQP;
#endif
  param.rc.i_qp_constant = 2;

  param.rc.b_stat_write = 1;
  param.rc.b_stat_read = 0;
  if (!zparam->logfile)
    param.rc.psz_stat_out = "/tmp/x264_log.tmp";
  else
    param.rc.psz_stat_out = zparam->logfile;
  printf ("[x264] codec using %s as statfile\n", param.rc.psz_stat_out);
  return preamble (fps1000, &admParam);
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
    val=val/1000; // bps->kdps
  printf ("[x264] pass 2, using bitrate of %u\n",val);
  memset (&param, 0, sizeof (param));
  x264_param_default (&param);
  
  memcpy(&admParam,zparam,sizeof(admParam));
#if X264_BUILD >=48
  param.rc.i_rc_method=X264_RC_ABR;
#else
  param.rc.b_cbr = 1;
#endif

  param.rc.i_bitrate = val;
  //param.rc.i_rc_buffer_size=val;
  //param.rc.i_rc_init_buffer=val>>1;

  param.rc.b_stat_write = 0;
  param.rc.b_stat_read = 1;


  if (!zparam->logfile)
    param.rc.psz_stat_in = "/tmp/x264_log.tmp";
  else
    param.rc.psz_stat_in = zparam->logfile;
  printf ("[x264] using %s as statfile\n", param.rc.psz_stat_in);

  return preamble (fps1000, &admParam);
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
#define H264_NAL_TYPE_SEI       0x6
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
  uint8_t    buffer[MAX_HEADER_X264];
  uint8_t    picParam[MAX_HEADER_X264];
  uint8_t    seqParam[MAX_HEADER_X264];
  uint8_t    sei[MAX_HEADER_X264];
  int	     picParamLen = 0, seqParamLen = 0,seiParamLen=0, len;
  int
    sz;

  // 1024 bytes should be enough
  extraData = new uint8_t[MAX_HEADER_X264];
  extraSize = 0;

  x264_t *x264=(x264_t *)_handle;
  //x264->i_frame=0; // force write_sei_params
  if (x264_encoder_headers (x264, &nal, &nal_count))
    {
      printf ("[x264] Cannot create header\n");
      return 0;
    }
  printf ("[x264] Nb nal: %d\n", nal_count);

  // Now encode them
  for (int i = 0; i < nal_count; i++)
    {
	  switch(nal[i].i_type)
	  {
	  case H264_NAL_TYPE_SEQ_PARAM:
		  	sz = x264_nal_encode (seqParam, &seqParamLen, 0, &nal[i]);
		  	break;
	  case H264_NAL_TYPE_PIC_PARAM:
		  	sz = x264_nal_encode (picParam, &picParamLen, 0, &nal[i]);
		  	break;
	  case H264_NAL_TYPE_SEI:
		  	sz = x264_nal_encode (sei, &seiParamLen, 0, &nal[i]);
		  	break;
	  default:
		  printf ("[x264] ?? type %d in nal %d\n", nal[i].i_type, i);
		  sz = x264_nal_encode (buffer, &len, 0, &nal[i]);
	 }
    if (sz <= 0)
	{
	  printf ("[x264] Cannot encode nal header %d\n", i);
	  return 0;
	}
    }
  // Now that we got all the nals encoded, time to build the avcC atom
  // Check we have everything we want
  if (!picParamLen || !seqParamLen)
    {
      printf ("[x264] Seqparam or PicParam not found\n");
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

  // Where x264 stores all its header, save it for later use
  if(seiParamLen) 
  {
	  	_seiUserDataLen=seiParamLen;
	  	_seiUserData=new uint8_t[_seiUserDataLen];
	  	memcpy(_seiUserData,sei,_seiUserDataLen);
  }

  extraSize = offset;

  ADM_assert (offset < MAX_HEADER_X264);
  return 1;
//********
}
#endif
