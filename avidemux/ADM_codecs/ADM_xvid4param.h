/***************************************************************************
                          GUI_xvidparam.h  -  description
                             -------------------
    begin                : Sun Nov 17 2002
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

#ifndef XVID4ENCPARAM__
#define XVID4ENCPARAM__

typedef struct xvid4EncParam
{

  int guiLevel;			// equivalent to divx level : Fast/medium/insane

  int min_key_interval;
  int max_key_interval;
  int bframes;

  int mpegQuantizer;		// 0 h263/ 1 Mpeg quantizer
  int interlaced;
  int inter4mv;
  int trellis;
  int cartoon;
  int greyscale;

  int qpel;
  int gmc;
  int hqac;

  int chroma_opt;
  int qmin[3];			// IPB
  int qmax[3];			// IPB



  // This if for 2 pass   
  int keyframe_boost;
  int curve_compression_high;
  int curve_compression_low;
  int overflow_control_strength;
  int max_overflow_improvement;
  int max_overflow_degradation;
  int kfreduction;
  int kfthreshold;

  int container_frame_overhead;
  //
  int bquant_ratio;
  int bquant_offset;
  int vhqmode;
  int chroma_me;
  int turbo;
  int packed;
  int closed_gop;
  int bframe_threshold;
  char logName[200];
} xvid4EncParam;



#endif
