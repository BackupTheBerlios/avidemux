//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef __X264_PARM_
#define __X264_PARM_

typedef struct ADM_x264Param
{
	uint32_t  globalHeader;				// b_repeat_headers  {0 = No, 1 = Yes}
	uint32_t  nbThreads;				// i_threads
	char      *logfile;					// logfile

	uint32_t KeyframeBoost;				// *** not used ***
	uint32_t BframeReduction;			// *** not used ***
	uint32_t BitrateVariability;		// rc.f_qcompress  (% to be translated to float between 0 and 1)

	uint32_t  MinQp;					// rc.i_qp_min
	uint32_t  MaxQp;					// rc.i_qp_max
	uint32_t  QpStep;					// rc.i_qp_step

	uint32_t  MaxRefFrames;				// i_frame_reference  (0 - 16)
	uint32_t  SceneCut;
	uint32_t  MinIdr;					// i_keyint_min  (0 - 100)
	uint32_t  MaxIdr;					// i_keyint_max  (1 - 1000)

	uint32_t  _8x8;						// analyse.b_transform_8x8  {0 = No, 1 = Yes}
	uint32_t  _8x8P;					// analyse.inter
	uint32_t  _8x8B;					// analyse.inter
	uint32_t  _4x4;						// analyse.inter
	uint32_t  _8x8I;					// analyse.inter
	uint32_t  _4x4I;					// analyse.inter

	uint32_t  MaxBFrame;				// i_bframe  (0 - 15)
	int32_t   Bias;						// i_bframe_bias  (-100 - 100)

	uint32_t BasReference;				// b_bframe_pyramid  {0 = No, 1 = Yes}
	uint32_t BidirME;					// analyse.b_bidir_me  {0 = No, 1 = Yes}
	uint32_t Adaptative;				// b_bframe_adaptive  {0 = No, 1 = Yes}
	uint32_t Weighted;					// analyse.b_weighted_bipred  {0 = No, 1 = Yes}
	uint32_t DirectMode;				// analyse.i_direct_mv_pred  {0 = none, 1 = spatial, 2 = temporal, 3 = auto}

	uint32_t PartitionDecision;			// analyse.i_subpel_refine	(1 - 7)
	uint32_t RDO;						// analyse.b_bframe_rdo  {0 = No, 1 = Yes}
	uint32_t Range;						// analyse.i_me_range  (0 - 64)
	uint32_t Method;					// analyse.i_me_method  {0 = dia, 1 = hex, 2 = umh, 3 = esa, 4 = tesa}
	uint32_t AR_Num;					// vui.i_sar_width
	uint32_t AR_Den;					// vui.i_sar_height
	uint8_t  AR_AsInput;				// vui.i_sar_width, vui.i_sar_height
	uint32_t DeblockingFilter;			// b_deblocking_filter  {0 = No, 1 = Yes}
	int32_t  Strength;					// i_deblocking_filter_alphac0  (-6 - 6)
	int32_t  Threshold;					// i_deblocking_filter_beta  (-6 - 6)
	uint32_t CABAC;						// b_cabac  {0 = No, 1 = Yes}
	uint32_t Trellis;					// analyse.i_trellis  (0 - 2)
	uint32_t ChromaME;					// analyse.b_chroma_me  {0 = No, 1 = Yes}
	uint32_t MixedRefs;					// analyse.b_mixed_references  {0 = No, 1 = Yes}
	uint32_t NoiseReduction;			// analyse.i_noise_reduction  (0 - 65536)
	uint32_t idc;						// i_level_idc

	uint32_t fastPSkip;					// analyse.b_fast_pskip  {0 = No, 1 = Yes}
	uint32_t DCTDecimate;				// analyse.b_dct_decimate  {0 = No, 1 = Yes}
	uint32_t interlaced;				// b_interlaced  {0 = No, 1 = Yes}

	uint32_t vbv_max_bitrate;			// rc.i_vbv_max_bitrate
	uint32_t vbv_buffer_size;			// rc.i_vbv_buffer_size
	uint32_t vbv_buffer_init;			// rc.f_vbv_buffer_init  (% to be translated to float between 0 and 1)

	int32_t mv_range;					// analyse.i_mv_range  {-1 = off, (32 - 512)}
	int32_t mv_range_thread;			// analyse.i_mv_range_thread
	int32_t direct_8x8_inference;		// analyse.i_direct_8x8_inference  {-1 = smallest possible, 1 = 8x8, 2 = 4x4}
	uint32_t pre_scenecut;				// b_pre_scenecut  {0 = No, 1 = Yes}
	uint32_t interLumaDeadzone;			// analyse.i_luma_deadzone[0]  (0 - 32)
	uint32_t intraLumaDeadzone;			// analyse.i_luma_deadzone[1]  (0 - 32)
	uint32_t cqmPreset;					// i_cqm_preset  {0 = flat, 1 = jvt, 2 = custom}

	uint8_t intra4x4Luma[16];			// cqm_4iy
	uint8_t intraChroma[16];			// cqm_4ic
	uint8_t inter4x4Luma[16];			// cqm_4py
	uint8_t interChroma[16];			// cqm_4pc
	uint8_t intra8x8Luma[64];			// cqm_8iy
	uint8_t inter8x8Luma[64];			// cqm_8py

	uint8_t rateTolerance;				// rc.f_rate_tolerance  (% to be translated to float between 0 and 1)
	float quantiserIpRatio;				// rc.f_ip_factor  (0 - 10)
	float quantiserPbRatio;				// rc.f_pb_factor  (0 - 10)
	uint8_t chromaQuantiserOffset;		// analyse.i_chroma_qp_offset  (-12 - 12)
	float quantiserComplexityBlur;		// rc.f_complexity_blur
	float quantiserBlur;				// rc.f_qblur

	uint8_t spsId;						// i_sps_id
	uint8_t deterministic;				// b_deterministic  {0 = No, 1 = Yes}
	uint8_t accessUnitDelimiters;		// b_aud  {0 = No, 1 = Yes}
	uint8_t psnrComputation;			// analyse.b_psnr  {0 = No, 1 = Yes}
	uint8_t ssimComputation;			// analyse.b_ssim  {0 = No, 1 = Yes}

	uint8_t overscan;					// vui.i_overscan
	uint8_t videoFormat;				// vui.i_vidformat
	uint8_t colourPrimaries;			// vui.i_colorprim
	uint8_t transferCharacteristics;	// vui.i_transfer
	uint8_t colourMatrix;				// vui.i_colmatrix
	uint8_t chromaSampleLocation;		// vui.i_chroma_loc
	uint8_t fullRangeSamples;			// vui.b_fullrange  {0 = No, 1 = Yes}
} ADM_x264Param;

#endif
