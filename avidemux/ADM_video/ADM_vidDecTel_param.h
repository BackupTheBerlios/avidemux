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
	int 	order;
	int 	back;
	int	back_saved;
	int 	guide;
	float 	gthresh;
	int 	post;
	int 	chroma;
	float 	vthresh;
	float	vthresh_saved;
	float 	bthresh;
	float 	dthresh;
	int 	blend;
	int 	nt;
	int 	y0;
	int 	y1;
	int 	hints;
	int 	show;
	int 	debug;
};
