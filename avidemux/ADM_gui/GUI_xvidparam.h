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

 #ifndef XVIDENCPARAM__
 #define XVIDENCPARAM__
 
 typedef struct xvidEncParam
 {

 	int gui_option;
	 int interlaced;
	 int inter4v;
	 int quantizer;
	 int imin;
	 int imax;
	 int pmin;
	 int pmax;

	 int lumi;
	 
 	 int me;
	 int halfpel  ;

	 /* 2 pass opt */
	 
	 int kfboost;
	 int min_key_interval;
	 int max_key_interval;
	 
	 	int curve_compression_high;
		int curve_compression_low;

		int 	bitrate_payback_delay;
	 
	 /*   Credits */
	 
	 int startcred_start;
	 int startcred_end;
	

	 int endcred_start;
	 int endcred_end;
	 
	 int cred_rate;
	 
	 /*     Alt */
	int use_alt_curve;
	int alt_curve_type;
	int alt_curve_low_dist;
	int alt_curve_high_dist;
	int alt_curve_min_rel_qual;
	int alt_curve_use_auto;
	int alt_curve_auto_str;
	int alt_curve_use_auto_bonus_bias;
	int alt_curve_bonus_bias;
	int bitrate_payback_method 	 ;
	
//	char logName[200];
}   xvidEncParam;


		      
#endif		      
