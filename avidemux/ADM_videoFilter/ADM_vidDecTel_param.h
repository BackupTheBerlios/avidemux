//
// C++ Interface: ADM_vidDecTel_param
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//


typedef struct TelecideParam
{
	uint32_t 	order;
	uint32_t 	back;
	uint32_t	back_saved;
	uint32_t 	guide;
	double	 	gthresh;
	uint32_t 	post;
	uint32_t 	chroma;
	double 		vthresh;
	double		vthresh_saved;
	double 		bthresh;
	double 		dthresh;
	uint32_t 	blend;
	uint32_t 	nt;
	uint32_t 	y0;
	uint32_t 	y1;
	uint32_t 	hints;
	uint32_t 	show;
	uint32_t 	debug;
};
