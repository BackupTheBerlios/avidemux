/***************************************************************************
                          ADM_vidSRT.cpp  -  description
                             -------------------
    begin                : Thu Dec 12 2002
    copyright            : (C) 2002 by mean
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <math.h>
#include <iconv.h>


#ifdef USE_FREETYPE

#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_video/ADM_vidFont.h"
#include "ADM_video/ADM_vidSRT.h"
#include <ADM_assert.h>

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"


#define SUB_OUT 0xf0000000

#define ALPHA

#define LUMA_LEVEL 10

#define SAFE_STRCPY(dest,destLen,src,srcLen) \
memcpy(dest,src,srcLen<destLen?srcLen:destLen-1); \
dest[srcLen<destLen?srcLen:destLen-1]='\0'; \


//                     1
//		Get in range in 121 + coeff matrix
//                     1
//
// If the value is too far away we ignore it
// else we blend
//__________________________________________________________________
uint8_t ADMVideoSubtitle::getFrameNumberNoAlloc(uint32_t frame,
						uint32_t *len,
   						ADMImage *data,
   						uint32_t *flags)
{

double f;
uint32_t time,srch;
uint32_t absFrame=frame+_info.orgFrame;

	//		printf("\n Stabilize : %lu\n",frame);
          _uncompressed=data;
	if(frame>=_info.nb_frames) 
	{
		printf("Filter : out of bound!\n");
		return 0;
	}
	if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;

	// if no subtitle return...
	if(!_subs) return 1;

	// compute time stamp
	      f=absFrame*1000000./_info.fps1000;
	      time=(uint32_t) floor(f);
	      // Do we have it already ?

	      if(time<=_subs[_oldline].endTime && time >=_subs[_oldline].startTime)
	      {
	      		aprintf("Sub: cached %lu %lu %lu\n",time,_subs[_oldline].startTime,_subs[_oldline].endTime);
			blend(YPLANE(data),_conf->_baseLine); // re-use it
			return 1;	

	      }
	      srch=search(time);
	      // we got the right one
     
	      if(srch!=SUB_OUT) // we got something...
			{
					_oldline=srch;
					aprintf("Sub: Using line %d\n",srch);
					//printf("\n %s \n",_subs[_oldline].string);
					displayString(&(_subs[_oldline]));
					blend(YPLANE(data),_conf->_baseLine);
			}

		return 1;

}
//_________________________________________________________________
uint32_t	ADMVideoSubtitle::search(uint32_t time)
{
	if(time<_subs[0].startTime)
		return SUB_OUT;

	  for(uint32_t i=0;i<_line;i++)
	  {
				if((time<=_subs[i].endTime)  &&
					(time>=_subs[i].startTime)
				)
				return i;

		}
		return SUB_OUT;
}
//
//	Display up to 3 lines of text centered on screen
//  Each line is separated by a |
// It is the basic version that tries to use only 2 lines
// in most cases, 3 when needed
//______________________________________
void ADMVideoSubtitle::displayString(subLine *string)
{
 
	uint32_t base,last=0,line=0;	
	char *ptr=NULL;
	uint32_t nbLine=0;
	uint32_t i=0;
	uint32_t suggestedLen=0;
	

      aprintf("Sub: Rendering string %s\n",string);


// bbase is the final position in line
// in the image


	base=0;

	memset(_bitmapBuffer,0,_info.height*_info.width);
	memset(_maskBuffer,0,_info.height*_info.width);
  	memset(_bgBitmapBuffer,0,(_info.height*_info.width)>>1);
  	memset(_bgMaskBuffer,0,_info.height*_info.width);

	
		
	
	nbLine=string->nbLine;
	if(nbLine>SRT_MAX_LINE )
	{
		nbLine=SRT_MAX_LINE;
		printf("Too much lines\n");
	}
	switch(nbLine)
	{
		case 0:
			base=2*_conf->_fontsize;; // 1 or 2 lines we dont use the upper line		
			break;
		case 1:
			base=_conf->_fontsize;; // 1 or 2 lines we dont use the upper line		
			break;
		default:break;
		
	}	
		
	aprintf("Sub: %d lines to render\n",nbLine);						
 
  
  	line=0;
	// scan and display each line
	for(i=0;i<nbLine;i++)
	{
		
			displayLine(string->string[i],base,string->lineSize[i]);
			base+=_conf->_fontsize;
			line++;
		
	}

    	
	
	// now blur bitmap into mask..
	memset(_maskBuffer,0,SRT_MAX_LINE*_conf->_fontsize*_info.width);

	uint32_t off;
	uint8_t *src,*dst;

	off=0;

	src=_bitmapBuffer;
	dst=_maskBuffer;


	// We shrink it down for u & v by 2x2
	// mask buffer->bitmap buffer

	uint8_t tmp[_info.width*_info.height];

	decimate(src,tmp,_info.width,_info.height);
	lowPass(src,dst,_info.width,_info.height);
	lowPass(tmp,src,_info.width>>1,_info.height>>1);

  	if (_conf->_useBackgroundColor) 
	{
    		decimate(_bgMaskBuffer,_bgBitmapBuffer,_info.width,_info.height);
    		//lowPass(tmp,_bgBitmapBuffer,_info.width>>1,_info.height>>1);
  	}
  
}
// Display a full line centered on screen. It returns the
// number of displayed chars.
//____________________________________________________________

// It also returns the suggested len for this line string (just when display fail)
//
uint32_t ADMVideoSubtitle::displayLine(ADM_GLYPH_T *string,uint32_t line, uint32_t len)
{
  // n first chars in string that can "fit" in this line
 

	//uint32_t pixstart;
	uint8_t *target;
	int car;


	aprintf("Sub DisplayString  line %lu len %lu\n",line,len);

	if(!len)
	{
		printf("\n null string\n");
      		return 0;
	}


  
	uint32_t w=0,next;
	int ww;

	// we dummy do it one time to get the actual width of the string
	// and hence perfectly center it
	target=_maskBuffer+_info.width*line;
	for(uint32_t i=0;i<len;i++)
	{
			if(ADM_ASC(*(string+i))==' ')
			{
					w+=_conf->_fontsize>>2;
			}
			else
			{
				ww=0;
				car=string[i];
				if(i) next=string[i-1];
					else next=0;
				// Change ' to "
				//if(ADM_ASC(car)==0x27)
				//	car=0x22;
	        		if(!_font->fontDraw((char *)(target+1+w),car,next , _info.width,_conf->_fontsize,&ww))
				{
					printf("Font error\n");
              return 0;
				}
				if((ww<0)||(ww>64))
				{
					printf("Warning w out of bound (%d)\n",ww);
					ww=0;
				}
				if(w+ww>_info.width)
				{
					printf("Line too long!\n");
					len=i;
					goto _abt;
					break;
				}
				w=w+ww;
			}
      
	}
_abt:
	//Now we can render it at its final position
	if(w<_info.width)
	{
		target=_bitmapBuffer+_info.width*line+((_info.width-w)>>1);
	}
	else
	{
		target=_bitmapBuffer+_info.width*line+1;
	}

  //printf("[debug] line %s\n",string);

	ww=0;
	w=0;
	for(uint32_t i=0;i<len;i++)
	{
			if(*(string+i)==' ')
			{
					w+=_conf->_fontsize>>2;

			}
			else
			{
				ww=0;
				car=string[i];
				if(i) next=string[i-1];
					else next=0;
	        		if(!_font->fontDraw((char *)(target+1+w),car,next , _info.width,_conf->_fontsize,&ww))
				{
					printf("Font error\n");
              return 0;
				}
				if((ww<0)||(ww>64))
				{
					printf("Warning w out of bound (%d)\n",ww);
					ww=0;
				}
				w=w+ww;
			}
      
	}

  {
    if (_conf->_useBackgroundColor) {
      //Create background info
      
      int32_t delta=_info.width*line+((_info.width-w)>>1);
      uint8_t *bitmapTarget=_bitmapBuffer+delta;
      uint8_t *maskTarget=_maskBuffer+delta;
      //uint8_t *bgBitmapTarget=_bgBitmapBuffer+delta;
      uint8_t *bgMaskTarget=_bgMaskBuffer+delta;
      
      delta=3*_info.width;
      bitmapTarget+=delta;
      maskTarget+=delta;
      //bgBitmapTarget+=delta;
      bgMaskTarget+=delta;
      
      for (uint32_t i=0;i<_conf->_fontsize;i++) {
	//memset(bgTarget,1,w);
	for (uint32_t j=0;j<w;j++) {
	  if (*(bitmapTarget+j)==0) {
	    *(bgMaskTarget+j)=1;   //at this position, we will apply a different color
	    //*(bgBitmapTarget+j)=1;
	    *(maskTarget+j)=0;
	    *(bitmapTarget+j)=0;
	  }
	}
	bgMaskTarget+=_info.width;
	//bgBitmapTarget+=_info.width;
	maskTarget+=_info.width;
	bitmapTarget+=_info.width;
      }

    }
  }
	

 
  return len;
}
//--------------------------------------------------------------------
//
//	Blend bitmap buffer into final screen
//      We scan the SRT_MAX_LINE last line
//--------------------------------------------------------------------
uint8_t ADMVideoSubtitle::blend(uint8_t *target,uint32_t baseLine)
{

	uint8_t  *mask; //*oldtarget=target,*
  uint8_t  *bgMask;
	int8_t *chromatarget;
	uint32_t hei,start;
	uint32_t y;
	uint32_t val;
	int32_t ssigned;
	double alpha,alpha2;
#define SHADOW_UP 	8
#define SHADOW_DOWN 	16

	hei=(SRT_MAX_LINE+1)*_conf->_fontsize;
	if(hei>=_info.height-1) hei=_info.height-1;
	hei*=_info.width;  // max height of our subtitle
	

	aprintf("Sub:Rendering : %d %d %d (yuv)\n",_conf->_Y_percent,_conf->_U_percent,_conf->_V_percent);
	
	// keep a little margin for renderin
	// to render f y or g
	if((baseLine) > _conf->_fontsize)
		baseLine-=_conf->_fontsize>>1;
	
		
	//__________________________
		
		// Shadow ..
		uint8_t *shadow=target;
		uint32_t shadow_pos;
		
		shadow_pos=_conf->_fontsize/10;
		start=_info.width*baseLine;  // base line in final image
		// mask out left and right
		mask=_maskBuffer; 
				
 		shadow+=start+(1+_info.width)*shadow_pos;
  		for( y=hei;y>0;y--)
		{
     		 if(*mask) 
		 {
				if(*mask>LUMA_LEVEL)
				{
					val=*shadow;
					val=(val*SHADOW_UP)/SHADOW_DOWN;
					*shadow=(uint8_t )val;
				}				
     		 } 	  
      
		shadow++;
		mask++;
		}
	
	// /Shadow
	//__________________________
	start=_info.width*baseLine;  // base line in final image

	chromatarget=(int8_t *)(target+(_info.width*_info.height));

	// mask out left and right

	mask=_maskBuffer;
  	bgMask=_bgMaskBuffer;
 	target+=start;
  	for( y=hei;y>0;y--)
	{
      if(*mask) {
	if(*mask>LUMA_LEVEL || _conf->_useBackgroundColor)
			{
				val=*mask*_conf->_Y_percent+128;
				val>>=8;
				*target=(uint8_t )val;
			}
	else { *target=0; }
      } else if(_conf->_useBackgroundColor && *bgMask) {
	*target=(uint8_t )_conf->_bg_Y_percent;
      }	  
      
		target++;
		mask++;
      bgMask++;
	}
	

// do u & v
	int8_t *ctarget;

	baseLine&=0xffffe;
	start=(_info.width*baseLine)>>2;
	hei>>=2;

	mask=_bitmapBuffer;
  	bgMask=_bgBitmapBuffer;
	ctarget=chromatarget+start;
	target=(uint8_t *)ctarget;

	ssigned=_conf->_U_percent;
	ssigned+=128;

  uint8_t bg_val=(uint8_t) _conf->_bg_U_percent+128;

//#define MAXVAL(x) {val=*mask*(x)+127;val>>=8;*ctarget=(uint8_t)(val&0xff);}

#define BLEND_LEVEL 	3
#define BLEND_PERCENT 	20

#define MAXVAL(x)  *target=(uint8_t )ssigned
#define DOIT if(*mask) \
		{ \
			alpha=*mask; \
			if(alpha>BLEND_LEVEL) \
				*target=(uint8_t)ssigned; \
			else			\
			{			\
				alpha2=(100-BLEND_PERCENT)*(*target-128); \
				alpha=BLEND_PERCENT*(ssigned-128); \
				alpha=(alpha2+alpha)/100; \
				*target=(uint8_t)(alpha+128) ;\
			} \
                } \
             else if (_conf->_useBackgroundColor && *bgMask) {*target=(uint8_t)bg_val;}
	     
#define RENDER	     \
	\
	for( y=hei;y>0;y--) \
	{ \
		DOIT; \
		ctarget++; \
		target++; \
		mask++; \
	bgMask++; \
	} \
	
	RENDER;

	mask=_bitmapBuffer;
  	bgMask=_bgBitmapBuffer;
	ctarget=chromatarget+start+((_info.width*_info.height)>>2);;
	target=(uint8_t *)ctarget;
	ssigned=_conf->_V_percent;
	ssigned+=128;

  	bg_val=(uint8_t) _conf->_bg_V_percent+128;

	RENDER;


	return 1;
}
//
//	Apply a low pass to alias it
//					1
// The softening is a 1 4 1  convolution filter
//					1
uint8_t ADMVideoSubtitle::lowPass(uint8_t *src, uint8_t *dst, uint32_t w, uint32_t h)
{
	uint8_t *prevline, *nextline,*curline,*target;
	uint32_t val;

	memcpy(dst,src,w*h);

	src++;dst++;

			for(uint32_t line=h-1;line>0;line--)
			{
				curline=src+line*w;
				nextline=curline+w;
				prevline=curline-w;
				target=dst+line*w;
				for(uint32_t col=w-1;col>1;col--)
				{
#define LEVEL 8*90
					// 1 4 1
					val= (*(curline)*4+*prevline+*nextline+*(curline-1)+*(curline+1));
					if(val> 0 )
					if(val<LEVEL)
						val=1;  // just enough to trigger masking
					else
						val>>=3;
					*target=val;
					target++;
					curline++;
					prevline++;
					nextline++;
				}

			}
	return 1;
}
//
//	Decimate 2x2 (luma->chroma)
//					1
//					1
uint8_t ADMVideoSubtitle::decimate(uint8_t *src, uint8_t *dst, uint32_t w, uint32_t h)
{
	uint8_t	*curline, *nextline;
	uint8_t *target=dst;

			for(uint32_t line=0;line<h>>1;line++)
			{
				curline=src+line*w*2;
				nextline=curline+w;
				target=dst+line*(w>>1);
				for(uint32_t col=0;col<w>>1;col++)
				{
					// 1 4 1
					*target= (*(curline)+*(curline+1)+*nextline+*(nextline+1))>>2;
					target++;
					curline+=2;
					nextline+=2;
				}

			}
	return 1;
}

#endif
