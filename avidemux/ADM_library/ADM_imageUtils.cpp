/***************************************************************************
    copyright            : (C) 2003-2005 by mean
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

#include "config.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ADM_assert.h"
#include "ADM_library/default.h"

#include "ADM_library/ADM_image.h"
#include "ADM_toolkit/ADM_cpuCap.h"
#include "admmangle.h"
static uint8_t tinyAverage(uint8_t *dst, uint8_t *src1, uint8_t *src2,uint32_t l)
{

uint8_t *s1,*s2,*d1;
int a1,a2;
        s1=src1;
        s2=src2;
        
        d1=dst;
          for(int y=0;y<l;y++)
                {
                        a1=*s1;
                        a2=*s2;
                        a1=a1+a2;
                        a1>>=1;
                        if(a1<0) a1=0;
                        if(a1>255) a1=255;
                        *d1=a1;                         

                        s1++;
                        s2++;
                        d1++;
                }
        
        return 1;
}
#if defined( ARCH_X86 ) || defined (ARCH_X86_64)
static uint8_t tinyAverageMMX(uint8_t *dst, uint8_t *src1, uint8_t *src2,uint32_t l)
{
int delta;
uint32_t ww,rr;
uint8_t *s1,*s2,*d1;
int a1,a2;
        s1=src1;
        s2=src2;
        
        d1=dst;
        ww=l>>2;
        rr=l&3;


         __asm__(
                         "pxor %%mm7,%%mm7"
                ::
                 );

          for(int y=0;y<ww;y++)
          {
                __asm__(
                        "movd           (%0),%%mm0 \n"
                        "movd           (%1),%%mm1 \n"
                        "punpcklbw      %%mm7,%%mm0 \n"
                        "punpcklbw      %%mm7,%%mm1 \n"
                        "paddw           %%mm1,%%mm0 \n"
                        "psrlw          $1,%%mm0 \n"
                        "packuswb       %%mm0,  %%mm0\n"
                        "movd           %%mm0,(%2) \n"

                : : "r" (s1),"r" (s2),"r"(d1)
                );
                        s1+=4;
                        s2+=4;
                        d1+=4;
                }
        __asm__(
                        "emms\n"
                ::
                );
        if(rr) tinyAverage(d1, s1, s2,rr);
        return 1;
}


#endif

uint8_t ADMImage::merge(ADMImage *src1,ADMImage *src2)
{
#if 1 && defined( ARCH_X86 ) || defined (ARCH_X86_64)
        if(CpuCaps::hasMMX())
        {
                tinyAverageMMX(YPLANE(this),YPLANE(src1),YPLANE(src2),(_width*_height*3)>>1);
                return 1;
        }
#endif
        tinyAverage(YPLANE(this),YPLANE(src1),YPLANE(src2),(_width*_height*3)>>1);
        return 1;


}
/* 3000 * 3000 max size, using uint32_t is safe... */
static uint32_t computeDiff(uint8_t  *s1,uint8_t *s2,uint32_t noise,uint32_t l)
{
uint32_t df=0;
uint32_t delta;
uint32_t ww,hh;


        for(int x=0;x<l;x++)
        {
                delta=abs(*s1-*s2);
                if(delta>noise)
                        df+=delta;
                s1++;
                s2++;

        }
        return df;
}
#if defined( ARCH_X86 ) || defined (ARCH_X86_64)
static uint32_t computeDiffMMX(uint8_t  *s1,uint8_t *s2,uint32_t noise,uint32_t l)
{
uint32_t df=0;
uint32_t delta;
uint32_t ll,rr;
uint64_t noise2=(uint64_t )noise;
static uint64_t noise64=noise2+(noise2<<16)+(noise2<<32)+(noise2<<48);
uint32_t result=0;
        ll=l>>2;
        rr=l&3;

         __asm__(
                         "pxor %%mm7,%%mm7\n"
                         "pxor %%mm3,%%mm3\n"
                         "movq (%0), %%mm6\n"
                :: "r" (noise64)
                 );

          for(int y=0;y<ll;y++)
          {
                __asm__(
                        "movd           (%0),  %%mm0 \n"
                        "movd           (%1),  %%mm1 \n"
                        "punpcklbw      %%mm7, %%mm0 \n"
                        "punpcklbw      %%mm7, %%mm1 \n"
                        "psubw          %%mm1, %%mm0 \n" // mm0=mm0 -mm1
                        "movq           %%mm7, %%mm5 \n" //0               
                        "pcmpgtw        %%mm0, %%mm5 \n" // Mask
                        "pxor           %%mm5, %%mm0 \n"
                        "psubw          %%mm5, %%mm0 \n" // abs (a-b)
        
                        "pcmpgtw        %%mm6, %%mm0 \n"
                        "pand           %%mm6, %%mm0 \n" // MM0 is the 4 diff, time to pack

                        "movq           %%mm0, %%mm1 \n"
                        "psrlq          $16,  %%mm1 \n"

                        "movq           %%mm0, %%mm2 \n"
                        "psrlq          $32,  %%mm2 \n"

                        "movq           %%mm0, %%mm4 \n"
                        "psrlq          $48,  %%mm2 \n"

                        "paddw          %%mm1, %%mm0 \n"
                        "paddw          %%mm2, %%mm4 \n"
                        "paddw          %%mm4, %%mm0 \n"

                        "psllq          $48,  %%mm0 \n"
                        "psrlq          $48,  %%mm0 \n"

                        "paddq          %%mm0, %%mm3 \n"

                : : "r" (s1),"r" (s2)
                );
                        s1+=4;
                        s2+=4;
                        
         }
        // Pack result
                __asm__(
                       
                        "movq           %%mm3,(%0)\n"
                        "emms\n"
                :: "r"(result)
                );
        if(rr) result+=computeDiff(s1, s2, noise,rr);
        return result;
}


#endif

uint32_t ADMImage::lumaDiff(ADMImage *src1,ADMImage *src2,uint32_t noise)
{
uint32_t r1,r2;
#if 0 && defined( ARCH_X86 ) || defined (ARCH_X86_64)
        if(CpuCaps::hasMMX())
        {
                r1= computeDiff(YPLANE(src1),YPLANE(src2),noise,src1->_width*src1->_height);
                r2= computeDiffMMX(YPLANE(src1),YPLANE(src2),noise,src1->_width*src1->_height);
                if(r1!=r2) ADM_assert(0);
                
        }
#endif
        return computeDiff(YPLANE(src1),YPLANE(src2),noise,src1->_width*src1->_height);
        


}

//EOF
