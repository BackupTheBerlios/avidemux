/***************************************************************************
                          ADM_vidFieldUtil.h  -  description
                             -------------------

Some functions to manipulate fields

    begin                : Sun Apr 11 2003
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
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"



#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFieldUtil.h"


void vidFieldDecimate(uint8_t *src,uint8_t *target, uint32_t linessrc, uint32_t width)
{
	linessrc>>=1;
	for(;linessrc>0;linessrc--)
		{
			memcpy(target,src,width);
			target+=width;
			src+=width*2;
		}


}

void vidFieldKeepOdd(uint32_t w,uint32_t h,uint8_t *src,uint8_t *target)
{

		uint32_t page=w*h;

 		vidFieldDecimate(src+w,target,h*2,w);

		src+=page*2;
		target+=page;
		vidFieldDecimate(src+(w>>1),target,h,w>>1);

		src+=page>>1;
		target+=page>>2;
		vidFieldDecimate(src+(w>>1),target,h,w>>1);
}
void vidFieldKeepEven(uint32_t w,uint32_t h,uint8_t *src,uint8_t *target)
{
	uint32_t page=w*h;

		vidFieldDecimate(src,target,h*2,w);

		src+=page*2;
		target+=page;
		vidFieldDecimate(src,target,h,w>>1);

		src+=page>>1;
		target+=page>>2;
		vidFieldDecimate(src,target,h,w>>1);
}
void vidFieldMerge(uint32_t w,uint32_t h,uint8_t *src,uint8_t *src2,uint8_t *target)
{
uint8_t *d,*s,*s2;
//uint32_t y;

// interleave Y

	d=target;
	s=src;
	s2=src2;
	for(uint32_t y=h>>1;y>0;y--)
	{
		memcpy(d,s,w);
		memcpy(d+w,s2,w);
		s+=w;
		s2+=w;
		d+=w*2;
	}
	//
	d=target+(w*h);
	s=src+((w*h)>>1);
	s2=src2+((w*h)>>1);
	for(uint32_t y=h>>1;y>0;y--)
	{
		memcpy(d,s,w>>1);
		memcpy(d+(w>>1),s2,w>>1);
		s+=w>>1;
		s2+=w>>1;
		d+=w;
	}

}

