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
#ifdef USE_MMX
//#define LOOP
#include "admmangle.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidVlad.h"
#include "ADM_filter/video_filters.h"




static  uint64_t _full_f = 0xFFFFFFFFFFFFFFFFLL;
static  uint64_t _keep_right_luma = 0x00FF00FF00FF00FFLL;
static  uint64_t _keep_left_luma = 0xFF00FF00FF00FF00LL;

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


char 								*AVDMVideoVlad::printConf(void)
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
//	  _uncompressed=new uint8_t[_info.width*_info.height*3];
//    _prev=new uint8_t[_info.width*_info.height*3];   
	 _uncompressed=new ADMImage(_info.width,_info.height); 
	  _prev=new ADMImage(_info.width,_info.height);
	  
	  ADM_assert(_mask);ADM_assert(_uncompressed);ADM_assert(_prev);
	  memset(_mask,0,	_info.width*_info.height*3);
	  
	   ythresholdMask=0;
	   ythresholdMask = (uint64_t)_param->ythresholdMask;
	   cthresholdMask = (uint64_t)_param->cthresholdMask;	   

		EXPAND(	ythresholdMask);
		EXPAND(	cthresholdMask);												            
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
		delete [] _prev;
		delete [] _uncompressed;
		DELETE(_param);
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
"movl "Mangle(_l_source)",%%esi \n\t"
"movl "Mangle(_l_dest)", %%edi \n\t"

"movl "Mangle(_l_prev)", %%eax \n\t"
"movl "Mangle(_l_sprev)", %%ebx \n\t"
"movl "Mangle(_l_mask)", %%edx \n\t"
"movq "Mangle(_threshold)",%%mm6 \n\t"
"pxor %%mm7,%%mm7 \n\t"
" \n\t"
"movl "Mangle(w8)", %%ecx \n\t"                // -width/8
"prefetchnta -128(%%esi) \n\t"        // preload cache
"prefetchnta -128(%%eax) \n\t"
"HLine%=:  \n\t"
"movq (%%esi,%%ecx,8),%%mm0 \n\t"     // mm0 <- lsource+(size/8)*8
"movq (%%eax,%%ecx,8),%%mm1 \n\t"     // mm1 <- lprev+(size/8)*8
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
"movd %%mm4,(%%edx,%%ecx,4) \n\t"     // store mask m4->mask+ecx*4
" \n\t"
"pxor "Mangle(_full_f)",%%mm1 \n\t"             // iinvert m1 
"pand %%mm3,%%mm0 \n\t"               // mm0=old and mask diff
"pand %%mm2,%%mm1 \n\t"               // mm1= source and invert diff
"por %%mm1,%%mm0 \n\t"                // m0 = mix
"movntq %%mm0,(%%edi,%%ecx,8) \n\t"   // store to des+ecx*8
"movntq %%mm0,(%%ebx,%%ecx,8) \n\t"   // store to mask+ecx*8
"sub $1,%%ecx \n\t"                  // add 1 to ecv
"jnz HLine%= \n\t"                      // while !=0
#ifdef LOOP		
/*		_l_source += _pitch_source;
		_l_prev += _pitch_source;
		_l_sprev += _pitch_source;
		_l_dest += _pitch_source;
		_l_mask +=_pitch_source>>1;
		*/
"movl "Mangle(_l_source)",%%esi \n\t"
"movl "Mangle(_l_prev)",  %%eax \n\t"
"movl "Mangle(_l_sprev)", %%ebx \n\t"
"movl "Mangle(_l_dest)",  %%edi \n\t"
"movl "Mangle(_l_mask)",  %%edx \n\t"
"movl "Mangle(_pitch_source)",%%ecx \n\t"
"addl  %%ecx,%%esi \n\t"
"addl  %%ecx,%%eax \n\t"
"addl  %%ecx,%%ebx \n\t"
"addl  %%ecx,%%edi \n\t"
"movl  %%esi, "Mangle(_l_source)" \n\t"
"movl  %%eax, "Mangle(_l_prev)" \n\t"
"movl  %%ebx, "Mangle(_l_sprev)" \n\t"
"movl  %%edi, "Mangle(_l_dest)" \n\t"
"sar   $1,%%ecx \n\t"
"addl  %%ecx,%%edx \n\t"
"movl  %%edx,"Mangle(_l_mask)"   \n\t"

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
"movl "Mangle(_l_source)", %%esi \n\t"
"movl "Mangle(_l_dest)", %%edi \n\t"
"movl "Mangle(_l_prev)", %%eax \n\t"
"movl "Mangle(_l_sprev)", %%ebx \n\t"
"movl "Mangle(_l_mask)", %%edx \n\t"
"movq "Mangle(_threshold)",%%mm6 \n\t"
"pxor %%mm7,%%mm7 \n\t"
" \n\t"
"movl "Mangle(w8)", %%ecx \n\t"
"Lfoo%=:  \n\t"
"movq (%%esi,%%ecx,8),%%mm0 \n\t"
"movq (%%eax,%%ecx,8),%%mm1 \n\t"
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
"movq (%%edx,%%ecx,8),%%mm4 \n\t"
"pand %%mm4,%%mm0 \n\t"
"movq %%mm0,%%mm1 \n\t"
" \n\t"
"pxor "Mangle(_full_f)",%%mm1 \n\t"
"pand %%mm3,%%mm0 \n\t"
"pand %%mm2,%%mm1 \n\t"
"por %%mm1,%%mm0 \n\t"
"movntq %%mm0,(%%edi,%%ecx,8) \n\t"
"movntq %%mm0,(%%ebx,%%ecx,8) \n\t"
"subl $1,%%ecx \n\t"
"jnz Lfoo%= \n\t"

#ifdef LOOP		
/*		_l_source += _pitch_source;
		_l_prev += _pitch_source;
		_l_sprev += _pitch_source;
		_l_dest += _pitch_source;
		_l_mask +=_pitch_source>>1;
		*/
"movl "Mangle(_l_source)",%%esi \n\t"
"movl "Mangle(_l_prev)",  %%eax \n\t"
"movl "Mangle(_l_sprev)", %%ebx \n\t"
"movl "Mangle(_l_dest)",  %%edi \n\t"
"movl "Mangle(_l_mask)",  %%edx \n\t"
"movl "Mangle(_pitch_source)",%%ecx \n\t"
"addl  %%ecx,%%esi \n\t"
"addl  %%ecx,%%eax \n\t"
"addl  %%ecx,%%ebx \n\t"
"addl  %%ecx,%%edi \n\t"
"addl  %%ecx,%%edx \n\t"
"movl  %%esi,"Mangle( _l_source)" \n\t"
"movl  %%eax,"Mangle( _l_prev)" \n\t"
"movl  %%ebx,"Mangle( _l_sprev)" \n\t"
"movl  %%edi,"Mangle( _l_dest)" \n\t"
"movl  %%edx,"Mangle(_l_mask)"   \n\t"

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
	uint32_t n;
	uint32_t page=_info.width*_info.height;
	
		if(n>(_info.nb_frames-1)) return 0;
		
		*len=(page*3)>>1;
		
		if(!n)
			 {
				 if(!	_in->getFrameNumberNoAlloc(0,	len,_prev,flags)) return 0;
				 memcpy(data,_prev,(page*3)>>1);
				 return 1  ;
				}
	
	  if(n!=num_frame+1) // random access
	  {						
		if(!_in->getFrameNumberNoAlloc( n-1,len,_prev,flags)) return 0;
		num_frame=n-1;
		}
		
		if(!_in->getFrameNumberNoAlloc( n,len,_uncompressed,flags)) return 0;
		
		
			  
			_threshold= ythresholdMask;
			ProcessYPlane (_uncompressed->data,
								_info.width,
						   		_prev->data,     	
   						   		 _prev->data, 
				   				data->data, 
				   				_mask, 
				   				_info.width, 
						       		_info.height,
				   				ythresholdMask);
/*     
			printf("n y_threshold : %lx\n",ythresholdMask);
			printf("n c_threshold : %lx\n",cthresholdMask);
*/			
			if (0==_param->cthresholdMask)
			{
				memcpy(data->data+page,_uncompressed->data+page,page>>1);
			}
			else
			{
				uint32_t of=page;
				_threshold= cthresholdMask;
				ProcessCPlane (_uncompressed->data+of,
							_info.width>>1,
							_prev->data+of, 
   							_prev->data+of, 
							data->data+of, 
							_mask, 
							_info.width>>1, 
							_info.height>>1,
							cthresholdMask);       
				
				of=of+(of>>2);	       
				
				ProcessCPlane (_uncompressed->data+of,
							_info.width>>1,
							_prev->data+of, 
							_prev->data+of, 
							data->data+of, 
							_mask, 
							_info.width>>1, 
							_info.height>>1,
							cthresholdMask);
			
			}
		
		num_frame = n;		
		return 1;

}


#endif
