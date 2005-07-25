/***************************************************************************
                         ADM_Rgb : wrapper for yv12->RGB display
                            using mplayer postproc


    begin                : Thu Apr 16 2003
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

#include "ADM_library/default.h"
#include <ADM_assert.h>

#include "colorspace.h"
#include "ADM_toolkit/ADM_cpuCap.h"
#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
extern "C" {
#include "../../adm_lavcodec/avcodec.h"
}
#endif

#include "../MPlayer_pp/img_format.h"
#include "../MPlayer_pp/swscale.h"
#include "ADM_rgb.h" 

#define CLEANUP() \
    if(_context) \
    {\
        if(ww==w || hh==h)\
           return 1; \
        clean();  \
    }

#if (defined( ARCH_X86)  || defined(ARCH_X86_64))		
		#define ADD(x,y) if( CpuCaps::has##x()) flags|=SWS_CPU_CAPS_##y;
#define FLAGS()		ADD(MMX,MMX);				ADD(3DNOW,3DNOW);		ADD(MMXEXT,MMX2);
#else
#define FLAGS()
#endif


 //***********************************************
 ColBase::ColBase(uint32_t ww, uint32_t hh)
 {
      _context=NULL;
      w=0xfffff;
      h=0xfffff;
      //reset(ww,hh);   
 }
 ColBase::~ColBase()
 {
    clean();   
 }
 //***********************************************
 uint8_t ColBase::clean(void)
 {
        if(_context)
		{
			sws_freeContext((SwsContext *)_context);
		}
		_context=NULL; 
		return 1;  
 }
 //***********************************************
 uint8_t ColYuvRgb::reset(uint32_t ww, uint32_t hh)
 {
 int flags=0;
	
    CLEANUP();
    FLAGS();
    
	 _context=(void *)sws_getContext(
				    		ww,hh,
						IMGFMT_YV12 ,
		 				ww,hh,
	   					IMGFMT_RGB32,
	    					flags, NULL, NULL,NULL);

    if(!_context) ADM_assert(0);
    w=ww;
    h=hh;
    return 1;
}

 uint8_t ColYuvRgb::scale(uint8_t *src, uint8_t *target)
 {
    uint8_t *srd[3];
	uint8_t *dst[3];
	int ssrc[3];
	int ddst[3];

	ADM_assert(_context);
	
			uint32_t page;

			page=w*h;
			srd[0]=src;
			srd[1]=src+page;
			srd[2]=src+((page*5)>>2);

			ssrc[0]=w;
			ssrc[1]=ssrc[2]=w>>1;

			
			dst[0]=target;
			dst[1]=NULL;
			dst[2]=NULL;
			ddst[0]=w*4;
			ddst[1]=ddst[2]=0;

			sws_scale((SwsContext *)_context,srd,ssrc,0,h,dst,ddst);
     
        return 1;
 }

//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
uint8_t ColYv12Rgb24::reset(uint32_t ww, uint32_t hh)
 {
 int flags=0;
	CLEANUP();
    FLAGS();
   
	 _context=(void *)sws_getContext(
				    		ww,hh,
						IMGFMT_YV12 ,
		 				ww,hh,
	   					IMGFMT_RGB24,
	    					flags, NULL, NULL,NULL);

    if(!_context) ADM_assert(0);
    w=ww;
    h=hh;
    return 1;
}
  uint8_t ColYv12Rgb24::scale(uint8_t *src, uint8_t *target)
 {
    uint8_t *srd[3];
	uint8_t *dst[3];
	int ssrc[3];
	int ddst[3];

	ADM_assert(_context);
	
			uint32_t page;

			page=w*h;
			srd[0]=src;
			srd[1]=src+page;
			srd[2]=src+((page*5)>>2);

			ssrc[0]=w;
			ssrc[1]=ssrc[2]=w>>1;

			
			dst[0]=target;
			dst[1]=NULL;
			dst[2]=NULL;
			ddst[0]=w*3;
			ddst[1]=ddst[2]=0;
			
			sws_scale((SwsContext *)_context,srd,ssrc,0,h,dst,ddst);
     
        return 1;
 }
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 uint8_t ColRgbToYV12::reset(uint32_t ww, uint32_t hh)
 {
 int flags=0;
 int c;	
    clean();
    FLAGS();
    if(_colorspace==ADM_COLOR_RGB24) c=IMGFMT_RGB24;
            else                     c=IMGFMT_RGB32;
	 _context=(void *)sws_getContext(
				    		ww,hh,
						c ,
		 				ww,hh,
	   					IMGFMT_YV12,
	    					flags, NULL, NULL,NULL);

    if(!_context) ADM_assert(0);
    w=ww;
    h=hh;
    return 1;
}
//***********************************************
 uint8_t ColRgbToYV12::scale(uint8_t *src, uint8_t *target)
 {
    uint8_t *srd[3];
	uint8_t *dst[3];
	int ssrc[3];
	int ddst[3];

	ADM_assert(_context);
	
			uint32_t page;

			page=w*h;
			srd[0]=src;
			srd[1]=0;
			srd[2]=0;
            if(_colorspace==ADM_COLOR_RGB24)
			    ssrc[0]=w*3;
			else
			    ssrc[0]=w*4;
			ssrc[1]=0;
			ssrc[2]=0;

			
			dst[0]=target;
			dst[1]=target+page;
			dst[2]=target+((page*5)>>2);
			ddst[0]=w;
			ddst[1]=ddst[2]=w>>1;

			sws_scale((SwsContext *)_context,srd,ssrc,0,h,dst,ddst);
     
        return 1;
 }
 uint8_t ColRgbToYV12::changeColorSpace(ADM_colorspace col)
 {
    if(col==_colorspace) return 1;
    _colorspace=col;
       
 }
 //*************************
static inline void SwapMe(uint8_t *tgt,uint8_t *src,int nb);
void SwapMe(uint8_t *tgt,uint8_t *src,int nb)
{
    uint8_t r,g,b;
   nb=nb;
   while(nb--)
   {
       r=*src++;
       g=*src++;
       b=*src++;
       *tgt++=b;
       *tgt++=g;
       *tgt++=r;
       
   }
   return;
    
}
//%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
 uint8_t  COL_yv12rgbBMP(uint32_t ww, uint32_t hh, uint8_t* in, uint8_t *out)
{
     ColYv12Rgb24 rgb(ww,hh);
     
        rgb.reset(ww,hh);
        rgb.scale(in,out);
        // Now time to swap it
        uint8_t swap[ww*3];
        
        uint8_t *up=out;
        uint8_t *down=out+(hh-1)*ww*3;
        
        for(int y=0;y<hh>>1;y++)
        {
            SwapMe(swap,up,ww); 
            SwapMe(up,down,ww);
            memcpy( down,swap,ww*3);
            down-=3*ww;
            up+=3*ww;
        }
        
        return 1;
     
}
