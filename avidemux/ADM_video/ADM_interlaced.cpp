/***************************************************************************
                          ADM_interlaced.cpp  -  description
                             -------------------

	This piece of code count how many interlaced part we detected in the frame
	The method is from smartdeinterlacer
	Works only on luma (as chroma is 2:2 downsampled)

    begin                : Wed Dec 18 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr

Benchmark:
		ASM 3-version 	: 20 ms
		C Version 		: 25 ms
Skip factor=2

		ASM 3-version 	: 7 ms
		C Version 		: 8 ms

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
#include <assert.h>
#include <assert.h>
#include <math.h>
#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
 #include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidCommonFilter.h"

//#define MMX_TRACE
#define ASM_ILACING


#include "mmx_macros.h"

#include "admmangle.h"


#define SKIP_FACTOR 2   // 2^SKIPFACTOR=SKIP_LINEAR+1
#define SKIP_LINEAR   3

 #define MATCH_THRESH (60*60)
 #define QMATCH_THRESH ((uint64_t)(MATCH_THRESH>>2))


/*
	This macro compute the number of pixel where c-p * c-n > threshold and add the result to mm7
	There will be an overflow if more than 50 % of the video is like that but we can safely assume
		that would be rare

		mm0 contains 4 bytes of c
		mm1 contains 4 bytes of p
		mm2 contains 4 bytes of n
*/


#if defined( USE_MMX) &&  defined(ASM_ILACING)

static uint8_t *_l_p=NULL;
static uint8_t *_l_c=NULL;
static uint8_t *_l_n=NULL;

static int64_t _l_h;

static mmx_t _lthresh,_added,_total,_lwrd;
static mmx_t _l255;
static mmx_t _l0;

#define COMPUTE \
punpcklbw_r2r(mm5,mm0);  /*c  expand 4 bytes -> 4 word */ \
punpcklbw_r2r(mm5,mm1);  /*p*/ \
punpcklbw_r2r(mm5,mm2); /* n*/ \
movq_r2r(mm0,mm3);		/* mm3 also c*/ \
psubw_r2r(mm1,mm0) ; /* mm0=mm0-mm1 =  c-p*/ \
psubw_r2r(mm2,mm3) ; /* mm3=mm3-mm2 =  c-n*/ \
psraw_i2r(1,mm0); /* to protect from overflow*/ \
psraw_i2r(1,mm3);\
pmullw_r2r(mm0,mm3); /* mm3=(c-p)*(c-n) / 4;*/ \
pcmpgtw_r2r(mm4,mm3); /* keep only > size*/ \
psrlw_i2r(15,mm3); 	/* keep only one bit*/ \
pmaddwd_r2r(mm3,mm3);\
movq_r2r(mm3,mm0);\
psrlq_i2r(32,mm0);\
paddw_r2r(mm3,mm0); /* we got the total of match in mm0*/ \
pand_r2r(mm6,mm0); /* and we keep the last word*/ \
paddd_r2r(mm0,mm7);


#endif
/*
	Returns the # of interlacing effects on 1/4 of the image
*/
#if defined( USE_MMX) &&  defined(ASM_ILACING)  && !defined(MMX_TRACE)
uint32_t      ADMVideo_interlaceCount( uint8_t *src ,uint32_t w, uint32_t h)
{
uint32_t m=0,y,x;

_total.uq=0LL;
//_l255.uq=0xffffffffffffffffLL;
//_l0.uq=0LL;
_lwrd.uq=0xffffLL;
uint32_t stride=w*SKIP_LINEAR;
	_l_p=src;
	_l_c=src+w;
	_l_n=src+w+w;
	_l_h=w;
	_lthresh.uq=QMATCH_THRESH+(QMATCH_THRESH<<16)+
				(QMATCH_THRESH<<32)+(QMATCH_THRESH<<48);

movq_m2r(_lwrd,mm6);
movq_m2r(_lthresh,mm4);
pxor_r2r(mm5,mm5);
pxor_r2r(mm7,mm7);

	for(y=h>>SKIP_FACTOR;  y >2 ; y--)
		{
			__asm__ __volatile__(
				"mov "Mangle(_l_c)",	%%eax\n\t"
				"mov "Mangle(_l_p)",	%%ebx\n\t"
				"mov "Mangle(_l_n)",	%%ecx\n\t"
				"mov "Mangle(_l_h)",	%%edx\n\t"
				"admlloopp%=: \n\t"
				"movd (%%eax),	%%mm0\n\t"
				"movd (%%ebx),	%%mm1\n\t"
				"movd (%%ecx),	%%mm2\n\t"
				:
				:
				: "eax", "ebx", "ecx", "edx"
				);
				COMPUTE

			__asm__ __volatile__(
				"addl $4,		%%eax\n\t"
				"addl $4	,	%%ebx\n\t"
				"addl $4,		%%ecx\n\t"
				"subl $1,		%%edx\n\t"
				"jnz 		admlloopp%=\n\t"
				:
				:
				: "eax", "ebx", "ecx", "edx"
				);


				_l_p+=(1+SKIP_LINEAR)*w;
				_l_c+=(1+SKIP_LINEAR)*w;
				_l_n+=(1+SKIP_LINEAR)*w;


		}
		// retrieve mm7 containg the total

		__asm__ __volatile__(
		"mov 	$"Mangle(_total)",		%%eax\n\t"
		"movd 	%%mm7,	(%%eax)\n\t"
		"emms\n\t"
		:
		:
		: "eax"
		);
		m=(uint32_t)_total.uw[0];

                 return m;
}
#endif
#if defined( USE_MMX) &&  defined(ASM_ILACING)  && defined(MMX_TRACE)
uint32_t      ADMVideo_interlaceCount( uint8_t *src ,uint32_t w, uint32_t h)
{
uint32_t m=0,y,x;
	printf("\n***TRACE***\n");
_total.uq=0LL;
//_l255.uq=0xffffffffffffffffLL;
//_l0.uq=0LL;
_lwrd.uq=0xffffLL;
uint32_t stride=w*SKIP_LINEAR;
	_l_p=src;
	_l_c=src+w;
	_l_n=src+w+w;
	_l_h=w;
	_lthresh.uq=QMATCH_THRESH+(QMATCH_THRESH<<16)+
				(QMATCH_THRESH<<32)+(QMATCH_THRESH<<48);

movq_m2r(_lwrd,mm6);
pxor_r2r(mm5,mm5);
pxor_r2r(mm7,mm7);

	for(y=h>>SKIP_FACTOR;  y >2 ; y--)
		{
			for(x=w>>2;x>0;x--)
			{
			__asm__ __volatile__(
				"mov "Mangle(_l_c)",	%%eax\n\t"
				"mov "Mangle(_l_p)",	%%ebx\n\t"
				"mov "Mangle(_l_n)",	%%ecx\n\t"
				"mov "Mangle(_l_h)",	%%edx\n\t"
				"movd (%%eax),	%%mm0\n\t"
				"movd (%%ebx),	%%mm1\n\t"
				"movd (%%ecx),	%%mm2\n\t"
				:
				:
				: "eax", "ebx", "ecx", "edx"
				);
				COMPUTE
				_l_c+=4;
				_l_p+=4;
				_l_n+=4;

			}

				_l_p+=(1+SKIP_LINEAR)*w;
				_l_c+=(1+SKIP_LINEAR)*w;
				_l_n+=(1+SKIP_LINEAR)*w;


		}
		// retrieve mm7 containg the total

		__asm__ __volatile__(
		"mov 	$"Mangle(_total)",		%%eax\n\t"
		"movd 	%%mm7,	(%%eax)\n\t"
		"emms\n\t"
		:
		:
		: "eax"
		);
		m=(uint32_t)_total.uw[0];
		printf("m: %lu\n",m);
                 return m;
}

#endif
#if !defined(USE_MMX) || !defined(ASM_ILACING)
// C version
uint32_t      ADMVideo_interlaceCount( uint8_t *src ,uint32_t w, uint32_t h)
{
uint32_t m=0,y,x;

			uint8_t *p,*n,*c;
			int j;
			c=src + w;
			n=src + w+w;
			p=src ;


			for(y=h>>SKIP_FACTOR;  y >2 ; y--)
					{
           				for(x=w;x>0;x--)
								{
								j=(int)*p-(int)*c;
								j*=(int)*n-(int)*c;
							//	printf("p %d c %d  n %d j %d\n",*p,*c,*n , j);
                             					if(  j >MATCH_THRESH)
								{
								m++;
							//	*c=*p=*n=0xff;
								}

										p++;c++;n++;
								}
								p+=(SKIP_LINEAR)*w;
								c+=(SKIP_LINEAR)*w;
								n+=(SKIP_LINEAR)*w;

					}

                 return m;
}
#endif

