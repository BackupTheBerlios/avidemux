/***************************************************************************
                          yv2bmp.cpp  -  description
                             -------------------

Convert yv12 to bmp firendly RGB

    begin                : Wed Apr 19 2003
    copyright            : (C) 2003 by mean
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
#include <ADM_assert.h>
#include <string.h>
#include "config.h"
#include "ADM_library/default.h"


#include "colorspace.h"
#ifdef HAVE_ALTIVEC
extern uint8_t altivecYV12RGB(uint8_t * ptr_y,
		    uint8_t * ptr_u,
		    uint8_t * ptr_v, uint8_t * ptr2, uint32_t w);
#endif		    

static void swapRnB (uint8_t * ptr, uint32_t w);
static uint8_t slowYV12RGB(uint8_t * ptr_y,
		    uint8_t * ptr_u,
		    uint8_t * ptr_v, uint8_t * ptr2, uint32_t w);


extern void fastYV12RGB_mmx ( uint8_t * py,
			       uint8_t * pu, uint8_t * pv,  uint8_t * image,
			       int h_size);
static uint8_t Clip[256+64+64];

/**
	Convert yv12 to RGB bmp
	i.e it is BGR and from bottom to top
	the output buffer is allocated here
____________________________________________
*/

 uint8_t  COL_yv12rgbBMP(uint32_t ww, uint32_t hh, uint8_t* in, uint8_t *out)
 {

	uint8_t *tmp;
	tmp=(uint8_t *)malloc(ww*hh*4);

	if( !tmp)
		{
			return 0;
		}
	COL_yv12rgb( ww,  hh, in, tmp);
	
	// First pack it so that we go from RGB32 to RGB24
	uint8_t *rin,*rout;
	
	rin=rout=tmp;
	
	for(uint32_t jj=ww*hh;jj>0;jj--)
	{
		
		*rout++=*rin++;
		*rout++=*rin++;
		*rout++=*rin++;
		rin++;
	
	}
	
	// now revert RGB & up/down order
	 // invert screen buffer + swap y
  uint8_t *e,*write;
  uint32_t w, y;
  uint8_t buff[800 * 3];

  w = ww*3;

  e = tmp;
  e += w * (hh - 1);
  write=out;

  y = (uint32_t) hh;
  for (; y > 0; y--)
    {
      //
      memcpy (buff, e, w);
      swapRnB (buff, ww);
      memcpy(write,buff,w);
      e -= w;
      write+=w;
    }

	free(tmp);
	return 1;
}
/**
	Convert yv12 to RGB the slow way, i.e. with plain C

*/
/*
Packed
		Integer computation convert YV12 -> plain RGB
		Y 4 bits U 4 bits V 4 bits
		

    2²21=2097152
R = Y + 1.402V              2940207
G = Y - 0.344U - 0.714V     721420 1497366
B = Y + 1.772U              3716153
See
http://support.microsoft.com/default.aspx?scid=kb;EN-US;q294880
For format explanation
 Comprised of an NxN Y plane followed by (N/2)x(N/2) U and V planes. This format draws a top-down image (for example, the first line is at the top of the screen).

*/
#define FAC  2097152
#define RFAC 2940207
#define GFAC1 721420
#define GFAC2 1497366
#define BFAC 3716153



uint8_t slowYV12RGB(uint8_t * ptr_y,
		    uint8_t * ptr_u,
		    uint8_t * ptr_v, uint8_t * ptr2, uint32_t w)
{

    int32_t y0, u0, v0;
    int32_t y1;
    int32_t r, g, b;
    int32_t r1, g1, b1;
    static int init=0;

    	if(init==0)
     		{
         		memset(Clip,255,256+64+64);
         		memset(Clip,0,64);
           	for(int16_t i=0;i<256;i++) Clip[i+64]=i;
              init=1;
         }
/*  	Let's do 2^21 computation   */

     if(w& 2) w-=2; // manolis bug
    while (w)
      {
        //

//#define CLIP(x) if(x<0) x=0;if(x>255) x=255;
#define CLIP(x) x=Clip[x+64]

#define MMM(y,u,v)		r1= RFAC*v; \
		g1= -GFAC1*u -GFAC2*v; \
		b1= + BFAC*u ;      \
	  r = (y +r1)>>21;    \
		g = (y +g1)>>21;    \
		b = (y +b1)>>21;    \
    CLIP(r);CLIP(g);CLIP(b); \
	*ptr2++=r;	*ptr2++=g;	*ptr2++=b;ptr2++;
#define MMM2(y) \
	  r = (y +r1)>>21;    \
		g = (y +g1)>>21;    \
		b = (y +b1)>>21;    \
    CLIP(r);CLIP(g);CLIP(b); \
	*ptr2++=r;	*ptr2++=g;	*ptr2++=b;ptr2++;

	  y0 = (*ptr_y++) <<21;
	  y1 = (*ptr_y++) <<21;

	  u0 = (*ptr_u++) - 128;
	  v0 = (*ptr_v++) - 128;
	  MMM(y0, u0, v0);
	  MMM2(y1);

	  //--------------------
	  //______________________________________
	  y0 = (*ptr_y++) <<21;
	  y1 = (*ptr_y++) <<21;
	  u0 = (*ptr_u++) - 128;
	  v0 = (*ptr_v++) - 128;
	  MMM(y0, u0, v0);
	  MMM2(y1);


//

	  w -= 4;
      }
    return 1;
}

//____________________________________________________________
// YUYV Version
//____________________________________________________________

uint8_t  COL_yv12rgb(uint32_t w, uint32_t h,uint8_t * ptr, uint8_t * ptr2 )
{
    uint32_t i, l;
    uint8_t *y, *u, *v, *e;

    l = w;			// line width in byte
    y = ptr;
    v = y + w * h;
    u = v + ((w * h) >> 2);
    e = ptr2;

    for (i = (h>>1); i > 0; i--)
      {
#ifdef USE_MMX
	  fastYV12RGB_mmx(y, u, v, e, w);
	  //fastYV12RGB_mmx(y+w, u, v, e+3*w, w);
#elif defined(HAVE_ALTIVEC) && defined(CONFIG_DARWIN)

	 altivecYV12RGB(y,
		    	u,
		    	v, e, w);		

#else
	  slowYV12RGB(y, u, v, e, w);
	  slowYV12RGB(y+w, u, v, e+4*w, w);
#endif

		y += w<<1;
		u += w >> 1;
		v += w >> 1;
		e += 8 * w;
      }
  return 1;
}

//____________________________________________
// Swap Red and Blue component
// Usefull for bmp for example
//____________________________________________________
void swapRnB (uint8_t * ptr, uint32_t w)
{
  uint8_t r;
  while (w-- > 0)
    {
      r = *ptr;
      *ptr = *(ptr + 2);
      *(ptr + 2) = r;
      ptr += 3;
    }

}




//
