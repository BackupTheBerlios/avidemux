/***************************************************************************
                          yv12_rgb.cpp  -  description
                             -------------------

	Code borrowed from mpeg2dec and vlc


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


/*
 * yuv2rgb_mmx.c, Software YUV to RGB coverter with Intel MMX "technology"
 *
 * Copyright (C) 2000, Silicon Integrated System Corp.
 * All Rights Reserved.
 *
 * Author: Olie Lho <ollie@sis.com.tw>
 *
 * This file is part of mpeg2dec, a free MPEG-2 video decoder
 *
 * mpeg2dec is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * mpeg2dec is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Make; see the file COPYING. If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *

  2²21=													2097152
R = Y + 1.402V              2940207
G = Y - 0.344U - 0.714V     721420 1497366
B = Y + 1.772U              3716153


 */

#include <stdio.h>
#include <stdlib.h>

#include <inttypes.h>
#include "config.h"

#if defined( ARCH_X86)  || defined(ARCH_X86_64)

#include "admmangle.h"

/* hope these constant values are cache line aligned */
static volatile uint64_t mmx_80w = 0x0080008000800080LL;
static volatile uint64_t mmx_10w = 0x1010101010101010LL;
static volatile uint64_t mmx_00ffw = 0x00ff00ff00ff00ffLL;
static volatile uint64_t mmx_Y_coeff = 0x253f253f253f253fLL;

/* hope these constant values are cache line aligned */
static volatile uint64_t mmx_U_green = 0xf37df37df37df37dLL;
static volatile uint64_t mmx_U_blue = 0x4093409340934093LL;

static volatile uint64_t mmx_V_red =     0x3312331233123312LL;
static volatile uint64_t mmx_V_green = 0xe5fce5fce5fce5fcLL;

/* hope these constant values are cache line aligned */
static volatile uint64_t mmx_redmask =  0xf8f8f8f8f8f8f8f8LL;
static volatile uint64_t mmx_grnmask =  0xfcfcfcfcfcfcfcfcLL;
static volatile uint64_t mmx_bluemask = 0xf8f8f8f8f8f8f8f8LL;

static volatile uint64_t mmx_grnshift = 0x03;
static volatile uint64_t mmx_blueshift =  0x03;
void no_warning_2(void);

void no_warning_2(void)
{
	static uint64_t plop=0;

			plop+=  mmx_redmask +  mmx_grnmask +  mmx_bluemask+     mmx_grnshift+  mmx_blueshift;
			plop-=    mmx_U_green+  mmx_U_blue +    mmx_V_red +  mmx_V_green;

			plop+=  mmx_80w+mmx_10w + mmx_00ffw - mmx_Y_coeff ;

			mmx_80w=plop;
}

//static uint8_t buffer[7.0*5.7*8];

//#define HAVE_3DNOW

#if defined (HAVE_SSE) || defined (HAVE_3DNOW)
#define movntq "movntq" // use this for processors that have SSE or 3Dnow
#else
#define movntq "movq" // for MMX-only processors
#endif

 #define MMX_INIT_32 "                                                       \n\
movd      (%1), %%mm0       # Load 4 Cb       00 00 00 00 u3 u2 u1 u0       \n\
movl      $0, (%3)          # cache preload for image                       \n\
movd      (%2), %%mm1       # Load 4 Cr       00 00 00 00 v3 v2 v1 v0       \n\
pxor      %%mm4, %%mm4      # zero mm4                                      \n\
movq      (%0), %%mm6       # Load 8 Y        Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0       \n\
"

 /*
 * Do the multiply part of the conversion for even and odd pixels,
 * register usage:
 * mm0 -> Cblue, mm1 -> Cred, mm2 -> Cgreen even pixels,
 * mm3 -> Cblue, mm4 -> Cred, mm5 -> Cgreen odd  pixels,
 * mm6 -> Y even, mm7 -> Y odd
 */


#define MMX_YUV_MUL "                                                       \n\
# convert the chroma part                                                   \n\
punpcklbw %%mm4, %%mm0          # scatter 4 Cb    00 u3 00 u2 00 u1 00 u0   \n\
punpcklbw %%mm4, %%mm1          # scatter 4 Cr    00 v3 00 v2 00 v1 00 v0   \n\
psubsw    "Mangle(mmx_80w)", %%mm0        # Cb -= 128                                 \n\
psubsw    "Mangle(mmx_80w)", %%mm1        # Cr -= 128                                 \n\
psllw     $3, %%mm0             # Promote precision                         \n\
psllw     $3, %%mm1             # Promote precision                         \n\
movq      %%mm0, %%mm2          # Copy 4 Cb       00 u3 00 u2 00 u1 00 u0   \n\
movq      %%mm1, %%mm3          # Copy 4 Cr       00 v3 00 v2 00 v1 00 v0   \n\
pmulhw    "Mangle(mmx_U_green)", %%mm2    # Mul Cb with green coeff -> Cb green       \n\
pmulhw    "Mangle(mmx_V_green)", %%mm3    # Mul Cr with green coeff -> Cr green       \n\
pmulhw    "Mangle(mmx_U_blue)", %%mm0     # Mul Cb -> Cblue 00 b3 00 b2 00 b1 00 b0   \n\
pmulhw    "Mangle(mmx_V_red)", %%mm1      # Mul Cr -> Cred  00 r3 00 r2 00 r1 00 r0   \n\
paddsw    %%mm3, %%mm2          # Cb green + Cr green -> Cgreen             \n\
                                                                            \n\
# convert the luma part                                                     \n\
psubusb   "Mangle(mmx_10w)", %%mm6        # Y -= 16                                   \n\
movq      %%mm6, %%mm7          # Copy 8 Y        Y7 Y6 Y5 Y4 Y3 Y2 Y1 Y0   \n\
pand      "Mangle(mmx_00ffw)", %%mm6      # get Y even      00 Y6 00 Y4 00 Y2 00 Y0   \n\
psrlw     $8, %%mm7             # get Y odd       00 Y7 00 Y5 00 Y3 00 Y1   \n\
psllw     $3, %%mm6             # Promote precision                         \n\
psllw     $3, %%mm7             # Promote precision                         \n\
pmulhw    "Mangle(mmx_Y_coeff)", %%mm6    # Mul 4 Y even    00 y6 00 y4 00 y2 00 y0   \n\
pmulhw    "Mangle(mmx_Y_coeff)", %%mm7    # Mul 4 Y odd     00 y7 00 y5 00 y3 00 y1   \n\
"
/*
 * Do the addition part of the conversion for even and odd pixels,
 * register usage:
 * mm0 -> Cblue, mm1 -> Cred, mm2 -> Cgreen even pixels,
 * mm3 -> Cblue, mm4 -> Cred, mm5 -> Cgreen odd  pixels,
 * mm6 -> Y even, mm7 -> Y odd
 */


#define MMX_YUV_ADD "                                                       \n\
# Do horizontal and vertical scaling                                        \n\
movq      %%mm0, %%mm3          # Copy Cblue                                \n\
movq      %%mm1, %%mm4          # Copy Cred                                 \n\
movq      %%mm2, %%mm5          # Copy Cgreen                               \n\
paddsw    %%mm6, %%mm0          # Y even + Cblue  00 B6 00 B4 00 B2 00 B0   \n\
paddsw    %%mm7, %%mm3          # Y odd  + Cblue  00 B7 00 B5 00 B3 00 B1   \n\
paddsw    %%mm6, %%mm1          # Y even + Cred   00 R6 00 R4 00 R2 00 R0   \n\
paddsw    %%mm7, %%mm4          # Y odd  + Cred   00 R7 00 R5 00 R3 00 R1   \n\
paddsw    %%mm6, %%mm2          # Y even + Cgreen 00 G6 00 G4 00 G2 00 G0   \n\
paddsw    %%mm7, %%mm5          # Y odd  + Cgreen 00 G7 00 G5 00 G3 00 G1   \n\
                                                                            \n\
# Limit RGB even to 0..255                                                  \n\
packuswb  %%mm0, %%mm0          # B6 B4 B2 B0 / B6 B4 B2 B0                 \n\
packuswb  %%mm1, %%mm1          # R6 R4 R2 R0 / R6 R4 R2 R0                 \n\
packuswb  %%mm2, %%mm2          # G6 G4 G2 G0 / G6 G4 G2 G0                 \n\
                                                                            \n\
# Limit RGB odd to 0..255                                                   \n\
packuswb  %%mm3, %%mm3          # B7 B5 B3 B1 / B7 B5 B3 B1                 \n\
packuswb  %%mm4, %%mm4          # R7 R5 R3 R1 / R7 R5 R3 R1                 \n\
packuswb  %%mm5, %%mm5          # G7 G5 G3 G1 / G7 G5 G3 G1                 \n\
                                                                            \n\
# Interleave RGB even and odd                                               \n\
punpcklbw %%mm3, %%mm0          #                 B7 B6 B5 B4 B3 B2 B1 B0   \n\
punpcklbw %%mm4, %%mm1          #                 R7 R6 R5 R4 R3 R2 R1 R0   \n\
punpcklbw %%mm5, %%mm2          #                 G7 G6 G5 G4 G3 G2 G1 G0   \n\
"
 /*
 * convert RGB plane to RGB packed format,
 * mm0 -> B, mm1 -> R, mm2 -> G, mm3 -> 0,
 * mm4 -> GB, mm5 -> AR pixel 4-7,
 * mm6 -> GB, mm7 -> AR pixel 0-3
 */



#define MMX_UNPACK_32 "                                                     \n\
pxor      %%mm3, %%mm3  # zero mm3                                          \n\
movq      %%mm0, %%mm6  #                 B7 B6 B5 B4 B3 B2 B1 B0           \n\
movq      %%mm1, %%mm7  #                 R7 R6 R5 R4 R3 R2 R1 R0           \n\
movq      %%mm0, %%mm4  #                 B7 B6 B5 B4 B3 B2 B1 B0           \n\
movq      %%mm1, %%mm5  #                 R7 R6 R5 R4 R3 R2 R1 R0           \n\
punpcklbw %%mm2, %%mm6  #                 G3 B3 G2 B2 G1 B1 G0 B0           \n\
punpcklbw %%mm3, %%mm7  #                 00 R3 00 R2 00 R1 00 R0           \n\
punpcklwd %%mm7, %%mm6  #                 00 R1 B1 G1 00 R0 B0 G0           \n\
movq      %%mm6, (%3)   # Store ARGB1 ARGB0                                 \n\
movq      %%mm0, %%mm6  #                 B7 B6 B5 B4 B3 B2 B1 B0           \n\
punpcklbw %%mm2, %%mm6  #                 G3 B3 G2 B2 G1 B1 G0 B0           \n\
punpckhwd %%mm7, %%mm6  #                 00 R3 G3 B3 00 R2 B3 G2           \n\
#psllq	  $8,    %%mm6 	#		  R3 G3 B3 00 R2 B2 G2 00	    \n\
movq      %%mm6, 8(%3)  # Store ARGB3 ARGB2                                 \n\
punpckhbw %%mm2, %%mm4  #                 G7 B7 G6 B6 G5 B5 G4 B4           \n\
punpckhbw %%mm3, %%mm5  #                 00 R7 00 R6 00 R5 00 R4           \n\
punpcklwd %%mm5, %%mm4  #                 00 R5 B5 G5 00 R4 B4 G4           \n\
#psllq	  $8,    %%mm4 #		  R3 G3 B3 00 R2 B2 G2 00	    \n\
movq      %%mm4, 16(%3) # Store ARGB5 ARGB4                                 \n\
movq      %%mm0, %%mm4  #                 B7 B6 B5 B4 B3 B2 B1 B0           \n\
punpckhbw %%mm2, %%mm4  #                 G7 B7 G6 B6 G5 B5 G4 B4           \n\
punpckhwd %%mm5, %%mm4  #                 00 R7 G7 B7 00 R6 B6 G6           \n\
#psllq	  $8,    %%mm4	#		  R3 G3 B3 00 R2 B2 G2 00	    \n\
movq      %%mm4, 24(%3) # Store ARGB7 ARGB6                                 \n\
                                                                            \n\
"






//  slowYV12RGB(y, u, v, e, w);
//

void fastYV12RGB_mmx ( uint8_t * py,
			       uint8_t * pu, uint8_t * pv,  uint8_t * image,
			       int h_size)
{
    int i_x = 0; //, i_y = 0; //,even=1;
	volatile uint8_t *cy,*cu,*cv,*o; //,*line;

		cy=py;
		cu=pu;
		cv=pv;
		o=image;

        for ( i_x = h_size>>3; i_x>0;i_x-- )
        {

            __asm__( ".align 8"
						MMX_INIT_32
                     MMX_YUV_MUL
                     MMX_YUV_ADD
                     MMX_UNPACK_32
                     : : "r" (cy), "r" (cv), "r" (cu), "r" (o));
			  __asm__( ".align 8"
						MMX_INIT_32
                     MMX_YUV_MUL
                     MMX_YUV_ADD
                     MMX_UNPACK_32
                     : : "r" (cy+h_size), "r" (cv), "r" (cu), "r" (o+h_size*4));

            cy += 8;
            cu += 4;
            cv += 4;
            o += 32;
            }
	__asm__ ("EMMS");

	#if 0
	// now pack it
	uint8_t *in,*out;

	in=image;
	out=image;

	for(uint32_t i=h_size*2;i>0;i--)
		{
	*out++= *in++					;
	*out++= *in++	;
	*out++=*in++;						;
	in+=1;
			
					
					
		}
	#endif
}


#endif
