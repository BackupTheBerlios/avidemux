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

#ifdef HAVE_ALTIVEC_H
#include "altivec.h"
#endif
#ifdef HAVE_ALTIVEC
static void YV12_422_Altivec( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h);
#endif

#ifdef USE_MMX
#include "../mpeg2enc/mmx.h"
static void YV12_422_mmx( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h);
#endif

static void YV12_422_C( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h);


void YV12_422( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h)
{
#ifdef HAVE_ALTIVEC 
	if( !(w&7))
		return YV12_422_Altivec(in,out,w,h);
#endif	
#if defined( USE_MMX) 
	if(!(w&3))
		return  YV12_422_mmx( in, out,  w, h);
#endif
	YV12_422_C(in,out,w,h);

}


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

/*
		0-> y1
		1-> y2
		2->u
		3->v
		4-> out
		5-> out 2

	Process 16 bytes = 4 pix from y1 and 4 pix from y2
*/

#ifdef USE_MMX
#define LOAD_M "                                                       \n\
# load y,u,v                                                 \n\
movd 		(%2), %%mm0    	 		 # load 4u       00000 u3 U2 U1 U0   \n\
movd 		(%3), %%mm1	    		 # load 4v       00000 V3 V2 V1 V0      \n\
punpcklbw   	%%mm0,%%mm1 			 # U2V2U1V1U0V0\n\
# now do y1 and y2 \n\
movd		(%0),  %%mm2 			 # 00 00 00 00 y3 y2 y1 y0 \n\
movd		(%1),  %%mm3 			 # 00 00 00 00 y3 y2 y1 y0     \n\
punpcklbw   	%%mm1, %%mm2 			 # 00 y3 00 y2 00 y1 00 y0 \n\
punpcklbw   	%%mm1, %%mm3 			 # 00 y3 00 y2 00 y1 00 y0 \n\
movq      	%%mm2, (%4) 			 # store \n\
movq      	%%mm3, (%5) 			 # store     \n\
"
void YV12_422_mmx( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h)
{

	uint8_t *u,*v,*out2;
	
	
		u=in+w*h;
		v=u+(w*h>>2);
		out2=out+2*w;

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


#ifdef HAVE_ALTIVEC 

#define LOAD_ALIGN(dest,src) \
		MSQ = vec_ld(0, src); \
		mask = vec_lvsl(0, src); \
		dest= vec_perm(MSQ, MSQ, mask); 


void YV12_422_Altivec( uint8_t *in, uint8_t *out, uint32_t w,uint32_t h)
{
uint8_t *y,*y2,*u,*v,*out2;
uint32_t dx,dy;
vector unsigned char vecy,vecy2,vecu,vecv,MSQ,mask;
vector unsigned char vecout,vecout2;
vector unsigned char zero;

#define VEC16 vector unsigned short
#define VEC8 vector unsigned char
	out2=out+w*2;
	y=in;
	y2=in+w;
	u=in+w*h;
	v=in+((w*h*5)>>2);
	zero=vec_splat_u8(0);
	for(dy=h>>1;dy>0;dy--)
	{
		// We do 4 pix in a raw
		for(dx=w>>3;dx>0;dx--)
		{
			LOAD_ALIGN(vecy,y); // expand
			LOAD_ALIGN(vecy2,y); // expand
			LOAD_ALIGN(vecu,u); // expand
			LOAD_ALIGN(vecv,u); // expand
			
			vecu=(VEC8)vec_mergeh(vecu,vecv);
			vecy=(VEC8)vec_mergeh(vecy,vecu);
			vecy2=(VEC8)vec_mergeh(vecy2,vecu);
			// Store
			vec_st(vecy,0,out); 
			vec_st(vecy2,0,out2); 
			// next
			out2+=16;
			out+=16;
			y+=8;
			y2+=8;
			u+=4;
			v+=4;
		}
		out+=w*2;
		out2+=w*2;
		y+=w;
		y2+=w;
		
	
	}
}


#endif
