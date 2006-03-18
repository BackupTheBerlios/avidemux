#ifndef ADM_CODEC_CONFIG_
#define ADM_CODEC_CONFIG_

#include "ADM_vidEncode.hxx"
// Yv12

COMPRES_PARAMS yv12codec = {
  CodecYV12,
  "YV12 (raw)",
  "YV12",
  "YV12",
  COMPRESS_CQ,
  1,
  1500,
  700,
  ADM_ENC_CAP_CQ,
  0,
  NULL,
  0,
  NULL
};
/********************** FFMPEG MPEG4*************************/
uint8_t getFFCompressParams (COMPRES_PARAMS * incoming);

FFcodecSetting ffmpeg4Extra = {
  ME_EPZS,			//     ME
  0,				//          GMC     
  1,				// 4MV
  0,				//           _QPEL;   
  0,				//           _TREILLIS_QUANT
  2,				//           qmin;
  31,				//          qmax;
  3,				//           max_qdiff;
  0,				//           max_b_frames;
  0,				//          mpeg_quant;
  1,				//
  -2,				//                 luma_elim_threshold;
  1,				//
  -5,				// chroma_elim_threshold;
  0.05,				//lumi_masking;
  1,				// is lumi
  0.01,				//dark_masking; 
  1,				// is dark
  0.5,				// qcompress amount of qscale change between easy & hard scenes (0.0-1.0
  0.5,				// qblur;    amount of qscale smoothing over time (0.0-1.0) 
  0,				// min bitrate in kB/S
  0,				// max bitrate
  0,				// default matrix
  0,				// no gop size
  NULL,
  NULL,
  0,				// interlaced
  0,				// WLA: bottom-field-first
  0,				// wide screen
  2,				// mb eval = distortion
  8000,				// vratetol 8Meg
  0,				// is temporal
  0.0,				// temporal masking
  0,				// is spatial
  0.0,				// spatial masking
  0,				// NAQ
  0				// DUMMY 
};

COMPRES_PARAMS ffmpegH263Codec = {
  CodecH263,
  "H263 (lavc)",
  "H263",
  "Lavcodec H263",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  getFFCompressParams
};
COMPRES_PARAMS ffmpegH263PCodec = {
  CodecH263P,
  "H263+ (lavc)",
  "H263P",
  "Lavcodec H263+",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  getFFCompressParams
};
COMPRES_PARAMS ffmpegMpeg4 = {
  CodecFF,
  "Mpeg4 (lavc)",
  "FFMpeg4",
  "Lavcodec Mpeg4",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  getFFCompressParams
};
COMPRES_PARAMS ffmpegSnow = {
  CodecSnow,
  "Snow (lavc)",
  "FFMpeg4",
  "Lavcodec Snow",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  NULL
};
COMPRES_PARAMS ffmpegFFHUFF = {
  CodecFFhuff,
  "FF HUFFYUV",
  "FFHUFF",
  "Lavcodec FF HUFFYUV",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  NULL
};
COMPRES_PARAMS ffmpegHUFF = {
  CodecHuff,
  "HUFFYUV",
  "HUFF",
  "Lavcodec HUFFYUV",
  COMPRESS_CQ,
  4,
  1500,
  700,
  0,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  NULL
};
COMPRES_PARAMS ffmpegFFV1 = {
  CodecFFV1,
  "FFV1 (lavcodec)",
  "FFV1",
  "Lavcodec FFV1",
  COMPRESS_CQ,
  4,
  1500,
  700,
  0,
  ADM_EXTRA_PARAM,
  &ffmpeg4Extra,
  sizeof (ffmpeg4Extra),
  NULL
};
static MJPEGConfig MjpegExtra = { 90, 0 };
extern uint8_t DIA_mjpegCodecSetting (COMPRES_PARAMS * param);
COMPRES_PARAMS MjpegCodec = {
  CodecMjpeg,
  "Mjpeg",
  "Mjpeg",
  "Mjpeg (lavcodec)",
  COMPRESS_CQ,
  4, 1500, 700,
  ADM_ENC_CAP_CQ,
  ADM_EXTRA_PARAM,
  &MjpegExtra,
  sizeof (MjpegExtra),
  &DIA_mjpegCodecSetting
};

//************************* FFMpeg mpeg1 **********************
FFcodecSetting ffmpeg1Extra = {
  ME_EPZS,			//     ME
  0,				//          GMC
  0,				//           _4MV;
  0,				//           _QPEL;
  0,				//           _TREILLIS_QUANT
  2,				//           qmin;
  31,				//          qmax;
  3,				//           max_qdiff;
  0,				//           max_b_frames;
  1,				//          mpeg_quant;
  1,				//
  -2,				//                 luma_elim_threshold;
  1,				//
  -5,				//                 chroma_elim_threshold;
  0.05,				//                lumi_masking;
  1,
  0.01,				//                dark_masking;
  1,
  0.5,				//         qcompress;  /* amount of qscale change between easy & hard scenes (0.0-1.0)*/
  0.5,				//         qblur;      /* amount of qscale smoothing over time (0.0-1.0) */
  (600 * 1000 >> 3),		// min bitrate in kB/S
  (2200 * 1000) >> 3,		// max bitrate
  0,				// user_matrix 0->default, 1 tmpg, 2 animé , 3 kvcd
  12,				// Safe gop size limit
  NULL,				// inter & intra matrix, will be set depending on user_matrix
  NULL,
  0,				// interlaced
  0,				// WLA: bottom-field-first
  0,				// wide screen
  2,				// mb eval = distortion
  8000,				// vratetol
  0,
  0.5,				// temporal masking
  0,
  0.5,				// spatial masking
  0,				// NAQ
  0,				// Use xvid ratecontrol
  40,				// buffersize VCD like
  0				// DUMMY                
};

COMPRES_PARAMS ffmpeg1Codec = {
  CodecXVCD,
  "VCD (lavc)",
  "XVCD",
  "Lavcodec Mpeg1",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg1Extra,
  sizeof (ffmpeg1Extra),
  NULL
};
//************ ffmpeg mpeg2 DVD **********

FFcodecSetting ffmpeg2DVDExtra = {
  ME_EPZS,			//     ME
  0,				//          GMC
  0,				//           _4MV;
  0,				//           _QPEL;
  FF_TRELLIS,			//          _TREILLIS_QUANT
  2,				//           qmin;
  31,				//          qmax;
  3,				//           max_qdiff;
  2,				//           max_b_frames;
  1,				//          mpeg_quant;
  1,				//
  -2,				//                 luma_elim_threshold;
  1,				//
  -5,				//                 chroma_elim_threshold;
  0.05,				//                lumi_masking;
  1,
  0.01,				//                dark_masking;
  1,
  0.5,				//         qcompress;  /* amount of qscale change between easy & hard scenes (0.0-1.0)*/
  0.5,				//         qblur;      /* amount of qscale smoothing over time (0.0-1.0) */
  0,				// min bitrate in kB/S
  (8000 * 1000) >> 3,		// max bitrate
  0,				// user_matrix 0->default, 1 tmpg, 2 animé , 3 kvcd
  12,				// Safe gop size limit
  NULL,				// inter & intra matrix, will be set depending on user_matrix
  NULL,
  0,				// interlaced
  0,				// WLA: bottom-field-first
  0,				// wide screen
  2,				// mb eval = distortion
  8000,				// vratetol
  0,
  0.5,				// temporal masking
  0,
  0.5,				// spatial masking
  0,				// NAQ
  1,				// Use xvid ratecontrol
  240,				// buffersize 240 KB for Mpeg2 /
  0				// DUMMY        
};
uint8_t DIA_DVDffParam (COMPRES_PARAMS * incoming);
COMPRES_PARAMS ffmpeg2DVDCodec = {
  CodecXDVD,
  "DVD (lavc)",
  "XDVD",
  "Lavcodec Mpeg2 (DVD)",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg2DVDExtra,
  sizeof (ffmpeg2DVDExtra),
  DIA_DVDffParam
};
//************ ffmpeg mpeg2 SVCD **********

FFcodecSetting ffmpeg2SVCDExtra = {
  ME_EPZS,			//     ME
  0,				//          GMC
  0,				//           _4MV;
  0,				//           _QPEL;
  FF_TRELLIS,			//          _TREILLIS_QUANT
  2,				//           qmin;
  31,				//          qmax;
  3,				//           max_qdiff;
  2,				//           max_b_frames;
  1,				//          mpeg_quant;
  1,				//
  -2,				//                 luma_elim_threshold;
  1,				//
  -5,				//                 chroma_elim_threshold;
  0.05,				//                lumi_masking;
  1,
  0.01,				//                dark_masking;
  1,
  0.5,				//         qcompress;  /* amount of qscale change between easy & hard scenes (0.0-1.0)*/
  0.5,				//         qblur;      /* amount of qscale smoothing over time (0.0-1.0) */
  0,				// min bitrate in kB/S
  (2400 * 1000) >> 3,		// max bitrate
  0,				// user_matrix 0->default, 1 tmpg, 2 animé , 3 kvcd
  12,				// Safe gop size limit
  NULL,				// inter & intra matrix, will be set depending on user_matrix
  NULL,
  0,				// interlaced
  0,				// WLA: bottom-field-first
  0,				// wide screen
  2,				// mb eval = distortion
  2400,				// vratetol
  0,
  0.5,				// temporal masking
  0,
  0.5,				// spatial masking
  0,				// NAQ
  1,				// Use xvid ratecontrol
  112,				// buffersize 240 KB for Mpeg2 /
  0				// DUMMY        
};

COMPRES_PARAMS ffmpeg2SVCDCodec = {
  CodecXSVCD,
  "SVCD (lavc)",
  "XSVCD",
  "Lavcodec Mpeg2 (SVCD)",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &ffmpeg2SVCDExtra,
  sizeof (ffmpeg2SVCDExtra),
  DIA_DVDffParam
};
// ************ Mpeg2enc VCD *************


COMPRES_PARAMS VCDCodec = {
  CodecVCD,
  "VCD",
  "VCD",
  "Mpeg2enc(VCD)",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR,
  ADM_EXTRA_PARAM,
  NULL,
  0,
  NULL
};
//************  MPEG2Enc SVCD *******************


#include "mpeg2enc/ADM_mpeg2enc.h"
extern uint8_t DIA_SVCDParam (COMPRES_PARAMS * toto);


Mpeg2encParam SVCDExtra = {
  (2500 * 1000) >> 3,		// Max BR
  12,				// Gop size
  0,				//int   wideScreen;
  0,				//int   matrix;
  0,				//int   interlacingType;
  0				// bff
};

COMPRES_PARAMS SVCDCodec = {
  CodecSVCD,
  "SVCD",
  "SVCD",
  "Mpeg2enc(SVCD)",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &SVCDExtra,
  sizeof (SVCDExtra),
  DIA_SVCDParam
};
//********************************************
Mpeg2encParam DVDExtra = {
  (9000 * 1000) >> 3,		// Max BR
  12,				// Gop size
  0,				//int   wideScreen;
  0,				//int   matrix;
  0,				//int   interlacingType;
  0				// bff
};

COMPRES_PARAMS DVDCodec = {
  CodecDVD,
  "DVD ",
  "DVD",
  "Mpeg2enc(DVD)",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &DVDExtra,
  sizeof (DVDExtra),
  DIA_SVCDParam
};

/*
//*************************** Xvid 0.9*****************
*/
#ifdef USE_XX_XVID
#error !!! XVID 0.9 support not codec
#error !!! XVID 0.9 support not codec
#error !!! XVID 0.9 support not codec
#error !!! XVID 0.9 support not codec


#endif

//*************************** Xvid 4*****************

#ifdef USE_XVID_4
xvid4EncParam xvid4Extra = {
  6,				//int guiLevel;              

  1,				//int min_key_interval;
  250,				// Max key interval
  2,				//int bframes;

  0,				//int mpegQuantizer; 
  0,				//int interlaced;
  1,				//int inter4mv;
  0,				//int trellis;       
  0,				//int cartoon;
  0,				//int greyscale;             
  0,				// qpel
  0,				// GMC
  1,				// hqac
  0,				// Chroma optim
  {2, 2, 2}
  ,				//qmin
  {31, 31, 31}
  ,				//qmax

  // This if for 2 pass   
  0,				//int keyframe_boost;
  0,				//int curve_compression_high;
  0,				//int curve_compression_low;
  5,				//int overflow_control_strength;
  5,				//int max_overflow_improvement;
  5,				//int max_overflow_degradation;
  0,				//int kfreduction;
  0,				//int kfthreshold;

  0,				//24,//int container_frame_overhead;
  150,				//int bquant_ratio;
  100,				//int bquant_offset;    
  1,				//vhqmode                
  1,				// chroma me
  0,				// turbo
  0,				// Packed bitstream - Not Xvid Default
  1,				// closed_gop
  0				// bframe_threshold
};
extern uint8_t DIA_xvid4 (COMPRES_PARAMS * incoming);
COMPRES_PARAMS Xvid4Codec = {
  CodecXvid4,
  "Xvid4",
  "XVID4",
  "XVID 4 mpeg4",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS,
  ADM_EXTRA_PARAM,
  &xvid4Extra,
  sizeof (xvid4Extra),
  DIA_xvid4
};
#endif


///////////////////////////////////////////:
#ifdef USE_X264

#include "ADM_encoder/adm_encx264.h"

ADM_x264Param x264Extra = {


  2,				//uint32_t qmin;
  51,				//uint32_t qmax;
  1,				//uint32_t cabac;
  250,				//uint32_t maxKf;
  5,				//uint32_t minKf;
  2,				//uint32_t nbBframe;  
  0,				// uint32_t esAtom
  0				// nbThreads
};
extern uint8_t DIA_x264 (COMPRES_PARAMS * conf);
COMPRES_PARAMS x264Codec = {
  CodecX264,
  "x264",
  "X264",
  "x264 h264 encoder",
  COMPRESS_CQ,
  4,
  1500,
  700,
  ADM_ENC_CAP_CBR + ADM_ENC_CAP_CQ + ADM_ENC_CAP_2PASS + ADM_ENC_CAP_GLOBAL,
  ADM_EXTRA_PARAM,
  &x264Extra,
  sizeof (x264Extra),
  DIA_x264
};

#endif
COMPRES_PARAMS DUMMYONE =
  { CodecDummy, "dummy", "dummy", "dummy", COMPRESS_CQ, 4, 1500, 700, 0, 0,
NULL, 0 };
COMPRES_PARAMS CopyCodec =
  { CodecCopy, "Copy", "Copy", "Copy", COMPRESS_CQ, 4, 1500, 700, 0, 0, NULL,
0 };

COMPRES_PARAMS *AllVideoCodec[] = {
  &CopyCodec,
  &ffmpegH263Codec,
  &MjpegCodec,
//ffmpegH263PCodec,
  &ffmpegMpeg4,
  &ffmpeg1Codec,
  &ffmpeg2DVDCodec,
  &ffmpeg2SVCDCodec,
  &VCDCodec,
  &SVCDCodec,
  &DVDCodec,
#ifdef USE_XVID_4
  &Xvid4Codec,
#endif
#ifdef USE_X264
  &x264Codec,
#endif
  &ffmpegHUFF,
  &ffmpegFFHUFF,
  &ffmpegFFV1,
  &yv12codec,
//&ffmpegSnow,
  &DUMMYONE
};
#endif
