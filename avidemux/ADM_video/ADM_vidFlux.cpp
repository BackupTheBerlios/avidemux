/***************************************************************************
                          ADM_vidFlux.cpp  -  description
                             -------------------
    begin                : Tue Dec 31 2002
    copyright            : (C) 2002 by mean
    email                : fixounet@free.fr
    
    Ported from FluxSmooth
    (c)  Ross Thomas <ross@grinfinity.com>
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
#include "ADM_video/ADM_vidFlux.h"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM fluxParam={2,{"temporal_threshold","spatial_threshold"}};


SCRIPT_CREATE(fluxsmooth_script,ADMVideoFlux,fluxParam);

//#define ASM_FLUX
BUILD_CREATE(fluxsmooth_create,ADMVideoFlux);

static uint64_t  spat_thresh,temp_thresh;
static uint64_t  _l_counter_init,_l_indexer,_l_prev_pels,_l_next_pels;
static int _l_src_pitch;
static int _l_dst_pitch;
static int _l_xmax;

static int ycnt;
static int16_t scaletab[16];
static uint64_t scaletab_MMX[65536];
static	uint8_t * _l_currp; 
static	 uint8_t * _l_prevp;								  								  
static	 uint8_t * _l_nextp; 
static	 uint8_t * _l_destp; 

void initScaleTab( void )
{
//uint32_t i;

		scaletab[1] = 32767;
		for(int i = 2; i < 16; ++i)
				scaletab[i] = (int)(32768.0 / i + 0.5);
		for(uint32_t  i = 0; i < 65536; ++i)
		{
			scaletab_MMX[i] = ( (uint64_t)scaletab[ i        & 15]       ) |
							  (((uint64_t)scaletab[(i >>  4) & 15]) << 16) |
							  (((uint64_t)scaletab[(i >>  8) & 15]) << 32) |
							  (((uint64_t)scaletab[(i >> 12) & 15]) << 48);
		}
}
static uint32_t size;

ADMVideoFlux::ADMVideoFlux(AVDMGenericVideoStream *in,CONFcouple *couples)
			: ADMVideoCached(in,couples)
{
  
	
	if(couples)
	{
		_param=NEW( FLUX_PARAM );
		GET(temporal_threshold);
		GET(spatial_threshold);
	}
	else
	{
		 _param=NEW( FLUX_PARAM );
		 _param->spatial_threshold=7;
		 _param->temporal_threshold=7;
	}
  num_frame=0xffff0000;
}

uint8_t	ADMVideoFlux::getCoupledConf( CONFcouple **couples)
{

			assert(_param);
			*couples=new CONFcouple(2);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(temporal_threshold);
	CSET(spatial_threshold);
		return 1;

}
uint8_t ADMVideoFlux::configure(AVDMGenericVideoStream *in)
{
UNUSED_ARG(in);
int i;
		i=_param->temporal_threshold;
	  if(GUI_getIntegerValue(&i,0,255,"Temporal  Threshold"))
			{
					_param->temporal_threshold=(uint32_t)i;
			}
		i=_param->spatial_threshold;
	  if(GUI_getIntegerValue(&i,0,255,"Spatial  Threshold"))
			{
					_param->spatial_threshold=(uint32_t)i;
			}			
			return 1;
	
}
ADMVideoFlux::~ADMVideoFlux()
{
	DELETE(_param);
}

char	*ADMVideoFlux::printConf( void) 
{
	static char conf[100];

		sprintf(conf,"FluxSmooth: Spatial :%02lu Temporal:%02lu",
						_param->spatial_threshold,
						_param->temporal_threshold);
		return conf;
	
}
uint8_t ADMVideoFlux::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
          																	uint8_t *data,uint32_t *flags)
{
UNUSED_ARG(flags);
uint32_t dlen,dflags;
uint32_t plane=_info.width*_info.height;



			*len=(plane*3)>>1;
			
			size=(_info.width*_info.height*3)>>1;
			if(frame>_info.nb_frames-1) return 0;
			// check if it the 1st frame
			if(frame> _info.nb_frames-1) return 0;
			if(!frame || (frame==_info.nb_frames-1))
			{
				 if(!_in->getFrameNumberNoAlloc(frame, &dlen,_buffer[0],&dflags))
				 {
					 	return 0;
					}
				 _bufnum[0]=frame;
					memcpy(data,_buffer[0],size);
					return 1;
			}	    
			
			switch(fillCache( frame))
			{
				 case 0: return 0; break;
				 case 1: memcpy(data,_buffer[index_c],size); return 1; break;
				 case 2: break;
				 default:assert(0);
			}				 
				
//	printf("\n Action\n");		
		   			
// now we have everything
int dst_pitch = _info.width,
		src_pitch = _info.width,
		row_size  = _info.width,
		height    = _info.height;
		
 uint8_t   *currp = _buffer[index_c],
								*prevp = _buffer[index_p],
								*nextp = _buffer[index_n];
	uint8_t				*destp = data;

	// line 1 and last
	memcpy(destp, currp, row_size);
	memcpy(destp + dst_pitch * (height - 1),
		currp + src_pitch * (height - 1), row_size);

	// skip one line		
	currp += src_pitch;
	prevp += src_pitch;
	nextp += src_pitch;
	destp += dst_pitch;
	
	DoFilter_C(currp, prevp, nextp, src_pitch,
			destp, dst_pitch, row_size, height - 2);
	// U
 
	
	dst_pitch = _info.width>>1;
	src_pitch = _info.width>>1;
  row_size  = _info.width>>1;
	height = _info.height>>1;
	currp = _buffer[index_c]+plane;
	prevp = _buffer[index_p]+plane;
	nextp = _buffer[index_n]+plane;
	destp = data+plane;

	memcpy(destp, currp, row_size);
	memcpy(destp + dst_pitch * (height - 1),
		currp + src_pitch * (height - 1), row_size);

	currp += src_pitch;
	prevp += src_pitch;
	nextp += src_pitch;
	destp += dst_pitch;

		DoFilter_C(currp, prevp, nextp, src_pitch,
			destp, dst_pitch, row_size, height - 2);

	// V

  plane=(_info.width*_info.height*5)>>2;
	
	dst_pitch = _info.width>>1;
	src_pitch = _info.width>>1;
  row_size  = _info.width>>1;
	height = _info.height>>1;
	currp = _buffer[index_c]+plane;
	prevp = _buffer[index_p]+plane;
	nextp = _buffer[index_n]+plane;
	destp = data+plane;

	memcpy(destp, currp, row_size);
	memcpy(destp + dst_pitch * (height - 1),
		currp + src_pitch * (height - 1), row_size);

	currp += src_pitch;
	prevp += src_pitch;
	nextp += src_pitch;
	destp += dst_pitch;

	#if defined(USE_MMX) && defined(ASM_FLUX)
		DoFilter_MMX(currp, prevp, nextp, src_pitch,
			destp, dst_pitch, row_size, height - 2);
	
	#else
		DoFilter_C(currp, prevp, nextp, src_pitch,
			destp, dst_pitch, row_size, height - 2);
	#endif

	return 1;
}	                           


void ADMVideoFlux::DoFilter_C(
 uint8_t * currp, 
 uint8_t * prevp,								  								  
 uint8_t * nextp, 
 int src_pitch,
 uint8_t * destp, 
 int dst_pitch,
 int row_size, 
 int height)
{

	 int skip = src_pitch - row_size + 1,
		dskip = dst_pitch - row_size + 1;
	int ycnt = height;

	do
	{
		*destp = *currp; // Copy left edge

		++currp;
		++prevp;
		++nextp;
		++destp;

		int xcnt = row_size - 2;

		do
		{
			int pbt = *prevp++, b = *currp, nbt = *nextp++;
			int pdiff = pbt - b, ndiff = nbt - b;
			if((pdiff < 0 && ndiff < 0) || (pdiff > 0 && ndiff > 0))
			{
				int pb1 = currp[-src_pitch - 1], pb2 = currp[-src_pitch],
					pb3 = currp[-src_pitch + 1], b1 = currp[-1], b2 = currp[1],
					nb1 = currp[src_pitch - 1], nb2 = currp[src_pitch],
					nb3 = currp[src_pitch + 1], sum = b, cnt = 1;

				if(abs(pbt - b) <= _param->temporal_threshold)
				{
					sum += pbt;
					++cnt;
				}
				if(abs(nbt - b) <= _param->temporal_threshold)
				{
					sum += nbt;
					++cnt;
				}
				if(abs(pb1 - b) <= _param->spatial_threshold)
				{
					sum += pb1;
					++cnt;
				}
				if(abs(pb2 - b) <= _param->spatial_threshold)
				{
					sum += pb2;
					++cnt;
				}
				if(abs(pb3 - b) <= _param->spatial_threshold)
				{
					sum += pb3;
					++cnt;
				}
				if(abs(b1 - b) <= _param->spatial_threshold)
				{
					sum += b1;
					++cnt;
				}
				if(abs(b2 - b) <= _param->spatial_threshold)
				{
					sum += b2;
					++cnt;
				}
				if(abs(nb1 - b) <= _param->spatial_threshold)
				{
					sum += nb1;
					++cnt;
				}
				if(abs(nb2 - b) <= _param->spatial_threshold)
				{
					sum += nb2;
					++cnt;
				}
				if(abs(nb3 - b) <= _param->spatial_threshold)
				{
					sum += nb3;
					++cnt;
				}

				assert(sum >= 0);
				assert(sum < 2806);
				assert(cnt > 0);
				assert(cnt < 12);

				*destp++ = (uint8_t )(((sum * 2 + cnt) * scaletab[cnt]) >> 16);
				++currp;
			} else
				*destp++ = *currp++;
		} while(--xcnt);
		assert(xcnt == 0);

		*destp = *currp; // Copy right edge

		currp += skip;
		prevp += skip;
		nextp += skip;
		destp += dskip;
	} while(--ycnt);
	assert(ycnt == 0);

}
#ifdef USE_MMX
#include "admmangle.h"
/*
	__asm movq mm2, mm0 \
	__asm movq mm3, mm1 \
	__asm psubusw mm2, mm1 \
	__asm psubusw mm3, mm0 \
	__asm por mm2, mm3				/  mm2 = abs diff  / \
	__asm pcmpgtw mm2, threshold	/  Compare with threshold  / \
	__asm paddw mm6, mm2			/  -1 from counter if not within  / \
	__asm pandn mm2, mm1 \
	__asm paddw mm5, mm2			/  Add to sum  / \
*/
#define CHECK_AND_ADD(threshold) " movq %%mm0, %%mm2 \n\t" \
"movq    %%mm1, %%mm3 \n\t" \
"psubusw %%mm1, %%mm2 \n\t" \
"psubusw %%mm0, %%mm3 \n\t" \
"por     %%mm3, %%mm2				\n\t" /* mm2 = abs diff */ \
"pcmpgtw "Mangle(threshold)", %%mm2	\n\t "/* Compare with threshold */ \
"paddw   %%mm2, %%mm6	\n\t	"/* -1 from counter if not within */ \
"pandn   %%mm1, %%mm2 \n\t" \
"paddw   %%mm2, %%mm5	\n\t" /* Add to sum */ 

#define EXPAND(x) { x=x+(x<<8)+(x<<16)+(x<<24)+(x<<32)+(x<<40) \
										+(x<<48);}


void ADMVideoFlux::DoFilter_MMX(
uint8_t * currp, 
 uint8_t * prevp,								  								  
 uint8_t * nextp, 
 int src_pitch,
 uint8_t * destp, 
 int dst_pitch,
 int row_size, 
 int height)
{
	  _l_xmax = row_size - 4;
	 	ycnt 		= height;
	
	
	_l_currp = currp;
  _l_prevp = prevp;								  								  
  _l_nextp = nextp;  
  _l_destp = destp; 
 	_l_src_pitch =src_pitch;
  _l_dst_pitch =dst_pitch;
 
		_l_counter_init = 0x000b000b000b000bLL,
		_l_indexer = 0x1000010000100001LL;
		
		spat_thresh = _param->spatial_threshold;
		temp_thresh = _param->temporal_threshold;
		EXPAND( spat_thresh);
		EXPAND( temp_thresh);

UNUSED_ARG(_l_prev_pels);
UNUSED_ARG(_l_next_pels);
asm(
"								 movl (%0),%%esi \n\t"
"                movl "Mangle(_l_currp)", %%esi \n\t"
"                movl "Mangle(_l_destp)", %%edi \n\t"
"                pxor %%mm7,%%mm7 \n\t"
" \n\t"
"yloop:  \n\t"
"                # Copy first dword \n\t"
" \n\t"
"                movl (%%esi),%%eax \n\t"
"                movl %%eax,(%%edi) \n\t"
" \n\t"
"                movl $4,%%ecx \n\t"
" \n\t"
"xloop:  \n\t"
"                # Get current pels, init sum and counter \n\t"
" \n\t"
"                movd (%%esi,%%ecx),%%mm0 \n\t"
"                punpcklbw %%mm7,%%mm0 \n\t"
"                movq %%mm0,%%mm5 \n\t"
"                movq "Mangle(_l_counter_init)",%%mm6 \n\t"
" \n\t"
"                # Middle left \n\t"
" \n\t"
"                movq %%mm0,%%mm1 \n\t"
"                psllq $16,%%mm1 \n\t"
"                movd -4(%%esi,%%ecx),%%mm2 \n\t"
"                punpcklbw %%mm7,%%mm2 \n\t"
"                psrlq $48,%%mm2 \n\t"
"                por %%mm2,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Middle right \n\t"
" \n\t"
"                movq %%mm0,%%mm1 \n\t"
"                psrlq $16,%%mm1 \n\t"
"                movd 4(%%esi,%%ecx),%%mm2 \n\t"
"                punpcklbw %%mm7,%%mm2 \n\t"
"                psllq $48,%%mm2 \n\t"
"                por %%mm2,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Top left \n\t"
" \n\t"
"                movl %%esi,%%eax \n\t"
"                subl "Mangle(_l_src_pitch)", %%eax \n\t"
"                movd (%%eax,%%ecx),%%mm1 \n\t"
"                punpcklbw %%mm7,%%mm1 \n\t"
"                psllq $16,%%mm1 \n\t"
"                movd -4(%%eax,%%ecx),%%mm2 \n\t"
"                punpcklbw %%mm7,%%mm2 \n\t"
"                psrlq $48,%%mm2 \n\t"
"                por %%mm2,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Top centre \n\t"
" \n\t"
"                movd (%%eax,%%ecx),%%mm1 \n\t"
"                punpcklbw %%mm7,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Top right \n\t"
" \n\t"
"                psrlq $16,%%mm1 \n\t"
"                movd 4(%%eax,%%ecx),%%mm2 \n\t"
"                punpcklbw %%mm7,%%mm2 \n\t"
"                psllq $48,%%mm2 \n\t"
"                por %%mm2,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Bottom left \n\t"
" \n\t"
"                movl %%esi,%%eax \n\t"
"                addl "Mangle(_l_src_pitch)", %%eax \n\t"
"                movd (%%eax,%%ecx),%%mm1 \n\t"
"                punpcklbw %%mm7,%%mm1 \n\t"
"                psllq $16,%%mm1 \n\t"
"                movd -4(%%eax,%%ecx),%%mm2 \n\t"
"                punpcklbw %%mm7,%%mm2 \n\t"
"                psrlq $48,%%mm2 \n\t"
"                por %%mm2,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Bottom centre \n\t"
" \n\t"
"                movd (%%eax,%%ecx),%%mm1 \n\t"
"                punpcklbw %%mm7,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Bottom right \n\t"
" \n\t"
"                psrlq $16,%%mm1 \n\t"
"                movd 4(%%eax,%%ecx),%%mm2 \n\t"
"                punpcklbw %%mm7,%%mm2 \n\t"
"                psllq $48,%%mm2 \n\t"
"                por %%mm2,%%mm1 \n\t"
" \n\t"
CHECK_AND_ADD(spat_thresh)
" \n\t"
"                # Previous frame \n\t"
" \n\t"
"                movl "Mangle(_l_prevp)", %%eax \n\t"
"                movd (%%eax,%%ecx),%%mm1 \n\t"
"                punpcklbw %%mm7,%%mm1 \n\t"
"                movq %%mm1, "Mangle(_l_prev_pels)" \n\t"
" \n\t"
CHECK_AND_ADD(temp_thresh)
" \n\t"
"                # Next frame \n\t"
" \n\t"
"                movl "Mangle(_l_nextp)", %%eax \n\t"
"                movd (%%eax,%%ecx),%%mm1 \n\t"
"                punpcklbw %%mm7,%%mm1 \n\t"
"                movq %%mm1, "Mangle(_l_next_pels)" \n\t"
" \n\t"
CHECK_AND_ADD(temp_thresh)
" \n\t"
"                # Average \n\t"
" \n\t"
"                psllw $1,%%mm5                                  # sum *= 2 \n\t"
"                paddw %%mm6,%%mm5                               # sum += count \n\t"
" \n\t"
"                pmaddwd "Mangle(_l_indexer)",%%mm6                  # Make index into lookup \n\t"
"                movq %%mm6,%%mm1 \n\t"
"                punpckhdq %%mm6,%%mm6 \n\t"
"                movl "Mangle(scaletab_MMX)", %%eax \n\t"
"                paddd %%mm6,%%mm1 \n\t"
"                movd %%mm1,%%ebx \n\t"
" \n\t"
"            movq (%%eax,%%ebx,8),%%mm2          # Do lookup \n\t"
"            pmulhw %%mm2,%%mm5                                  # mm5 = average \n\t"
" \n\t"
"                # Apply smoothing only to fluctuating pels \n\t"
" \n\t"
"                movq %%mm0,%%mm1 \n\t"
"                movq "Mangle(_l_prev_pels)",%%mm2 \n\t"
"                movq %%mm0,%%mm3 \n\t"
"                movq "Mangle(_l_next_pels)",%%mm4 \n\t"
" \n\t"
"                pcmpgtw %%mm2,%%mm1                             # curr > prev \n\t"
"                pcmpgtw %%mm4,%%mm3                             # curr > next \n\t"
"                pcmpgtw %%mm0,%%mm2                             # prev > curr \n\t"
"                pcmpgtw %%mm0,%%mm4                             # next > curr \n\t"
" \n\t"
"                pand %%mm3,%%mm1                                # (curr > prev) and (curr > next) \n\t"
"                pand %%mm4,%%mm2                                # (prev > curr) and (next > curr) \n\t"
"                por %%mm2,%%mm1                                 # mm1 = FFh if fluctuating, else 00h \n\t"
" \n\t"
"                movq %%mm1,%%mm2 \n\t"
"                pand %%mm5,%%mm1                                # mm1 = smoothed pels \n\t"
"                pandn %%mm0,%%mm2                               # mm2 = unsmoothed pels \n\t"
"                por %%mm2,%%mm1                                 # mm1 = result \n\t"
" \n\t"
"                # Store \n\t"
" \n\t"
"                packuswb %%mm7,%%mm1 \n\t"
"                movntq %%mm1,(%%edi,%%ecx) \n\t"
" \n\t"
"                # Advance \n\t"
" \n\t"
"                addl $4,%%ecx \n\t"
"                cmpl "Mangle(_l_xmax)", %%ecx \n\t"
"                jl xloop \n\t"
" \n\t"
"                # Copy last dword \n\t"
" \n\t"
"                movl (%%esi,%%ecx),%%eax \n\t"
"                movl %%eax,(%%edi,%%ecx) \n\t"
" \n\t"
"                # Next row \n\t"
" \n\t"
"                addl "Mangle(_l_src_pitch)", %%esi \n\t"
"                movl "Mangle(_l_prevp)", %%eax \n\t"
"                addl "Mangle(_l_src_pitch)", %%eax \n\t"
"                movl %%eax, "Mangle(_l_prevp)" \n\t"
"                movl "Mangle(_l_nextp)", %%ebx \n\t"
"                addl "Mangle(_l_src_pitch)", %%ebx \n\t"
"                movl %%ebx, "Mangle(_l_nextp)" \n\t"
"                addl "Mangle(_l_dst_pitch)", %%edi \n\t"
" \n\t"
"                sub $1, "Mangle(ycnt)" \n\t"
"                jnz yloop \n\t"
" \n\t"
//"MISMATCH: "                sfence" \n\t"
"                emms \n\t"
" \n\t"

 : : "r"(_l_src_pitch) );
}
#endif
//
