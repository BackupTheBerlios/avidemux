/***************************************************************************
                          ADM_vidConvolution.cpp  -  description
                             -------------------

       General convolution filter

    begin                : Wed Apr 10 2002
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
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include"ADM_video/ADM_vidConvolution.hxx"
#include "ADM_filter/video_filters.h"


static FILTER_PARAM convParam={2,{"luma","chroma"}};
SCRIPT_CREATE(median_script,AVDMFastVideoMedian,convParam);
SCRIPT_CREATE(mean_script,AVDMFastVideoMean,convParam);
SCRIPT_CREATE(sharpen_script,AVDMVideoSharpen,convParam);
SCRIPT_CREATE(gaussian_script,AVDMVideoGaussian,convParam);


//AVDMGenericVideoStream *sharpen_create(AVDMGenericVideoStream *in, void *param);
//AVDMGenericVideoStream *(AVDMGenericVideoStream *in, void *param);
//AVDMGenericVideoStream *Gaussian_create(AVDMGenericVideoStream *in, void *param);
BUILD_CREATE(median_create,AVDMFastVideoMedian);

 char *AVDMVideoConvolution::printConf(void)
{
 		ADM_assert(0); // this one is pure
}

//_______________________________________________________________

AVDMVideoConvolution::AVDMVideoConvolution(
									AVDMGenericVideoStream *in,CONFcouple *setup)
{
    UNUSED_ARG(setup);

  	_in=in;		
   	memcpy(&_info,_in->getInfo(),sizeof(_info));  		
	
					
// 	_uncompressed=new uint8_t [3*_in->getInfo()->width*_in->getInfo()->height];
 	_uncompressed=new ADMImage(_in->getInfo()->width,_in->getInfo()->height);
  ADM_assert(_uncompressed);
  _U=new uint8_t [_in->getInfo()->width*_in->getInfo()->height];
  ADM_assert(_U);
   _V=new uint8_t [_in->getInfo()->width*_in->getInfo()->height];
  ADM_assert(_V);
  _info.encoding=1;

  	  	
}



AVDMVideoConvolution::~AVDMVideoConvolution()
{
 	delete _uncompressed;
 	delete [] _U;
  	delete [] _V;

  _uncompressed=NULL;
  _U=_V=NULL;
}

//
//	Basically ask a uncompressed frame from editor and ask
//		GUI to decompress it .
//

uint8_t AVDMVideoConvolution::getFrameNumberNoAlloc(uint32_t frame,
				uint32_t *len,
   				ADMImage *data,
				uint32_t *flags)
{
uint8_t *dst,*dstu,*dstv,*srcu,*srcv;
			ADM_assert(frame<_info.nb_frames);
			ADM_assert(_uncompressed);					
								
			// read uncompressed frame
       		if(!_in->getFrameNumberNoAlloc(frame, len,_uncompressed,flags)) return 0;

         	
                srcu=_uncompressed->data+( _info.height * _info.width);
                srcv=srcu+(( _info.height * _info.width)>>2);
                // expand u & v in buffers
                 dstu=_U;
                 dstv=_V;
              	dst=data->data;
                for(int32_t y=(int32_t)_info.height>>1;y>0;y--)
                {
 	               for(int32_t x=(int32_t)_info.width>>1;x>0;x--)
                 			{
                  			   *(dstu+1)=*dstu= *(dstu+1+_info.width)=*(dstu+_info.width)=*srcu++;
                  			   *(dstv+1)=*dstv=*(dstv+1+_info.width)=*(dstv+_info.width)=*srcv++;
                  			

                        				dstu+=2;
                        				dstv+=2;

                      	}
                       dstu+=_info.width;
                       dstv+=_info.width;
                 }


               // Luma
              for(int32_t y=0;y<(int32_t)_info.height;y++)
  				{
               	for(int32_t x=0;x<(int32_t)_info.width;x++)
                	{
          				*dst++=convolutionKernel(x,y,_uncompressed->data);
          				

              	}
               }

                // Chroma u & v
                dstu=data->data+( _info.height * _info.width);
               	dstv=dstu+(( _info.height * _info.width)>>2)  ;

               for(int32_t y=0;y<(int32_t)_info.height>>1;y++)
  				{
               	for(int32_t x=0;x<(int32_t)_info.width>>1;x++)
                	{
          			*dstu++=(convolutionKernel(2*x,2*y,_U)+convolutionKernel(2*x+1,2*y,_U)+
              					convolutionKernel(2*x,2*y+1,_U)+convolutionKernel(2*x,2*y+1,_U))>>2;
          				
          			*dstv++=(convolutionKernel(2*x,2*y,_V)+convolutionKernel(2*x+1,2*y,_V)+
              				convolutionKernel(2*x,2*y+1,_V)+convolutionKernel(2*x,2*y+1,_V))>>2;

              	}
               }
//
           	  *flags=0;
       		  *len= _info.width*_info.height+(_info.width*_info.height>>1); 
		    data->copyInfo(_uncompressed);
      return 1;
}



//_____________________ sharpen__________________
uint8_t  AVDMVideoSharpen:: convolutionKernel(int32_t x,int32_t y,uint8_t *data )
{
//
//
//
//


	//         -1 -2 -1
 	//         -2 16 -2 *1/16
  	//		    -1 -2 -1

   int32_t o=0;

#define GetPixel(x,y) getPixel(x,y,data)
           /*
       		o=GetPixel(x-1,y)+GetPixel(x+1,y)+GetPixel(x,y-1)+GetPixel(x,y+1);
              o<<=1;
              o+=GetPixel(x+1,y+1)+GetPixel(x-1,y+1)+GetPixel(x+1,y-1)+GetPixel(x-1,y-1);
              o=(GetPixel(x,y)*12)-o;
              o>>=4;

              o>>=1;
              if(o>128) o=128;
              if(o<-128) o=-128;

              o=GetPixel(x,y)+o;
              */

               	o+=GetPixel(x,y-1);
               	o+=GetPixel(x-1,y);
                 	o+=GetPixel(x+1,y);	
                	o+=GetPixel(x,y+1);

              o=(4*GetPixel(x,y))-o;

              o>>=2;
              // 0.66
              o=o+3*GetPixel(x,y);
              o=o/3;
              // we now have 1.66 value
              // *5/3

              if(o<0) o=0;
              if(o>255) o=255;

              return ((uint8_t) o);


}
char *AVDMVideoSharpen::printConf(void)
{
 		return (char *)"Sharpen";; // this one is pure
}



 //________________Mean _____________________

uint8_t  AVDMVideoMean:: convolutionKernel(int32_t x,int32_t y,uint8_t *data )
{

	//   	1 1 1
   //		1 1 1
   //		1 1 1
   int32_t o=0;

#define GetPixel(x,y) getPixel(x,y,data)

       		for(int32_t xx=-1;xx<2;xx++)
         		{
               	o+=GetPixel(x+xx,y-1);
               	o+=GetPixel(x+xx,y);
               	o+=GetPixel(x+xx,y+1);

              }
              o=o/9;

              return ((uint8_t) o);

}

char *AVDMVideoMean::printConf(void)
{
 		return (char *)"Mean";; // this one is pure
}



 //________________ Median _____________________

uint8_t  AVDMVideoMedian:: convolutionKernel(int32_t x,int32_t y,uint8_t *data )
{


   int32_t o=0,i=0;
   static uint8_t tab[9];

#define GetPixel(x,y) getPixel(x,y,data)

       		for(int32_t xx=-1;xx<2;xx++)
         		{
               	tab[i++]=GetPixel(x+xx,y-1);
               	tab[i++]=GetPixel(x+xx,y);
               	tab[i++]=GetPixel(x+xx,y+1);
              }
              // sort tab

               for(uint32_t bo=0;bo<9;bo++)
               	{
                   	for(o=bo;o<9;o++)
                    		{
                         	if( tab[bo]>tab[o])
                          			{
                                    	i=tab[bo];
                                       tab[bo]=tab[o];
                                       tab[o]=i;
                               		}

                        }

                  }


              return tab[4];


}

char *AVDMVideoMedian::printConf(void)
{
 		return (char *)"Median";; // this one is pure
}




 //________________ Gaussian smooth _____________________

uint8_t  AVDMVideoGaussian:: convolutionKernel(int32_t x,int32_t y,uint8_t *data )
{

	//         6 10 10
 	//         10 16 10 *1/80
  	//		    6 10 6

   int32_t o=0;

#define GetPixel(x,y) getPixel(x,y,data)
              o=  10*GetPixel(x,y-1);
              o+=  16*GetPixel(x,y);
				o+=  10*GetPixel(x,y+1);

       		
               	o+=6*GetPixel(x+1,y-1);
               	o+=10*GetPixel(x+1,y);
               	o+=6*GetPixel(x+1,y+1);

               	o+=6*GetPixel(x-1,y-1);
               	o+=10*GetPixel(x-1,y);
               	o+=6*GetPixel(x-1,y+1);

                o+=40;
                o>>=3;
                o/=10;

    			return (uint8_t)(o);


}

char *AVDMVideoGaussian::printConf(void)
{
 		return (char *)"Gaussian Smooth";; // this one is pure
}



#endif


