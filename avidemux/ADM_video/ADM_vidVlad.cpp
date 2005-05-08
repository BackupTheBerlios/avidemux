/***************************************************************************
                          ADM_vidVlad.cpp  -  description
                             -------------------
    begin                : Fri Jan 3 2003
    copyright            : (C) 2003 by mean
    email                : fixounet@free.fr
    
    Port from Vlad59 / Jim Casaburi TemporalCleaner from avisynth YV12
    
    Luma only
    
    
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
#if defined( ARCH_X86)  || defined(ARCH_X86_64)
//#define LOOP
#include "admmangle.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidVlad.h"
#include "ADM_filter/video_filters.h"




static  uint64_t _full_f ASM_CONST = 0xFFFFFFFFFFFFFFFFLL;
static  uint64_t _keep_right_luma ASM_CONST = 0x00FF00FF00FF00FFLL;
static  uint64_t _keep_left_luma ASM_CONST = 0xFF00FF00FF00FF00LL;

static volatile uint8_t *_l_source ;
static volatile uint8_t *_l_prev ;
static volatile uint8_t *_l_sprev;
static volatile uint8_t *_l_dest ;
static volatile uint8_t *_l_mask ;
static volatile uint64_t  _threshold;
static volatile long int _pitch_source;
static volatile long int w8;
static void ProcessCPlane(unsigned char *source, int pitch_source,
				   unsigned char *prev, 
				   unsigned char *save_prev, 
				   unsigned char* dest, 
				   unsigned char* mask, 
				   int width, int height,
				   uint64_t  threshold);
static void ProcessYPlane( unsigned char *source, long int pitch_source,
				    unsigned char *prev,
				    unsigned char *save_prev,
				    unsigned char* dest,
				    unsigned char* mask, 
				    long  int width, long int height,
				    uint64_t  threshold);				   
#define EXPAND(x) { x=x+(x<<8)+(x<<16)+(x<<24)+(x<<32)+(x<<40) \
										+(x<<48)+(x<<56);}
static FILTER_PARAM vladParam={2,{"ythresholdMask","cthresholdMask"}};


SCRIPT_CREATE(vladsmooth_script,AVDMVideoVlad,vladParam);
BUILD_CREATE(vladsmooth_create,AVDMVideoVlad);


char *AVDMVideoVlad::printConf(void)
{
	static char buf[50];
 	
	sprintf((char *)buf," Temporal Cleaner : Y: %02lu / c: %02lu",_param->ythresholdMask,
				_param->cthresholdMask	);
        return buf;
}


uint8_t AVDMVideoVlad::configure( AVDMGenericVideoStream *instream)
{
UNUSED_ARG(instream);
int i;
		i=_param->ythresholdMask;
	  if(GUI_getIntegerValue(&i,0,255,"Luma Temporal  Threshold"))
			{
					_param->ythresholdMask=i;
			}
	i=_param->cthresholdMask;			
	if(GUI_getIntegerValue(&i,0,255,"Chroma Temporal  Threshold"))
			{
					_param->cthresholdMask=i;
			}		
		 ythresholdMask = (uint64_t)_param->ythresholdMask;
	   cthresholdMask = (uint64_t)_param->cthresholdMask;	   

		EXPAND(	ythresholdMask);
		EXPAND(	cthresholdMask);					
		return 1;
}     											
AVDMVideoVlad::AVDMVideoVlad(  AVDMGenericVideoStream *in,CONFcouple *couples)
		

{
	_in=in;
	memcpy(&_info,_in->getInfo(),sizeof(_info));
	num_frame=0xFFFF0000;
	if(couples)
	{
		_param=NEW(VLAD_PARAM);
		GET(ythresholdMask);
		GET(cthresholdMask);
	}
	else
	{
		_param=NEW(VLAD_PARAM);
		ADM_assert(_param);
	  _param->ythresholdMask=5;
 	  _param->cthresholdMask=0;
   }
    	_mask=new uint8_t[_info.width*_info.height*3];
  	memset(_mask,0,	_info.width*_info.height*3);
	  
	   ythresholdMask=0;
	   ythresholdMask = (uint64_t)_param->ythresholdMask;
	   cthresholdMask = (uint64_t)_param->cthresholdMask;	   

		EXPAND(	ythresholdMask);
		EXPAND(	cthresholdMask);
	vidCache=new VideoCache(5,in);
}


uint8_t	AVDMVideoVlad::getCoupledConf( CONFcouple **couples)
{

			ADM_assert(_param);
			*couples=new CONFcouple(2);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(ythresholdMask);
	CSET(cthresholdMask);
	return 1;

}
AVDMVideoVlad::~AVDMVideoVlad()
{
		delete [] _mask;
		DELETE(_param);
		delete vidCache;
}  									

void ProcessYPlane( unsigned char *source, long int pitch_source,
				    unsigned char *prev,
				    unsigned char *save_prev,
				    unsigned char* dest,
				    unsigned char* mask, 
				    long  int width, long int height,
				    uint64_t  threshold)
{


	long int  h2;
			_l_source= source ;
			_l_prev= prev ;
			_l_sprev= save_prev ;
			_l_dest= dest ;
			_l_mask= mask ;
			_threshold=threshold;
			
			_pitch_source=pitch_source;
		

	w8 = (width >> 3);
//	w8 = -w8;
	h2 = (height >> 1);
#ifndef LOOP	
  w8=(height*width)>>3;
#else  
	for (int y = 0; y < height; y++)
	{
#endif		

//gcc doesn't find asm use of those vars
UNUSED_ARG(_full_f);
UNUSED_ARG(_keep_right_luma);
UNUSED_ARG(_keep_left_luma);
		
__asm__ __volatile__(
"StartASM1%=: \n\t"
"mov "Mangle(_l_source)","REG_si" \n\t"
"mov "Mangle(_l_dest)", "REG_di" \n\t"

"mov "Mangle(_l_prev)", "REG_ax" \n\t"
"mov "Mangle(_l_sprev)", "REG_bx" \n\t"
"mov "Mangle(_l_mask)", "REG_dx" \n\t"
"movq "Mangle(_threshold)",%%mm6 \n\t"
"pxor %%mm7,%%mm7 \n\t"
" \n\t"
"mov "Mangle(w8)", "REG_cx" \n\t"                // -width/8
"prefetchnta -128("REG_si") \n\t"        // preload cache
"prefetchnta -128("REG_ax") \n\t"
"HLine%=:  \n\t"
"movq ("REG_si","REG_cx",8),%%mm0 \n\t"     // mm0 <- lsource+(size/8)*8
"movq ("REG_ax","REG_cx",8),%%mm1 \n\t"     // mm1 <- lprev+(size/8)*8
"movq %%mm0,%%mm2 \n\t"               // mm2 <- mm0  source
"movq %%mm1,%%mm3 \n\t"               // mm3 <-mm1   oold
"psubusb %%mm1,%%mm0 \n\t"            // mm0=mm0-mm1
"psubusb %%mm2,%%mm1 \n\t"            // mm1=mm1-mm2
"por %%mm1,%%mm0 \n\t"                // mm0=mm0 or mm1
"pavgb %%mm2,%%mm3 \n\t"              // mm3= 'mm2+mm3"/2
" \n\t"                               // mm0=mm6-mm0 diff to threshold
"psubusb %%mm6,%%mm0 \n\t"            // >0 ?
"pcmpeqb %%mm7,%%mm0 \n\t"
"movq %%mm0,%%mm1 \n\t"               // maskded diff -> m1 
" \n\t"
"movq %%mm0,%%mm4 \n\t"               // masked diff >m4
"pand "Mangle(_keep_left_luma)",%%mm4 \n\t"     // left bytes m4
"psrlw $8,%%mm4 \n\t"                 // shift
"pand "Mangle(_keep_right_luma)",%%mm1 \n\t"    // right bytes
"pand %%mm1,%%mm4 \n\t"               // if right & left triggered
"packuswb %%mm4,%%mm4 \n\t"           // packed to 4 bytes
"movq %%mm0,%%mm1 \n\t"               // mm0 -> mm1 (invert diff to thresh)
"movd %%mm4,("REG_dx","REG_cx",4) \n\t"     // store mask m4->mask+ecx*4
" \n\t"
"pxor "Mangle(_full_f)",%%mm1 \n\t"             // iinvert m1 
"pand %%mm3,%%mm0 \n\t"               // mm0=old and mask diff
"pand %%mm2,%%mm1 \n\t"               // mm1= source and invert diff
"por %%mm1,%%mm0 \n\t"                // m0 = mix
"movntq %%mm0,("REG_di","REG_cx",8) \n\t"   // store to des+ecx*8
"movntq %%mm0,("REG_bx","REG_cx",8) \n\t"   // store to mask+ecx*8
"sub $1,"REG_cx" \n\t"                  // add 1 to ecv
"jnz HLine%= \n\t"                      // while !=0
#ifdef LOOP		
/*		_l_source += _pitch_source;
		_l_prev += _pitch_source;
		_l_sprev += _pitch_source;
		_l_dest += _pitch_source;
		_l_mask +=_pitch_source>>1;
		*/
"mov "Mangle(_l_source)","REG_si" \n\t"
"mov "Mangle(_l_prev)",  "REG_ax" \n\t"
"mov "Mangle(_l_sprev)", "REG_bx" \n\t"
"mov "Mangle(_l_dest)",  "REG_di" \n\t"
"mov "Mangle(_l_mask)",  "REG_dx" \n\t"
"mov "Mangle(_pitch_source)","REG_cx" \n\t"
"add  "REG_cx","REG_si" \n\t"
"add  "REG_cx","REG_ax" \n\t"
"add  "REG_cx","REG_bx" \n\t"
"add  "REG_cx","REG_di" \n\t"
"mov  "REG_si", "Mangle(_l_source)" \n\t"
"mov  "REG_ax", "Mangle(_l_prev)" \n\t"
"mov  "REG_bx", "Mangle(_l_sprev)" \n\t"
"mov  "REG_di", "Mangle(_l_dest)" \n\t"
"sar   $1,"REG_cx" \n\t"
"add  "REG_cx","REG_dx" \n\t"
"mov  "REG_dx","Mangle(_l_mask)"   \n\t"

#endif	
 : /* no output */
 :   
 : "esi", "edi", "eax",  "ebx", "ecx", "edx");
#ifdef LOOP
 }
#endif 
 __asm__ __volatile__("emms \n\t");
}

//#pragma -O0

void ProcessCPlane(unsigned char *source, int pitch_source,
				   unsigned char *prev, 
				   unsigned char *save_prev, 
				   unsigned char* dest, 
				   unsigned char* mask, 
				   int width, int height,
				   uint64_t  threshold)
{

	_l_source = source ;
	_l_prev = prev ;
	_l_sprev = save_prev ;
	_l_dest = dest ;
	_l_mask = mask ;
	_threshold=threshold;
	_pitch_source=pitch_source;
	w8 = (width >> 3);
//	w8 = -w8;
#ifdef LOOP
	for (int y = 0; y < height; y++)
	{
#else
   w8=w8*height;
#endif


__asm__ __volatile__ (
"mov "Mangle(_l_source)", "REG_si" \n\t"
"mov "Mangle(_l_dest)", "REG_di" \n\t"
"mov "Mangle(_l_prev)", "REG_ax" \n\t"
"mov "Mangle(_l_sprev)", "REG_bx" \n\t"
"mov "Mangle(_l_mask)", "REG_dx" \n\t"
"movq "Mangle(_threshold)",%%mm6 \n\t"
"pxor %%mm7,%%mm7 \n\t"
" \n\t"
"mov "Mangle(w8)", "REG_cx" \n\t"
"Lfoo%=:  \n\t"
"movq ("REG_si","REG_cx",8),%%mm0 \n\t"
"movq ("REG_ax","REG_cx",8),%%mm1 \n\t"
"movq %%mm0,%%mm2 \n\t"
"movq %%mm1,%%mm3 \n\t"
"psubusb %%mm1,%%mm0 \n\t"
"psubusb %%mm2,%%mm1 \n\t"
"por %%mm1,%%mm0 \n\t"
"pavgb %%mm2,%%mm3 \n\t"
" \n\t"
"psubusb %%mm6,%%mm0 \n\t"
"pcmpeqb %%mm7,%%mm0 \n\t"
" \n\t"
"movq ("REG_dx","REG_cx",8),%%mm4 \n\t"
"pand %%mm4,%%mm0 \n\t"
"movq %%mm0,%%mm1 \n\t"
" \n\t"
"pxor "Mangle(_full_f)",%%mm1 \n\t"
"pand %%mm3,%%mm0 \n\t"
"pand %%mm2,%%mm1 \n\t"
"por %%mm1,%%mm0 \n\t"
"movntq %%mm0,("REG_di","REG_cx",8) \n\t"
"movntq %%mm0,("REG_bx","REG_cx",8) \n\t"
"sub $1,"REG_cx" \n\t"
"jnz Lfoo%= \n\t"

#ifdef LOOP		
/*		_l_source += _pitch_source;
		_l_prev += _pitch_source;
		_l_sprev += _pitch_source;
		_l_dest += _pitch_source;
		_l_mask +=_pitch_source>>1;
		*/
"mov "Mangle(_l_source)","REG_si" \n\t"
"mov "Mangle(_l_prev)",  "REG_ax" \n\t"
"mov "Mangle(_l_sprev)", "REG_bx" \n\t"
"mov "Mangle(_l_dest)",  "REG_di" \n\t"
"mov "Mangle(_l_mask)",  "REG_dx" \n\t"
"mov "Mangle(_pitch_source)","REG_cx" \n\t"
"add  "REG_cx","REG_si" \n\t"
"add  "REG_cx","REG_ax" \n\t"
"add  "REG_cx","REG_bx" \n\t"
"add  "REG_cx","REG_di" \n\t"
"add  "REG_cx","REG_dx" \n\t"
"mov  "REG_si","Mangle( _l_source)" \n\t"
"mov  "REG_ax","Mangle( _l_prev)" \n\t"
"mov  "REG_bx","Mangle( _l_sprev)" \n\t"
"mov  "REG_di","Mangle( _l_dest)" \n\t"
"mov  "REG_dx","Mangle(_l_mask)"   \n\t"

#endif	
 : /* no output */
 :   
 : "esi", "edi", "eax",  "ebx", "ecx", "edx");
#ifdef LOOP
 }
#endif 
 __asm__ __volatile__("emms \n\t");
}
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
//$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
uint8_t AVDMVideoVlad::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
	
	uint32_t page=_info.width*_info.height;
	ADMImage *cur,*prev;
	
		if(frame>(_info.nb_frames-1)) return 0;
		
		*len=(page*3)>>1;
		
		cur=vidCache->getImage(frame);
		if(!cur) 
				return 0;
		data->copyInfo(cur);
		if(!frame)
		{
			
			data->duplicate(cur);
			vidCache->unlockAll();
			return 1  ;
		}
	
	 	prev=vidCache->getImage(frame-1);
		if(!prev)
		{
			vidCache->unlockAll();
			return 0  ;
		}
		
		
		
			  
		_threshold= ythresholdMask;
		ProcessYPlane (YPLANE(cur),
				_info.width,
				YPLANE(prev),     	
   				YPLANE(prev), 
				YPLANE(data), 
				_mask, 
				_info.width, 
		       		_info.height,
				ythresholdMask);
		if (0==_param->cthresholdMask)
		{
			//memcpy(data->data+page,_uncompressed->data+page,page>>1);
			memcpy(UPLANE(data),UPLANE(cur),page>>2);
			memcpy(VPLANE(data),VPLANE(cur),page>>2);
		}
		else
		{
			
			_threshold= cthresholdMask;
				ProcessCPlane (UPLANE(cur),
							_info.width>>1,
							UPLANE(prev),     	
   							UPLANE(prev),  
							UPLANE(data), 
							_mask, 
							_info.width>>1, 
							_info.height>>1,
							cthresholdMask);       
				
			
				ProcessCPlane (VPLANE(cur),
							_info.width>>1,
							VPLANE(prev),     	
   							VPLANE(prev),  
							VPLANE(data), 
							_mask, 
							_info.width>>1, 
							_info.height>>1,
							cthresholdMask);       				
			
			}
		
		vidCache->unlockAll();
		return 1;

}


#endif
