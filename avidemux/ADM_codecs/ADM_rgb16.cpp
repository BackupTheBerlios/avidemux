/***************************************************************************
                          ADM_rgb16.cpp  -  description
                             -------------------
    begin                : Mon May 27 2002
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



#include "ADM_library/default.h"

#include <ADM_assert.h>
#include "ADM_colorspace/colorspace.h"

#include "ADM_codecs/ADM_codec.h"
#include "ADM_codecs/ADM_rgb16.h"
void                  	rgbrgb(uint32_t r,uint32_t g,uint32_t b,uint32_t *y,uint32_t *u,uint32_t *v,uint32_t scale);



// constructor for mjpeg, init encoder and stuff
decoderRGB16::decoderRGB16(uint32_t w,uint32_t h) :decoders(w,h)
{
		planar=new uint8_t[_w*_h*2];
}
decoderRGB16::~decoderRGB16()
{
		delete [] planar;
}

//
//
//
//	Y  =      (0.257 * R) + (0.504 * G) + (0.098 * B) + 16
//	Cr = V =  (0.439 * R) - (0.368 * G) - (0.071 * B) + 128
//	Cb = U = -(0.148 * R) - (0.291 * G) + (0.439 * B) + 128
//
//
//

uint8_t 	decoderRGB16::uncompress(uint8_t *in,ADMImage *out,uint32_t len,uint32_t *flag) 	
{
uint32_t xx,yy,i;
uint8_t *ry,*ru,*rv;
uint16_t *in16   ;
uint32_t  r,g,b;
uint32_t  y,u,v;

 	  if(flag)
		       *flag=AVI_KEY_FRAME;

         if(len==(3*_w*_h)) // rgb 24 ?
         {
           	printf("\n RGB24");
         		return uncompressRGB24(  in,out, len);
           }

         	ry=out->data;
          ru=planar;
          rv=planar+_w*_h;
          in16=(uint16_t *)in;

         for( yy=_h;yy>0;yy--)
         {
           in16=(uint16_t *)in+(yy-1)*_w;
         	for(xx =_w;xx>0;xx--)
          {
                   i=*in16++;
           	     r=( ( i & 0x7C00 )>>10); 			// 0111 1100 0000 0000
           	     g= (( i & 0x03E0)>>5);             // 0000 0011 1110 0000
           	     b=( ( i & 0x001F));                   // 0000 0000 0001 1111

                 	rgbrgb(r,g,b,&y,&u,&v,20);
                     *ry++=(uint8_t)y;
                     *ru++=(uint8_t)u;
                     *rv++=(uint8_t)v;
            }
          }


            // now shrink  u,v to v12
            //
            uint8_t *ru2,*rv2,a,bb,c,d;
            uint32_t s;

          ru=planar;
          rv=planar+_w*_h;

            rv2=out->data+_w*_h;
            ru2=rv2+(_w*_h>>2);
            for( yy=_h>>1;yy>0;yy--)
            {
		     for(xx =_w>>1;xx>0;xx--)
            	{
              	       a=*ru;
                      	bb=*(ru+1);
                        c=*(ru+_w);
                        d=*(ru+_w+1);
                        s=a+bb+c+d;
                        s>>=2;
                        *ru2++=s;
                        ru+=2;

                         a=*rv;
                      	bb=*(rv+1);
                        c=*(rv+_w);
                        d=*(rv+_w+1);
                        s=a+bb+c+d;
                        s>>=2;
                        *rv2++=s;
                        rv+=2;
             		}
                   ru+=_w;
                   rv+=_w;

              }

         return 1;
}
uint8_t 	decoderRGB16::uncompressRGB24(uint8_t *in,ADMImage *out,uint32_t len) 	
{
UNUSED_ARG(len);
uint32_t xx,yy; //i;
uint8_t *ry,*ru,*rv;
uint8_t *in8   ;
uint32_t  r,g,b;
uint32_t  y,u,v;


         	ry=out->data;
          ru=planar;
          rv=planar+_w*_h;

         for( yy=_h;yy>0;yy--)
         {
           in8=(uint8_t *)in;
           in8+=(yy-1)*3*_w;
         	for(xx =_w;xx>0;xx--)
          {
            		b=*in8++;
             		g=*in8++;
            		r=*in8++;

                               	rgbrgb(r,g,b,&y,&u,&v,23);

                     *ry++=(uint8_t)y;
                     *ru++=(uint8_t)u;
                     *rv++=(uint8_t)v;
            }
          }
            // now shrink  u,v to v12
            //
            uint8_t *ru2,*rv2,a,bb,c,d;
            uint32_t s;

          ru=planar;
          rv=planar+_w*_h;

            rv2=out->data+_w*_h;
            ru2=rv2+(_w*_h>>2);
            for( yy=_h>>1;yy>0;yy--)
            {
		     for(xx =_w>>1;xx>0;xx--)
            	{
              	       a=*ru;
                      	bb=*(ru+1);
                        c=*(ru+_w);
                        d=*(ru+_w+1);
                        s=a+bb+c+d;
                        s>>=2;
                        *ru2++=s;
                        ru+=2;

                         a=*rv;
                      	bb=*(rv+1);
                        c=*(rv+_w);
                        d=*(rv+_w+1);
                        s=a+bb+c+d;
                        s>>=2;
                        *rv2++=s;
                        rv+=2;
             		}
                   ru+=_w;
                   rv+=_w;

              }

         return 1;
}
//__________________________________
//
//  Convert RGB triplet to YUV scale
//
//
//__________________________________

#define CLIP(x) if(x>255) x=255; if(x<0) x=0;
void                  	rgbrgb(uint32_t r,
											uint32_t g,
           								uint32_t b,
                   					uint32_t *y,
                        				uint32_t *u,
                            		uint32_t *v,uint32_t scale)
{

int32_t yy,uu,vv;
//                 		y= ((0.257 * r) + (0.504 * g) + (0.098 * b))*8.+16. ;
// multiply *2^23 8388608
                    yy=r*2155872+g*4227858+822083*b;

						vv= 3682598*r;
      					vv-=3087007*g;
           			vv-=595591*b;
					uu= 3682598*b;
     				uu-= 1241513*r;
         			uu-=2441084*g;

                  yy>>=scale;
                  yy+=16;

              	vv>>=scale;
                  vv+=128;

                  uu>>=scale;
                  uu+=128;
				      CLIP(yy);
                     CLIP(vv);
                     CLIP(uu);
            *u=uu;
            *v=vv;
            *y=yy;


 }

