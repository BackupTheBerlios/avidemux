/***************************************************************************
                          ADM_ffmpeg.cpp  -  description
                             -------------------

	Encoder for ffmpeg
	Rever-enginereed from mplayer & transcode
		

    begin                : Tue Sep 10 2002
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
#include "ADM_lavcodec.h"

#include "avi_vars.h"
#include "prototype.h"


#include "ADM_colorspace/colorspace.h"
#include "ADM_codecs/ADM_divxEncode.h"

#include "ADM_toolkit/toolkit.hxx"
#include <ADM_assert.h>
#include "ADM_codecs/ADM_ffmpeg.h"
//#define TEST_NOB 1

static char LogName[500];


static myENC_RESULT res;
ffmpegEncoder::ffmpegEncoder (uint32_t width, uint32_t height, FF_CODEC_ID id):encoder (width,
	 height)
{
  printf ("\n Build : %d \n", LIBAVCODEC_BUILD);

  _id = id;
  _swap = 0;
  _context = NULL;
  _settingsPresence = 0;

  _context = avcodec_alloc_context ();

  ADM_assert (_context);
  memset (&_frame, 0, sizeof (_frame));
  _frame.pts = AV_NOPTS_VALUE;
  _context->width = _w;
  _context->height = _h;
  _frame.linesize[0] = _w;
  _frame.linesize[1] = _w >> 1;
  _frame.linesize[2] = _w >> 1;

};

/*
   	Initialize codec in Q mode

*/
uint8_t
ffmpegEncoder::getResult (void *ress)
{
  memcpy (ress, &res, sizeof (res));
  return 1;
}

uint8_t
ffmpegEncoder::encodePreamble (uint8_t * in)
{
  _frame.linesize[0] = _w;
  _frame.linesize[1] = _w >> 1;
  _frame.linesize[2] = _w >> 1;

/*
   		It seems ffmpeg likes the u & v to be swapped WTF...

*/

  _frame.data[0] = in;
  _frame.data[2] = in + _w * _h;
  _frame.data[1] = in + _w * _h + ((_w * _h) >> 2);

  _frame.key_frame = 0;
  _frame.pict_type = 0;
  _frame.quality = 0;
  return 1;
}

uint32_t
ffmpegEncoder::frameType (void)
{
  int k, t;

  k = _context->coded_frame->key_frame;
  t = _context->coded_frame->pict_type;

  //printf(" Kf: %d type :%d \n",k,t);
  if (k == 1)
    return AVI_KEY_FRAME;
  if (t == FF_B_TYPE)
    return AVI_B_FRAME;
  //if(t==FF_I_TYPE) return AVI_KEY_FRAME;
  return 0;

}

/*
	This is called for mpeg1/2 initContext
	Set some specific stuff.

*/
uint8_t
ffmpegEncoder::gopMpeg1 (void)
{
  // small gop, 2 b frames allowed
  // min bitrate 500 max bitrate 2200



  if (_id == FF_MPEG2)
    {
      if (FRAME_FILM == identMovieType (_context->frame_rate))
	{
	  printf ("\nPulldown activated...\n");
	  _context->flags2 |= CODEC_FLAG2_32_PULLDOWN;
	}
    }
#ifdef  TEST_NOB		// disable B frames
  _context->max_b_frames = 0;
#else
  _context->max_b_frames = 2;
#endif
  printf ("\n Using 2 b frames \n");
  if (_id == FF_MPEG2)
    {
      _context->mpeg_quant = 1;	//1; // Should be mpeg quant §
    }
  else
    {
      _context->mpeg_quant = 0;	//1; // Should be mpeg quant §
    }
  if (_settingsPresence)
    {
    	if(_settings.widescreen) 
	{
		_context->sample_aspect_ratio.num=16;
		_context->sample_aspect_ratio.den=9;
	}
	else
	{
		_context->sample_aspect_ratio.num=4;
		_context->sample_aspect_ratio.den=3;
	}

      _context->rc_max_rate_header = _settings.maxBitrate * 8;	//1800*1000;// 2400 max, 700 min
      _context->rc_buffer_size_header=_settings.bufferSize * 8 * 1024;
      // If we don't have a maxrate, don't set buffer_size
      if(1 && !_settings.override_ratecontrol) // FIXME
      
      {
      		_context->rc_buffer_size = _context->rc_buffer_size_header;
		_context->rc_max_rate    = _context->rc_max_rate_header;
	}
	else
	{
		_context->rc_buffer_size=0; // for xvid, no VBV so no ratecontrol
		_context->rc_max_rate   = 0;
	}
      _context->gop_size = _settings.gop_size;

    }
  else
    {
      _context->rc_buffer_size = 200 * 8 * 1024;	// 40 for VCD  & 200 for SVCD
      _context->gop_size = _settings.gop_size;
      
    }
  _context->rc_buffer_aggressivity = 1.0;
  _context->rc_initial_cplx = 3;
  _context->qmin = 2;
  _context->qmax = 31;

  _context->scenechange_threshold = 0xfffffff;	// Don't insert I frame out of order

  _frame.interlaced_frame=_settings.interlaced;
  if (_settings.interlaced) 
  	_frame.top_field_first=!_settings.bff;
  
#if defined(CODEC_FLAG_INTERLACED_DCT)
  _context->flags |= _settings.interlaced ? CODEC_FLAG_INTERLACED_DCT : 0;
#endif
#if defined(CODEC_FLAG_INTERLACED_ME)
  _context->flags |= _settings.interlaced ? CODEC_FLAG_INTERLACED_ME : 0;
#endif
  
  //
  //_context->dsp_mask= FF_MM_FORCE;
  printf ("Mpeg12 settings:\n____________\n");
  printf ("FF Max rate   (header) : %lu kbps\n", (_context->rc_max_rate_header) / 1000);
  printf ("FF Buffer Size(header) : %lu bits / %lu kB\n", (_context->rc_buffer_size_header),
	  _context->rc_buffer_size_header / (8 * 1024));
  printf ("FF Max rate   (rc) : %lu kbps\n", (_context->rc_max_rate) / 1000);
  printf ("FF Buffer Size(rc) : %lu bits / %lu kB\n", (_context->rc_buffer_size),
	  _context->rc_buffer_size / (8 * 1024));
  
  printf ("FF GOP Size    : %lu\n", _context->gop_size);
  return 1;
}

uint8_t
ffmpegEncoder::initContext (void)
{
  int res = 0;

  // set a gop size close to what's requested for most
  // player compatiblity               
  if (_id == FF_MPEG1 || _id == FF_MPEG2)
    gopMpeg1 ();
  if (_id == FF_HUFF || _id == FF_FFV1)
    _context->strict_std_compliance = -1;
  switch (_id)
    {
    case FF_MPEG4:
      res = avcodec_open (_context, &mpeg4_encoder);
      break;
    case FF_MSMP4V3:
      res = avcodec_open (_context, &msmpeg4v3_encoder);
      break;
    case FF_MPEG1:
      res = avcodec_open (_context, &mpeg1video_encoder);
      break;
    case FF_MPEG2:
      res = avcodec_open (_context, &mpeg2video_encoder);
      break;
    case FF_H263:
      res = avcodec_open (_context, &h263_encoder);
      break;
    case FF_H263P:
      res = avcodec_open (_context, &h263p_encoder);
      break;
    case FF_HUFF:
      res = avcodec_open (_context, &huffyuv_encoder);
      break;
    case FF_FFV1:
      res = avcodec_open (_context, &ffv1_encoder);
      break;
    case FF_MJPEG:
      res = avcodec_open (_context, &mjpeg_encoder);
      break;
    case FF_SNOW:
      res = avcodec_open (_context, &snow_encoder);
      break;
    default:
      ADM_assert (0);
    }

  if (res < 0)
    {
      printf ("\n Problem opening ffmpeg codec\n");
      return 0;
    }
  return 1;

}

/*
		Set user selected matrices.

*/
uint8_t
ffmpegEncoder::setCustomMatrices (uint16_t * intra, uint16_t * inter)
{
  _context->intra_matrix = intra;
  _context->inter_matrix = inter;
  return 1;
}

uint8_t
ffmpegEncoder::setGopSize (uint32_t size)
{

  _context->gop_size = size;
  printf ("\n Gop size is now %d\n", _context->gop_size);
  return 1;

}

uint8_t
ffmpegEncoder::setLogFile (const char *name)
{
  strcpy (LogName, name);
  return 1;

}

uint8_t
ffmpegEncoder::setConfig (FFcodecSetting * set)
{
  memcpy (&_settings, set, sizeof (_settings));
  _settingsPresence = 1;
  return 1;

}

uint8_t
ffmpegEncoderCQ::init (uint32_t val, uint32_t fps1000, uint8_t vbr)
{

  mplayer_init ();
  _qual = val;
  _vbr = vbr;
  _context->flags |= CODEC_FLAG_QSCALE;

  if (_vbr)
    {
      _context->flags |= CODEC_FLAG_PASS1;
      _statfile = NULL;

    }

  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;

  _context->bit_rate = 0;
  _context->bit_rate_tolerance = 1024 * 8 * 1000;

  return initContext ();
}

uint32_t ffmpegEncoder::getCodedFrame(void)
{
	return _last_coded_frame;
}

uint8_t
  ffmpegEncoderCQ::encode (ADMImage * in,
			   uint8_t * out, uint32_t * len, uint32_t * flags)
{
  int32_t sz = 0;
  uint32_t f;
  encodePreamble (in->data);

  _frame.quality = (int) floor (FF_QP2LAMBDA * _qual + 0.5);

  //_context->quality=_qual;

  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  f = frameType ();
  if (flags)
    *flags = f;
  res.is_key_frame = 0;
  if (f == AVI_KEY_FRAME)
    res.is_key_frame = 1;
  res.texture_bits = _context->i_tex_bits + _context->p_tex_bits;
  res.motion_bits = _context->mv_bits;
  res.total_bits = sz * 8;	// bytes -> bits
  res.quantizer = 2;
  res.out_quantizer =
    (int) floor (_context->coded_frame->quality / (float) FF_QP2LAMBDA);


  //      printf("text : %d motion %d\n",res.texture_bits,res.motion_bits);


  if (_vbr)			// update for lavcodec internal
    {
      if (!_statfile)
	{
	  printf ("FFmpeg using %s as log file\n", LogName);
	  _statfile = fopen (LogName, "wb");
	}

      if (!_statfile)
	{
	  printf
	    ("\n FF codec : cannot open log file for writing ! (%s)\n",
	     LogName);
	  return 0;
	}
      if (_context->stats_out)
	fprintf (_statfile, "%s", _context->stats_out);
    }

  return 1;

}

/*
_____________________________________________
*/
/*
   	Initialize codec in CBR mode

*/

uint8_t
ffmpegEncoderCBR::init (uint32_t val, uint32_t fps1000)
{
//       mpeg4_encoder
  //
//              now init our stuff
//
  _br = val;
  mplayer_init ();
  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;

  _context->bit_rate = _br;

  printf ("--> br: %lu", _br);

  return initContext ();

}

uint8_t
  ffmpegEncoderCBR::encode (ADMImage * in,
			    uint8_t * out, uint32_t * len, uint32_t * flags)
{
  int32_t sz = 0;

  encodePreamble (in->data);
  _context->bit_rate = _br;


  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  res.out_quantizer =
    (int) floor (_context->coded_frame->quality / (float) FF_QP2LAMBDA);
  if (flags)
//               if(_context->key_frame)
    *flags = frameType ();
  return 1;

}

//------------------------------------------------------------------------------
//              VBR
//------------------------------------------------------------------------------
uint8_t
  ffmpegEncoderVBR::encode (ADMImage * in,
			    uint8_t * out, uint32_t * len, uint32_t * flags)
{
  uint16_t q;
  uint8_t kf;

  q = (*flags) >> 8;
  kf = (*flags) & 1;

  return encodeVBR (in, out, len, flags, q, kf);

}

/*----------------------------- ffmpeg VBR , internal 2 pass engine --------------------*/
uint8_t
  ffmpegEncoderVBR::encodeVBR (ADMImage * in,
			       uint8_t * out,
			       uint32_t * len,
			       uint32_t * flags, uint16_t nq,
			       uint8_t forcekey)
{
  UNUSED_ARG (nq);
  UNUSED_ARG (forcekey);
  int32_t sz = 0;

  encodePreamble (in->data);


  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  if (flags)
    *flags = frameType ();
  res.out_quantizer =
    (int) floor (_context->coded_frame->quality / (float) FF_QP2LAMBDA);
  return 1;


}

/*
   			val is the average bitrate wanted, else it is useless



*/
uint8_t
ffmpegEncoderVBR::init (uint32_t val, uint32_t fps1000)
{
  uint32_t statSize;
  FILE *_statfile;

  printf ("\n initializing FFmpeg in VBR mode \n");
  _qual = val;
  mplayer_init ();

  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;



  /* If internal 2 passes mode is selected ... */
  _context->flags |= CODEC_FLAG_PASS2;

  _statfile = fopen (LogName, "rb");
  if (!_statfile)
    {
      printf ("\n internal file does not exists ?\n");
      return 0;
    }

  fseek (_statfile, 0, SEEK_END);
  statSize = ftello (_statfile);
  fseek (_statfile, 0, SEEK_SET);
  _context->stats_in = (char *) ADM_alloc (statSize + 1);
  _context->stats_in[statSize] = 0;
  fread (_context->stats_in, statSize, 1, _statfile);

  _context->bit_rate = val;	// bitrate

  return initContext ();

}

ffmpegEncoderVBR::~ffmpegEncoderVBR ()
{


  ADM_dealloc (_context->stats_in);

}

//--------------------- FFmpeg VBR, external Qzation engine ------------------
uint8_t
  ffmpegEncoderVBRExternal::encode (ADMImage * in,
				    uint8_t * out,
				    uint32_t * len, uint32_t * flags)
{
  uint16_t q;
  uint8_t kf;

  q = (*flags) >> 16;
  kf = (*flags) & 1;

  return encodeVBR (in, out, len, flags, q, kf);

}

uint8_t
  ffmpegEncoderVBRExternal::encodeVBR (ADMImage * in,
				       uint8_t * out,
				       uint32_t * len,
				       uint32_t * flags,
				       uint16_t nq, uint8_t forcekey)
{
  UNUSED_ARG (nq);
  UNUSED_ARG (forcekey);
  int32_t sz = 0;
  uint32_t f;
  encodePreamble (in->data);



  //_frame.quality=nq;
  _frame.quality = (int) floor (FF_QP2LAMBDA * nq + 0.5);
  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  f = frameType ();
  if (flags)
    *flags = f;
  res.out_quantizer =
    (int) floor (_context->coded_frame->quality / (float) FF_QP2LAMBDA);
  if (f == AVI_KEY_FRAME)
    res.is_key_frame = 1;
  else
    res.is_key_frame = 0;
  return 1;


}


/**
	This is used only for Mpeg1, so it is a bit tuned for it
*/
uint8_t
ffmpegEncoderVBRExternal::init (uint32_t val, uint32_t fps1000)
{

  printf ("\n initializing FFmpeg in VBRExternal  mode \n");
  _qual = val;
  mplayer_init ();

  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;
  _context->flags |= CODEC_FLAG_QSCALE;;
  _context->bit_rate = 0;
  _context->bit_rate_tolerance = 1024 * 8 * 1000;
  _context->max_qdiff = 10;

  // since this is used only for mpeg1 ...
  // PAL ?
  _context->bit_rate = 2500 * 1000 * 8;
  _context->sample_aspect_ratio.num = 4;
  _context->sample_aspect_ratio.den = 3;

  return initContext ();;
}

ffmpegEncoderVBRExternal::~ffmpegEncoderVBRExternal ()
{



}

//--------------------- FFmpeg VBR, external Qzation engine ------------------

void
ffmpegEncoder::mplayer_init (void)
{
  /*
     default values : Copy/past from mplayer
   */
  _context->pix_fmt = PIX_FMT_YUV420P;	//PIX_FMT_YV12;

  if (!_settingsPresence)
    {
      printf ("\n using FFmpeg default encode settings \n");
      _context->qmin = 2;
      _context->qmax = 31;
      _context->max_b_frames = 0;
      _context->mpeg_quant = 0;
      _context->me_method = ME_EPZS;
      _context->flags = 0;
      _context->max_qdiff = 3;
      _context->luma_elim_threshold = 0;
      _context->chroma_elim_threshold = 0;
      _context->lumi_masking = 0.0;;
      _context->dark_masking = 0.0;
      _context->qcompress = 0.5;
      _context->qblur = 0.5;
      _context->gop_size = 250;
    }
  else
    {
      if (_id == FF_MPEG1 || _id == FF_MPEG2)
	{
	  _context->gop_size = _settings.gop_size;
	}
      else
	{
	  _context->gop_size = 250;
	}
#define SETX(x) _context->x=_settings.x; printf(#x" : %d\n",_settings.x);
#define SETX_COND(x) if(_settings.is_##x) {_context->x=_settings.x; printf(#x" : %d\n",_settings.x);} else\
		{ printf(#x" is not activated\n");}
      SETX (me_method);
      SETX (qmin);
      SETX (qmax);
      SETX (max_b_frames);
      SETX (mpeg_quant);
      SETX (max_qdiff);
      SETX_COND (luma_elim_threshold);
      SETX_COND (chroma_elim_threshold);

#undef SETX
#undef SETX_COND

#define SETX(x)  _context->x=_settings.x; printf(#x" : %f\n",_settings.x);
#define SETX_COND(x)  if(_settings.is_##x) {_context->x=_settings.x; printf(#x" : %f\n",_settings.x);} else  \
									{printf(#x" No activated\n");}
      SETX_COND (lumi_masking);
      SETX_COND (dark_masking);
      SETX (qcompress);
      SETX (qblur);
      SETX_COND (temporal_cplx_masking);
      SETX_COND (spatial_cplx_masking);

#undef SETX
#undef SETX_COND

#define SETX(x) if(_settings.x){ _context->flags|=CODEC_FLAG##x;printf(#x" is set\n");}
      SETX (_GMC);


      switch (_settings.mb_eval)
	{
	case 0:
	  _context->mb_decision = FF_MB_DECISION_SIMPLE;
	  break;
	case 1:
	  _context->mb_decision = FF_MB_DECISION_BITS;
	  break;
	case 2:
	  _context->mb_decision = FF_MB_DECISION_RD;
	  break;
	default:
	  ADM_assert (0);


	}
      //SETX(_HQ);
      SETX (_4MV);
      SETX (_QPEL);
      SETX (_TRELLIS_QUANT);
      SETX (_NORMALIZE_AQP);

      if (_settings.widescreen)
	{
	  _context->sample_aspect_ratio.num = 16;
	  _context->sample_aspect_ratio.den = 9;
	  printf ("16/9 aspect ratio is set.\n");

	}
#undef SETX
    }
  if ((_id == FF_H263) || (_id == FF_H263P))
    _context->bit_rate_tolerance = 4000;
  else
    _context->bit_rate_tolerance = 8000000;


  _context->b_quant_factor = 1.25;
  _context->rc_strategy = 2;
  _context->b_frame_strategy = 0;
  _context->b_quant_offset = 1.25;
  _context->rtp_payload_size = 0;
  _context->strict_std_compliance = 0;
  _context->i_quant_factor = 0.8;
  _context->i_quant_offset = 0.0;
  _context->rc_qsquish = 1.0;
  _context->rc_qmod_amp = 0;
  _context->rc_qmod_freq = 0;
  _context->rc_eq = const_cast < char *>("tex^qComp");
  _context->rc_max_rate = 000;
  _context->rc_min_rate = 000;
  _context->rc_buffer_size = 000;
  _context->rc_buffer_aggressivity = 1.0;
  _context->rc_initial_cplx = 0;
  _context->dct_algo = 0;
  _context->idct_algo = 0;
  _context->p_masking = 0.0;

  _context->bit_rate = 0;

}

uint8_t
ffmpegEncoder::init (uint32_t val, uint32_t fps1000)
{
  UNUSED_ARG (val);
  UNUSED_ARG (fps1000);
  return 0;
}

uint8_t
ffmpegEncoder::init (uint32_t val, uint32_t fps1000, uint8_t sw)
{
  UNUSED_ARG (sw);
  return init (val, fps1000);
}

uint8_t
  ffmpegEncoder::encode (ADMImage * in, uint8_t * out, uint32_t * len,
			 uint32_t * flags)
{
  UNUSED_ARG (in);
  UNUSED_ARG (out);
  UNUSED_ARG (len);
  UNUSED_ARG (flags);
  ADM_assert (0);
  return 0;
}

//------------------------------
uint8_t
ffmpegEncoderHuff::init (uint32_t val, uint32_t fps1000, uint8_t vbr)
{
  UNUSED_ARG (val);
  UNUSED_ARG (vbr);
  mplayer_init ();

  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;


  _context->bit_rate = 0;
  _context->bit_rate_tolerance = 1024 * 8 * 1000;
  _context->gop_size = 250;

  return initContext ();
}



uint8_t
  ffmpegEncoderHuff::encode (ADMImage * in,
			     uint8_t * out, uint32_t * len, uint32_t * flags)
{
  int32_t sz = 0;

  encodePreamble (in->data);



  //_context->quality=_qual;

  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  if (flags)
    *flags = AVI_KEY_FRAME;
  res.is_key_frame = _frame.key_frame;
  return 1;

}

uint8_t
ffmpegEncoder::getExtraData (uint32_t * l, uint8_t ** d)
{
  *d = (uint8_t *) _context->extradata;
  *l = _context->extradata_size;
  printf ("\n We got some extra data  : %lu \n", *l);
  if (*l)
    return 1;

  else
    return 0;
}

//----
//------------------------------
uint8_t
ffmpegEncoderFFV1::init (uint32_t val, uint32_t fps1000, uint8_t vbr)
{
  UNUSED_ARG (val);
  UNUSED_ARG (vbr);
  mplayer_init ();

  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;


  _context->bit_rate = 0;
  _context->bit_rate_tolerance = 1024 * 8 * 1000;
  _context->gop_size = 250;
  printf ("FFV1 codec initializing...\n");
  return initContext ();
}



uint8_t
  ffmpegEncoderFFV1::encode (ADMImage * in,
			     uint8_t * out, uint32_t * len, uint32_t * flags)
{
  int32_t sz = 0;

  encodePreamble (in->data);



  //_context->quality=_qual;

  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  if (flags)
    *flags = AVI_KEY_FRAME;
  res.is_key_frame = _frame.key_frame;
  return 1;

}

/*---
*/
uint8_t
  ffmpegEncoderFFMjpeg::init (uint32_t val, uint32_t fps1000, uint8_t vbr)
{
  UNUSED_ARG (val);
  UNUSED_ARG (vbr);
  mplayer_init ();

  float f;

  f = val;
  f = 31. - (29. * f / 100.);

  _qual = (uint32_t) floor (f);

  _context->frame_rate_base = 1000;
  _context->frame_rate = fps1000;
  _context->flags = CODEC_FLAG_QSCALE;
  _context->bit_rate = 0;
  _context->bit_rate_tolerance = 1024 * 8 * 1000;
  _context->gop_size = 250;
  printf ("FF Mjpeg codec initializing %d %% -> q =%d...\n", val, _qual);
  return initContext ();
}



uint8_t
  ffmpegEncoderFFMjpeg::encode (ADMImage * in,
				uint8_t * out, uint32_t * len,
				uint32_t * flags)
{
  int32_t sz = 0;

  encodePreamble (in->data);



//       _frame.quality=_qual;
  _frame.quality = (int) floor (FF_QP2LAMBDA * _qual + 0.5);


  if ((sz = avcodec_encode_video (_context, out, _w * _h * 3, &_frame)) < 0)
    return 0;
  _last_coded_frame=_context->real_pict_num;
  *len = (uint32_t) sz;
  if (flags)
    *flags = AVI_KEY_FRAME;
  res.is_key_frame = _frame.key_frame;
  return 1;

}
#endif
