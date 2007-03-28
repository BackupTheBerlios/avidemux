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
  uint32_t  globalHeader;
  uint32_t  nbThreads;
  char      *logfile;
  //
  uint32_t KeyframeBoost;
  uint32_t BframeReduction;
  uint32_t BitrateVariability;
  //
  uint32_t  MinQp;
  uint32_t  MaxQp;
  uint32_t  QpStep;
  //
  uint32_t  MaxRefFrames;
  uint32_t  SceneCut;
  uint32_t  MinIdr;
  uint32_t  MaxIdr;
  //
  uint32_t  _8x8;
  uint32_t  _8x8P;
  uint32_t  _8x8B;
  uint32_t  _4x4;
  uint32_t  _8x8I;
  uint32_t  _4x4I;
  //
  uint32_t  MaxBFrame;
  int32_t   Bias;
  //
  uint32_t BasReference;
  uint32_t BidirME;
  uint32_t Adaptative;
  uint32_t Weighted;
  uint32_t DirectMode;
  //
  uint32_t PartitionDecision;
  uint32_t Range;
  uint32_t Method;
  uint32_t AR_Num;
  uint32_t AR_Den;
  uint32_t DeblockingFilter;
  int32_t  Strength;
  int32_t  Threshold;
  uint32_t CABAC;
  uint32_t Trellis;
  uint32_t ChromaME;
  uint32_t MixedRefs;
  uint32_t NoiseReduction;
  //
  uint32_t idc;
  // Sadarax ui
  uint32_t fastPSkip;
  uint32_t DCTDecimate;
  uint32_t interlaced;
  
  uint32_t vbv_max_bitrate;
  uint32_t vbv_buffer_size;
  uint32_t vbv_buffer_init; /* In % to be translated to float between 0 and 1 */

} ADM_x264Param;

#endif
