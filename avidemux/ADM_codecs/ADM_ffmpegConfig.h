/***************************************************************************
                          ADM_ffmpegConfig -  description
                             -------------------
	Fine tuning for lavcodec encoder			     
			     
    begin                : 19-01-2003
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
#ifndef ADM_FFCONFIG
#define ADM_FFCONFIG
typedef struct FFcodecSetting
{
  Motion_Est_ID me_method;
  uint8_t _GMC;
  uint8_t _4MV;
  uint8_t _QPEL;
  uint8_t _TRELLIS_QUANT;
  int qmin;			// 2-31
  int qmax;			// 2-31
  int max_qdiff;		// 1-31
  int max_b_frames;		// 0-1
  int mpeg_quant;		// 0-1
  int is_luma_elim_threshold;
  int luma_elim_threshold;	// -99--99
  int is_chroma_elim_threshold;	// -99--99           
  int chroma_elim_threshold;	// -99--99      

  float lumi_masking;		// -1--1        
  int is_lumi_masking;		// -1--1
  float dark_masking;		// -1--1        
  int is_dark_masking;		// -1--1
  float qcompress;		// 0.0--1.0
  float qblur;			// 0.0--1.0
  int minBitrate;
  int maxBitrate;
  int user_matrix;		// 0 normal / 1 tmpgenc / 2 anime / 3 kvcd / 4 hr-tmpgenc
  int gop_size;			// For mpeg1/2 , 12 is good
  uint16_t *intra_matrix;
  uint16_t *inter_matrix;
  uint8_t interlaced;
  uint8_t bff;			// WLA: bottom field first flag
  uint8_t widescreen;

  // new stuff from jakub ui
  int mb_eval;			// Replace hq 0..2
  int vratetol;			// filesize tolerance in kb

  int is_temporal_cplx_masking;	// temporal masking 0--1        
  float temporal_cplx_masking;	// temporal masking 0--1

  int is_spatial_cplx_masking;	// spatial masking 0--1
  float spatial_cplx_masking;	// spatial masking 0--1
  int _NORMALIZE_AQP;		// normalize adap quantiz

  //
  int use_xvid_ratecontrol;
  int bufferSize;		// in KBYTES !!!!
  int override_ratecontrol;
  int dummy;

} FFcodecSetting;



#endif
