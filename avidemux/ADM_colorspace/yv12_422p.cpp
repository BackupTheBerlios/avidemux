/***************************************************************************
                          yv12_422p.cpp  -  description
                             -------------------
    begin                : Wed Aug 5 2009
    copyright            : (C) 2009 by mean
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

#include <inttypes.h>
#include "config.h"
#ifdef USE_MMX
/*
		0-> y1
		1-> y2
		2->u
       3->v
		4-> out
		5-> out 2

	Process 16 bytes = 4 pix from y1 and 4 pix from y2
*/
#define LOAD_M "                                                       \n\
# load y,u,v                                                 \n\
pxor				%%mm7,%%mm7					\n\
pxor				%%mm6,%%mm6					\n\
movd 				(%2),%%mm0    # load 4u                 00000 u3 U2 U1 U0   \n\
movd 				(%3),	%%mm1    # load 4v             00000 V3 V2 V1 V0      \n\
punpcklbw   	%%mm0 , %%mm2 #                 00 u3 00 u2 00 u1 00 u0 \n\
punpcklbw   	%%mm1 , %%mm3 #                 00 v3 00 v2 00 v1 00 V0 \n\
# \n\
punpcklwd 	%%mm7, %%mm2#                    00 00 00 u1 00 00 00 u0 \n\
punpcklwd 	%%mm7, %%mm3 #                   00 00 00 v1 00 00 00 v0 \n\
psllq      		$8 , %%mm4                     #       00 00 u1 00 00 00 u0 00 \n\
psllq      		$24 , %%mm5                    #      v1 00 00 00 v0 00 00 00 \n\
por					%%mm4,%%mm5                 #   v1 00 u1 00 v0 00 u0 00 \n\
# now do y1 and y2 \n\
movd				(%0), %%mm0 #				00 00 00 00 y3 y2 y1 y0 \n\
movd				(%1), %%mm1 #				00 00 00 00 y3 y2 y1 y0     \n\
punpcklbw   	%%mm0 , %%mm6 #                 00 y3 00 y2 00 y1 00 y0 \n\
punpcklbw   	%%mm1 , %%mm7 #                 00 y3 00 y2 00 y1 00 y0 \n\
por					%%mm5,%%mm6		#                 v1 y3 u1 y2 v0 y1 u0 y0 \n\
por					%%mm5,%%mm7		#                 v1 y3 u1 y2 v0 y1 u0 y0  \n\
movq      		%%mm6, (%4) # store \n\
movq      		%%mm7, (%5) # store     \n\
"






void fastYV12_422_mmx( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h)
{

	uint8_t *u,*v;
	
		u=in+w*h;
		v=u+(w*h>>2);

	for(uint32_t y=h>>1;y>0;y--)
	{
			for(uint32_t x=w>>2; x>0;x--)
			{

               __asm__( ".align 8"
										LOAD_M
								  : : "r" (in), "r" (in+w), "r" (u), "r" (v),"r" (out), "r" (out+2*w));

					in+=4;			
					u+=2;
					v+=2;
					out+=8;
				
			}
			// skip a yy line & out line
			   in+=w;
				out+=2*w;

     }

	__asm__ ("EMMS");
}
#endif

// Convert yv12 planar to yuv422 i

void YV12_422_C( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h)
{
uint8_t *y,*y2,*u,*v,*out2;
uint32_t dx,dy;

	out2=out+w*2;
	y=in;
	y2=in+w;
	u=in+w*h;
	v=in+((w*h*5)>>2);
	for(dy=h>>1;dy>0;dy--)
	{
		for(dx=w>>1;dx>0;dx--)
		{

			*(out+0)=*y++;
			*(out+1)=*v;
			*(out+2)=*y++;
			*(out+3)=*u;


			*(out2+0)=*y2++;
			*(out2+1)=*v++;	
			*(out2+2)=*y2++;
			*(out2+3)=*u++;	

			out2+=4;
			out+=4;
		}
		out+=w*2;
		out2+=w*2;
		y+=w;
		y2+=w;
		
	
	}
}
void YV12_422( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h)
{
	YV12_422_C(in,out,w,h);
}
