
/***************************************************************************
                          yv.cpp  -  description
                             -------------------

	Convert a RGB triplet to its YUV conterpart

    begin                : Wed Aug 12 2003
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

#include <stdio.h>
#include <stdlib.h>
#include <ADM_assert.h>
#include <string.h>
#include <math.h>
#include "config.h"
#include "ADM_library/default.h"


#include "colorspace.h"

/**
	This is unoptimized because seldom called
*/
uint8_t COL_RgbToYuv(uint8_t R,uint8_t G,uint8_t B, uint8_t *y,int8_t *u,int8_t *v)
{

	float rr=R,bb=B,gg=G;
	float yy,uu,vv;

	yy=0.299*rr+ 		0.587*gg+ 	0.114*bb;
	uu=-0.169*rr+ 	-0.331*gg+  	0.5*bb;
	vv=0.5*rr+ 		-0.419*gg+ 	-0.081*bb;


	if(uu>127) uu=127;
	if(uu<-127) uu=-127;
	*u=(int8_t)floor(uu);

	if(vv>127) vv=127;
	if(vv<-127) vv=-127;
	*v=(int8_t)floor(vv);

	if(yy>255) yy=255;
	if(yy<0) yy=0;
	*y=(uint8_t)floor(yy);

	return 1;

}
uint8_t COL_YuvToRgb( uint8_t y,int8_t u,int8_t v,uint8_t *r,uint8_t *g,uint8_t *b)
{

	float rr,bb,gg;
	float yy=y,uu=u,vv=v;

	rr=	yy+			 	1.402*vv;
	gg= yy+ 	-0.344*uu+  	-0.714*vv;
	bb=	yy+ 	1.772*uu 	 		;

	#define CLIP(x) if(x>255) x=255; else if (x<0) x=0;x=x+0.49;
	#define CVT(x,y) CLIP(x);*y=(uint8_t)floor(x);

	CVT(rr,r);
	CVT(gg,g);
	CVT(bb,b);

	return 1;

}
