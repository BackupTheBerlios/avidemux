/***************************************************************************
                          yv2bmp.cpp  -  description
                             -------------------

Convert yv12 to RGB, altivec optimized
this is my first altivec simd, probably less than optimal


    2²21=2097152
R = Y + 1.402V              2940207
G = Y - 0.344U - 0.714V     721420 1497366
B = Y + 1.772U              3716153
S
    begin                : Wed March 13 2004
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr
	
- correct Y by - 16
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <ADM_assert.h>
#include <string.h>
#include "default.h"
#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME 0
#include "ADM_toolkit/ADM_debug.h"


#if defined(HAVE_ALTIVEC) && defined(CONFIG_DARWIN)
#ifdef HAVE_ALTIVEC_H
#include "altivec.h"
#endif
// Protoyping


#define MULY 256
#define MULRU 359
#define MULBV 454
#define MULGu -88
#define MULGV -183

#define vecbyte vector unsigned char
#define vecshort vector unsigned short

uint8_t altivecYV12RGB(uint8_t * ptr_y,
		    uint8_t * ptr_u,
		    uint8_t * ptr_v, uint8_t * ptr2, uint32_t w);

// A pack of constant vectors we will use later
// we more or less rely on gcc to have good register allocation
//________________________________________________________________
 const  vector signed short multy=(vector signed short)
		(MULY,MULY,MULY,MULY, MULY,MULY,MULY,MULY);
const  vector signed short multRu=(vector signed short)(MULRU,0,MULRU,0, MULRU,0,MULRU,0);
const  vector signed short multBv=(vector signed short)(MULBV,0,MULBV,0, MULBV,0,MULBV,0);
const  vector signed short multGu=(vector signed short)(MULGu,0,MULGu,0, MULGu,0,MULGu,0);
const  vector signed short multGv=(vector signed short)(MULGV,0,MULGV,0, MULGV,0,MULGV,0);
const  vector signed short conv2Signed=(vector signed short)(-128,-128,-128,-128,-128,-128,-128,-128);	
const  vector unsigned char conv16=(vector unsigned char)(16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16);	


const vecbyte maskR=(vecbyte)(2,0x12,6,0x16,10,0x1a,14,0x1e,  0,0,0,0,0,0,0,0);
#define MXF 0xff00
const vector signed int maxFF=(vector signed int)(MXF,MXF,MXF,MXF);

//____________________________________
//	We handle 8 pixels at a time
//	My first altivec code :)
//____________________________________
uint8_t altivecYV12RGB(uint8_t * ptr_y,
		    uint8_t * ptr_u,
		    uint8_t * ptr_v, uint8_t * ptr2, uint32_t w)
{

	vector unsigned char y,y2,u,v;
	vector signed short sy16,su16,sv16,sz16;
	vector signed int sy16e,sy16o,su32,sv32,sz16e,sz16o;
	vector unsigned char srcy,srcy2,srcu,srcv;
	
	
	vector unsigned char MSQ, LSQ, mask,neg1,zero,align;
	vector unsigned char OUT,OUT2;
	vector signed int Ru,Gu,Gv,Bu;
	vector signed int Ra,Ga,Ba;
	
	vector signed int R0,G0,B0;
	vector signed int R1,G1,B1;
	vector unsigned char nullVect;
	vecbyte R,G,B;
	
	
	uint8_t *ptr3=ptr2+w*4;

	#if 0	
#define LOAD_ALIGN(dest,src) \
		MSQ = vec_ld(0, src); \
		LSQ = vec_ld(16, src); \
		mask = vec_lvsl(0, src); \
		dest= vec_perm(MSQ, LSQ, mask); 
#endif
#define LOAD_ALIGN(dest,src) \
		MSQ = vec_ld(0, src); \
		mask = vec_lvsl(0, src); \
		dest= vec_perm(MSQ, MSQ, mask); 

		
		
	// We do 8 pixels at a time
	int count=w;
	count>>=3;
	
	while(count)
	{
		aprintf("_________________________________\n");
		//srcy2=(vector unsigned char )ptr_y2;
		// expand y to signed int
		// Realign if needed
		LOAD_ALIGN(srcy,ptr_y);
		LOAD_ALIGN(srcy2,ptr_y+w);
		
		srcy=vec_subs(srcy,conv16);
		srcy2=vec_subs(srcy2,conv16);
		
		nullVect=vec_splat_u8(0);
		sy16=(vector signed short)vec_mergeh(nullVect,srcy);	
		sz16=(vector signed short)vec_mergeh(nullVect,srcy2);	
		aprintf("sy16:%vd\n",sy16);
		// multiply by scaling factor for y
		sy16e=vec_mule(sy16,multy);
		sy16o=vec_mulo(sy16,multy);
		
		sz16e=vec_mule(sz16,multy);
		sz16o=vec_mulo(sz16,multy);
		
		
		aprintf("sy16e:%vld\n",sy16e);
		aprintf("sy16o:%vld\n",sy16o);
		//--------- get u, convert to long
		LOAD_ALIGN(srcu,ptr_u);
		aprintf("su8:%vd\n",srcu);
		#if 0
		su16=(vector signed short )vec_perm(srcu,nullVect,permC);	
		su16=vec_add(su16,conv2Signed);
		#else
		srcu=(vecbyte)vec_mergeh(nullVect,srcu);
		su16=(vector signed short)vec_mergeh( (vector signed short)srcu,(vector signed short)nullVect);
		su16=vec_add(su16,conv2Signed);
		#endif
		aprintf("su16:%vd\n",su16);
		//--------- get v, convert to long
		LOAD_ALIGN(srcv,ptr_v);
		#if 0
		sv16=(vector signed short )vec_perm(srcv,nullVect,permC);	
		sv16=vec_add(sv16,conv2Signed);
		#else
		srcv=(vecbyte)vec_mergeh(nullVect,srcv);
		sv16=(vector signed short)vec_mergeh((vecshort)srcv,(vecshort)nullVect);
		sv16=vec_add(sv16,conv2Signed);
		#endif
		aprintf("sv32:%vd\n",sv16);

		// multiply to get Ru'
		Ra=vec_mule(su16,multRu);
		aprintf("Ru:%vd\n",multRu);
		aprintf("Ra:%vld\n",Ra);
		// multiply to get Gu'
		Gu=vec_mule(su16,multGu);
		Gv=vec_mule(sv16,multGv);
		Ga=vec_add(Gu,Gv);		

		aprintf("Gu:%vld\n",Gu);
		aprintf("Gv:%vld\n",Gv);
		aprintf("Ga:%vld\n",Ga);
		// multiply to get Bv'
		Ba=vec_mule(sv16,multBv);
		aprintf("Bv:%vd\n",multBv);
		aprintf("Ba:%vld\n",Ba);

		// so now we add Y to Ra to get R, Y+Ga=G, Y+Ba=B
		R0=vec_add(sy16e,Ra);
		R1=vec_add(sy16o,Ra);
		G0=vec_add(sy16e,Ga);
		G1=vec_add(sy16o,Ga);
		B0=vec_add(sy16e,Ba);
		B1=vec_add(sy16o,Ba);

		
		
		aprintf("R0:%vld\n",R0);
		aprintf("R0b:%vd\n",(vecbyte)R0);
		aprintf("R1:%vld\n",R1);
		aprintf("R1b:%vd\n",(vecbyte)R1);
		
		aprintf("B0:%vld\n",B0);
		aprintf("B1:%vld\n",B1);
				
		
		// now time to pack the result		
		// R0 G0 B0 R1 G1 B1 for each pack
		// Merge 0/1 to get packed R G B
		
		

		// Interleav R0 and G0
		// We done do clipping (yet)
		// Saturate
		#define SAT(x) \
			x=vec_max(x,(vector signed int)nullVect); \
			x=vec_min(x,(vector signed int)maxFF);
		SAT(R0);
		SAT(R1);
		SAT(G0);
		SAT(G1);
		SAT(B0);
		SAT(B1);
		
		
		
		R=vec_perm((vecbyte)R0,(vecbyte)R1,maskR);
		G=vec_perm((vecbyte)G0,(vecbyte)G1,maskR);
		B=vec_perm((vecbyte)B0,(vecbyte)B1,maskR);
		
		aprintf("R:%vd\n",R);
		aprintf("G:%vd\n",G);
		aprintf("B:%vd\n",B);
				
		// aligned write (?)
		
		vecbyte merge0,merge1;
		vecshort merge2;
#define MERGE_STORE_RGB(x) \
		merge1=vec_mergeh(R,nullVect); \
		merge0=vec_mergeh(B,G); \
		merge2=vec_mergeh( (vecshort) merge0,(vecshort )merge1); \
		vec_st((vecbyte)merge2,0,x); \
		merge2=vec_mergel( (vecshort) merge0,(vecshort )merge1); \
		vec_st((vecbyte)merge2,16,x);


		MERGE_STORE_RGB(ptr2);	
		// and do y2________________________________
		
		R0=vec_add(sz16e,Ra);
		R1=vec_add(sz16o,Ra);
		G0=vec_add(sz16e,Ga);
		G1=vec_add(sz16o,Ga);
		B0=vec_add(sz16e,Ba);
		B1=vec_add(sz16o,Ba);

		SAT(R0);
		SAT(R1);
		SAT(G0);
		SAT(G1);
		SAT(B0);
		SAT(B1);

		R=vec_perm((vecbyte)R0,(vecbyte)R1,maskR);
		G=vec_perm((vecbyte)G0,(vecbyte)G1,maskR);
		B=vec_perm((vecbyte)B0,(vecbyte)B1,maskR);
		
		MERGE_STORE_RGB(ptr3);	
		//
		count--;
		ptr_y+=8;
		ptr_u+=4;
		ptr_v+=4;
		ptr2+=32;	
		ptr3+=32;

	}
	
	return 1;
}
//

#endif
