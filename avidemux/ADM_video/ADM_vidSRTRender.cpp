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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <assert.h>
#include <math.h>
#include <math.h>
#include <iconv.h>
#include "config.h"

#ifdef USE_FREETYPE

#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_video/ADM_vidFont.h"
#include "ADM_video/ADM_vidSRT.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"


#define SUB_OUT 0xf0000000

#define ALPHA
//                     1
//		Get in range in 121 + coeff matrix
//                     1
//
// If the value is too far away we ignore it
// else we blend
//__________________________________________________________________
uint8_t ADMVideoSubtitle::getFrameNumberNoAlloc(uint32_t frame,
						uint32_t *len,
   						uint8_t *data,
   						uint32_t *flags)
{

double f;
uint32_t time,srch;
uint32_t absFrame=frame+_info.orgFrame;

	//		printf("\n Stabilize : %lu\n",frame);
          _uncompressed=data;
	assert(frame<_info.nb_frames);
	if(!_in->getFrameNumberNoAlloc(frame, len,data,flags)) return 0;

	// if no subtitle return...
	if(!_subs) return 1;

	// compute time stamp
	      f=absFrame*1000000./_info.fps1000;
	      time=(uint32_t) floor(f);
	      // Do we have it already ?

	      if(time<=_subs[_oldline].endTime && time >=_subs[_oldline].startTime)
	      {
	      		//printf("\n cached\n");
			blend(data,_conf->_baseLine); // re-use it
			return 1;	

	      }
	      srch=search(time);
	      // we got the right one
	      if(srch!=SUB_OUT) // we got something...
			{
					_oldline=srch;
					//printf("\n %s \n",_subs[_oldline].string);
					displayString(_subs[_oldline].string);
					blend(data,_conf->_baseLine);
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
//______________________________________
void ADMVideoSubtitle::displayString(char *string)
{
	uint32_t base,last=0,line=0;
	double bbase;

//	printf("%s\n",string);


					// bbase is the final position in line
					// in the image


					base=0;

					memset(_bitmapBuffer,0,_info.height*_info.width);
					memset(_maskBuffer,0,_info.height*_info.width);


//   // search for | char
//   for(uint32_t i=0;i<strlen(string);i++)
//     {
//       if( *(string+i)=='|')
// 	{
// 	  displayLine(string+last,base,i-last);
// 	  last=i+1;
// 	  base+=_conf->_fontsize;
// 	  line++;
// 	  if(line>3)
// 	    {
// 	      printf("\n Maximum 3 lines!\n");
// 	      return;
// 	    }
// 	}
//     }
//   //printf("\len :%d\n",strlen(string)-last);
//   displayLine(string+last,base,strlen(string)-last);


							{
    const uint32_t stringLen=strlen(string);
    uint32_t i=0;
    uint32_t suggestedLen=0;
    while (last<stringLen && line<3) {
      // search for | char
      while (*(string+i)!='|' && i<stringLen) {
	i++;
      }
      
      uint32_t couldDisplay=displayLine(string+last,base,i-last,&suggestedLen);
      //printf("[debug] couldDisplay=%d suggestedLen=%d\n",couldDisplay,suggestedLen);
      if (! couldDisplay && _conf->_selfAdjustable && suggestedLen>0) {
	// Try to fit
	uint32_t newSuggLen=suggestedLen;
	while (newSuggLen>0 && string[last+newSuggLen-1]!=' '){
	  newSuggLen--;
	}
	if (newSuggLen>0){ suggestedLen=newSuggLen; }
	//printf("[debug] suggestedLen=%d\n",suggestedLen);
	couldDisplay = displayLine(string+last,base,suggestedLen);
	last+=suggestedLen;
	if (i<=last){i++;}
	//printf("[debug] last=%d i=%d\n",last,i);
      } else {
	if (! couldDisplay) {
	  printf("line is too big.\n");
	}
									last=i+1;
	i++;
      }
      
									base+=_conf->_fontsize;
									line++;
    }

    if(line>=3)
									{
	//index range [0,1,2]
											printf("\n Maximum 3 lines!\n");
	//return;
									}
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


}
// Display a full line centered on screen. It returns the
// number of displayed chars.
//____________________________________________________________
uint32_t ADMVideoSubtitle::displayLine(char *string,uint32_t line, uint32_t len) {
  uint32_t i;
  return displayLine(string,line,len,&i);
}

// It also returns the suggested len for this line string (just when display fail)
uint32_t ADMVideoSubtitle::displayLine(char *string,uint32_t line, uint32_t len, uint32_t *suggestedLen)
{
  // n first chars in string that can "fit" in this line
  (*suggestedLen) = 0;

	//uint32_t pixstart;
	uint8_t *target;
	int car;


	aprintf("--> %s at line %lu len %lu\n",string,line,len);

	if(!len)
	{
		printf("\n null string\n");
      return 0;
	}

	uint32_t w=0;
	int ww;

	// we dummy do it one time to get the actual width of the string
	// and hence perfectly center it
	target=_maskBuffer+_info.width*line;
	for(uint32_t i=0;i<len;i++)
	{
			if(*(string+i)==' ')
			{
					w+=_conf->_fontsize>>2;
			}
			else
			{
				ww=0;
				car=(int) (unsigned char)*(string+i);

	        		if(!_font->fontDraw((char *)(target+1+w),car , _info.width,_conf->_fontsize,&ww))
				{
					printf("Font error\n");
              return 0;
				}
				if((ww<0)||(ww>64))
				{
					printf("Warning w out of bound (%d)\n",ww);
					ww=0;
				}
				w=w+ww+2;
			}
      if(w>(_info.width-_conf->_fontsize*2)) {
	// try i-1 chars
	(*suggestedLen) = i>0 ? i-1 : 0;
	return 0;
      }
	}

	//Now we can render it at its final position
	target=_bitmapBuffer+_info.width*line+((_info.width-w)>>1);

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
				car=(int) (unsigned char)*(string+i);

	        		if(!_font->fontDraw((char *)(target+1+w),car , _info.width,_conf->_fontsize,&ww))
				{
					printf("Font error\n");
              return 0;
				}
				if((ww<0)||(ww>64))
				{
					printf("Warning w out of bound (%d)\n",ww);
					ww=0;
				}
				w=w+ww+2;
			}
      if(w>(_info.width-_conf->_fontsize*2)) {
	(*suggestedLen) = i>0? i-1: 0;
	break;
      }
	}
	// now w is the real width to displayString
	// memcpy it to its real position
	if(w>_info.width)
		{
      uint32_t r=(*suggestedLen);
      (*suggestedLen)=0;
      printf(" line too long\n");
      return r;
		}

  (*suggestedLen)=0;
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
	int8_t *chromatarget;
	uint32_t hei,start;
	uint32_t y;
	uint32_t val;
	int32_t ssigned;


	hei=SRT_MAX_LINE*_conf->_fontsize*_info.width;  // max height of our subtitle

	aprintf("Sub:Rendering : %d %d %d (yuv)\n",_conf->_Y_percent,_conf->_U_percent,_conf->_V_percent);
	start=_info.width*baseLine;  // base line in final image

	chromatarget=(int8_t *)(target+(_info.width*_info.height));

	// mask out left and right

	mask=_maskBuffer;
 	target+=start;
#define LUMA_LEVEL 10
  	for( y=hei;y>0;y--)
	{
		if(*mask)
		{
			if(*mask>LUMA_LEVEL)
			{
				val=*mask*_conf->_Y_percent+128;
				val>>=8;
				*target=(uint8_t )val;
			}
			else *target=0;
		}
		target++;
		mask++;
	}

// do u & v
	int8_t *ctarget;

	baseLine&=0xffffe;
	start=(_info.width*baseLine)>>2;
	hei>>=2;

	mask=_bitmapBuffer;
	ctarget=chromatarget+start;
	target=(uint8_t *)ctarget;

	ssigned=_conf->_U_percent;
	ssigned+=128;

//#define MAXVAL(x) {val=*mask*(x)+127;val>>=8;*ctarget=(uint8_t)(val&0xff);}
#define MAXVAL(x)  *target=(uint8_t )ssigned
#define DOIT if(*mask) \
		{ \
			if(*mask==1) \
				*target=128; \
			else \
				MAXVAL(_conf->_U_percent); \
		}
	//if(_conf->_U_percent!=128)
	if(1)
  	for( y=hei;y>0;y--)
	{
		DOIT;
		ctarget++;
		target++;
		mask++;
	}
	else
	{
		aprintf("No U\n");
	}


	mask=_bitmapBuffer;
	ctarget=chromatarget+start+((_info.width*_info.height)>>2);;
	target=(uint8_t *)ctarget;
	ssigned=_conf->_V_percent;
	ssigned+=128;


	//if(_conf->_V_percent)
	if(1)
  	for( y=hei;y>0;y--)
	{

		DOIT;
		ctarget++;
		target++;
		mask++;
	}
	else
	{
		aprintf("No V\n");
	}



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
