/***************************************************************************
                          ADM_vidFastConvolution.cpp  -  description
                             -------------------
    begin                : Sat Nov 23 2002
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
#include "ADM_default.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include"ADM_filter/video_filters.h"
#include "DIA_factory.h"
#include "ADM_vidConvolution.hxx"
#include "ADM_assert.h"


BUILD_CREATE(mean_create,AVDMFastVideoMean);
BUILD_CREATE(sharpen_create,AVDMFastVideoSharpen);
BUILD_CREATE(Gaussian_create,AVDMFastVideoGauss);
BUILD_CREATE(median_create,AVDMFastVideoMedian);

static FILTER_PARAM convParam={2,{"luma","chroma"}};

SCRIPT_CREATE(mean_script,AVDMFastVideoMean,convParam);
SCRIPT_CREATE(sharpen_script,AVDMFastVideoSharpen,convParam);
SCRIPT_CREATE(gaussian_script,AVDMFastVideoGauss,convParam);
SCRIPT_CREATE(median_script,AVDMFastVideoMedian,convParam);


uint8_t	AVDMFastVideoConvolution::getCoupledConf( CONFcouple **couples)
{

	ADM_assert(_param);
	
	*couples=new CONFcouple(2);

#define CSET(x)  (*couples)->setCouple((char *)#x,(_param->x))
	CSET(chroma);
	CSET(luma);
	return 1;

}

//_______________________________________________________________

AVDMFastVideoConvolution::AVDMFastVideoConvolution(
			AVDMGenericVideoStream *in,CONFcouple *couples)
{


	_in=in;		
	memcpy(&_info,_in->getInfo(),sizeof(_info));
	_uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);	
	ADM_assert(_uncompressed); 
	_info.encoding=1;
	if(couples==NULL)
	{
		_param=NEW( CONV_PARAM);
		_param->chroma=1;
		_param->luma=1;	
		printf("\n Creating from null\n");				
	}
	else
	{
		_param=NEW(CONV_PARAM);
		GET(luma);
		GET(chroma);
	}

  	  	
}
AVDMFastVideoConvolution::~AVDMFastVideoConvolution()
{
	if(_uncompressed)
 		delete _uncompressed;
	_uncompressed=NULL;
 	DELETE(_param);
}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMFastVideoConvolution::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
//uint8_t *dst,*dstu,*dstv,*srcu,*srcv;
uint8_t *x1,*x2,*x3,*o1;
uint32_t stride,page;

	if(frame>= _info.nb_frames) return 0;
	ADM_assert(_uncompressed);					
	stride=_info.width;
	page=(stride*_info.height)>>2;
	

	
	// read uncompressed frame
	if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;               
         
	if(!_param->luma)
	{
		memcpy(YPLANE(data),YPLANE(_uncompressed),page*4);
	}
	else
	{
		o1=YPLANE(data)+stride;
		x1=YPLANE(_uncompressed);
		x2=x1+stride;
		x3=x2+stride;

		// first and last line
		memcpy(YPLANE(data),YPLANE(_uncompressed),stride);
		memcpy(YPLANE(data)+page*4-stride,YPLANE(_uncompressed)+page*4-stride,stride);          
		// Luma
		for(int32_t y=1;y<(int32_t)_info.height-1;y++)
		{
			doLine(x1,x2,x3,o1,stride);
			x1=x2;
			x2=x3;
			x3+=stride; 
			o1+=stride;                 
		}
	}
      	// chroma u & v
	if(!_param->chroma)
	{
		memcpy(UPLANE(data),UPLANE(_uncompressed),page);
		memcpy(VPLANE(data),VPLANE(_uncompressed),page);
	}
	else
	{
		stride>>=1;
		// chroma u
		o1=UPLANE(data)+stride;
		x1=UPLANE(_uncompressed);
		x2=x1+stride;
		x3=x2+stride;
		// first and last line
		memcpy(UPLANE(data),UPLANE(_uncompressed),stride);
		memcpy(UPLANE(data)+page-stride,UPLANE(_uncompressed)+page-stride,stride);          
		// Luma
		for(int32_t y=1;y<(int32_t)(_info.height>>1)-1;y++)
		{
			doLine(x1,x2,x3,o1,stride);
			x1=x2;
			x2=x3;
			x3+=stride; 
			o1+=stride;                 
		}
		
		// chroma V
		o1=VPLANE(data)+stride;
		x1=VPLANE(_uncompressed);
		x2=x1+stride;
		x3=x2+stride;
		// first and last line
		memcpy(VPLANE(data),VPLANE(_uncompressed),stride);
		memcpy(VPLANE(data)+page-stride,VPLANE(_uncompressed)+page-stride,stride);          
		// Luma
		for(int32_t y=1;y<(int32_t)(_info.height>>1)-1;y++)
		{
			doLine(x1,x2,x3,o1,stride);
			x1=x2;
			x2=x3;
			x3+=stride; 
			o1+=stride;                 
		}
	}
	data->copyInfo(_uncompressed);
      return 1;
}
//
//	Run the convolution kernel on a whole line
//	to speed up things (cache effect + locality of datas)
//
//----------------------------------------------------------------
//----------------------------------------------------------------
//-----------------------MEAN---------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

char 	*AVDMFastVideoMean::printConf(void)
{
		static char str[]="Mean(fast)";
		return (char *)str;
	
}
uint8_t AVDMFastVideoMean::doLine(uint8_t  *pred,
																					uint8_t *cur,
   																				uint8_t *next,
   																				uint8_t *out,
                       										uint32_t w)
                                 
{
	uint8_t a1,a2,a3;
	uint8_t b1,b2,b3;
	uint8_t c1,c2,c3;
	int32_t o;
	
	a2=*pred++;a3=*pred++;
	b2=*cur++;b3=*cur++;
	c2=*next++;c3=*next++;
	
	*out++=b2;
	w--;
	
	while(w>1)
	{
			a1=a2;
			a2=a3;
			a3=*pred++;
			b1=b2;
			b2=b3;
			b3=*cur++;
			c1=c2;
			c2=c3;
			c3=*next++;
		
		  //
		  o=a1+a2+a3+b1+b2+b3+c1+c2+c3;
		  o/=9;
		  
		  *out++=o;
		  w--;
	}	
		*out++=b3;
		return 1;
}

//----------------------------------------------------------------
//----------------------------------------------------------------
//-----------------------GAUSS---------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

//          6 10 6
//         10 16 10 *1/80
//		    6 10 6
char 							*AVDMFastVideoGauss::printConf(void)
{
		static char str[]="Gauss(fast)";
		return (char *)str;
	
}
 uint8_t AVDMFastVideoGauss::doLine(uint8_t  *pred,
																					uint8_t *cur,
   																				uint8_t *next,
   																				uint8_t *out,
                       										uint32_t w)
                                 
{
	uint8_t a1,a2,a3;
	uint8_t b1,b2,b3;
	uint8_t c1,c2,c3;
	int32_t o;
//#define MASKED__	
#define threshold 80
	int v,r;
	
	a2=*pred++;a3=*pred++;
	b2=*cur++;b3=*cur++;
	c2=*next++;c3=*next++;
	
	*out++=b2;
	w--;
	
	while(w>1)
	{
			a1=a2;
			a2=a3;
			a3=*pred++;
			b1=b2;
			b2=b3;
			b3=*cur++;
			c1=c2;
			c2=c3;
			c3=*next++;
		
		  //
#ifdef MASKED__
		v=b2;
		r=16;
		o=b2*16;
		#define MORE(x,coef) if(abs(x-v)<=threshold) {o+=x*coef;r+=coef;}
		MORE(a1,6);
		MORE(a2,10);
		MORE(a3,6);
		
		MORE(b1,10);
		MORE(b3,10);
		
		MORE(c1,6);
		MORE(c2,10);
		MORE(c3,6);
		
		o=(o+r-1)/r;		
		
#else		  
		  o=6*a1+10*a2+6*a3+10*b1+16*b2+10*b3+6*c1+10*c2+6*c3;		  
		  o/=80;
#endif		  
		  
		  *out++=o;
		  w--;
	}	
	*out++=b3;
		return 1;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
//-----------------------SHARPEN---------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

//         -1 -2 -1
 	//         -2 16 -2 *1/16
  	//		    -1 -2 -1
char 							*AVDMFastVideoSharpen::printConf(void)
{
		static char str[]="Sharpen(fast)";
		return (char *)str;
	
}
 uint8_t AVDMFastVideoSharpen::doLine(uint8_t  *pred,
																					uint8_t *cur,
   																				uint8_t *next,
   																				uint8_t *out,
                       										uint32_t w)
                                 
{
	uint8_t a1,a2,a3;
	uint8_t b1,b2,b3;
	uint8_t c1,c2,c3;
	int32_t o;
	
	a2=*pred++;a3=*pred++;
	b2=*cur++;b3=*cur++;
	c2=*next++;c3=*next++;
	
	*out++=b2;
	w--;
	
	while(w>1)
	{
			a1=a2;
			a2=a3;
			a3=*pred++;
			b1=b2;
			b2=b3;
			b3=*cur++;
			c1=c2;
			c2=c3;
			c3=*next++;
		
		  //
		  //         -1 -2 -1
 	//         -2 16 -2 *1/16
  	//		    -1 -2 -1
		  //o=-a1+-2*a2+-a3+-2*b1+16*b2+-2*b3+-1*c1+-2*c2+-1*c3;
		  //o/=16;
		  o=4*b2-a2-c2-b1-b3;
		  o>>=2;
		  o+=3*a2;
		  o=o/3;
		  if(o<0) o=0;
		  if(o>255) o=255;
		  
		  *out++=o;
		  w--;
	}	
	*out++=b3;
		return 1;
}
//----------------------------------------------------------------
//----------------------------------------------------------------
//-----------------------Median---------------------
//----------------------------------------------------------------
//----------------------------------------------------------------

//         -1 -2 -1
 	//         -2 16 -2 *1/16
  	//		    -1 -2 -1
char 							*AVDMFastVideoMedian::printConf(void)
{
		static char str[]="Median(fast)";
		return (char *)str;
	
}
 uint8_t AVDMFastVideoMedian::doLine(uint8_t  *pred,
																					uint8_t *cur,
   																				uint8_t *next,
   																				uint8_t *out,
                       										uint32_t w)
                                 
{
	uint8_t a1,a2,a3;
	uint8_t b1,b2,b3;
	uint8_t c1,c2,c3; //,i;
	//int32_t o;
	uint8_t temp;
	
	static uint8_t tab[9];
	a2=*pred++;a3=*pred++;
	b2=*cur++;b3=*cur++;
	c2=*next++;c3=*next++;
	
	*out++=b2;
	w--;
	
	while(w>1)
	{
			tab[0]=a1=a2;
			tab[1]=a2=a3;
			tab[2]=a3=*pred++;
			tab[3]=b1=b2;
			tab[4]=b2=b3;
			tab[5]=b3=*cur++;
			tab[6]=c1=c2;
			tab[7]=c2=c3;
			tab[8]=c3=*next++;
		
#define PIX_SORT(a,b) { if ((a)>(b)) PIX_SWAP((a),(b)); }
#define PIX_SWAP(a,b) { temp=(a);(a)=(b);(b)=temp; }

   uint8_t *p=(uint8_t *)tab;
								
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[1]) ; PIX_SORT(p[3], p[4]) ; PIX_SORT(p[6], p[7]) ;
    PIX_SORT(p[1], p[2]) ; PIX_SORT(p[4], p[5]) ; PIX_SORT(p[7], p[8]) ;
    PIX_SORT(p[0], p[3]) ; PIX_SORT(p[5], p[8]) ; PIX_SORT(p[4], p[7]) ;
    PIX_SORT(p[3], p[6]) ; PIX_SORT(p[1], p[4]) ; PIX_SORT(p[2], p[5]) ;
    PIX_SORT(p[4], p[7]) ; PIX_SORT(p[4], p[2]) ; PIX_SORT(p[6], p[4]) ;
    PIX_SORT(p[4], p[2]) ; 
		  
		  *out++=tab[4];
		  w--;
	}	
	*out++=b3;
	return 1;
}
uint8_t AVDMFastVideoConvolution::configure(AVDMGenericVideoStream * instream)
{
  
  //return DIA_getLumaChroma(&(_param->luma),&(_param->chroma)) ; 
  diaElemToggle luma(&(_param->luma),QT_TR_NOOP("_Process luma"),QT_TR_NOOP("Process luma plane"));
  diaElemToggle chroma(&(_param->chroma),QT_TR_NOOP("P_rocess chroma"));
  
  diaElem *elems[2]={&luma,&chroma};
  
  return diaFactoryRun(QT_TR_NOOP("Fast Convolution"),2,elems);
}



