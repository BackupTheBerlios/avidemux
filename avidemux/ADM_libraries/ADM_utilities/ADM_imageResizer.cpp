/***************************************************************************
    copyright            : (C) 2003-2005 by mean
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ADM_assert.h"
#include "default.h"

#include "ADM_image.h"
#include "ADM_osSupport/ADM_cpuCap.h"

#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
extern "C" {
#include "ADM_lavcodec/avcodec.h"
}
#endif
#include "ADM_lavutil/avutil.h"
#include "ADM_libswscale/swscale.h"


ADMImageResizer::ADMImageResizer(uint32_t ow,uint32_t oh, uint32_t dw, uint32_t dh)
{
int flags=0;

    orgWidth=ow;
    orgHeight=oh;
    destWidth=dw;
    destHeight=dh;

    flags=SWS_SPLINE;
#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
        #define ADD(x,y) if( CpuCaps::has##x()) flags|=SWS_CPU_CAPS_##y;
        ADD(MMX,MMX);		
        ADD(3DNOW,3DNOW);
        ADD(MMXEXT,MMX2);
#endif	
#ifdef USE_ALTIVEC
        flags|=SWS_CPU_CAPS_ALTIVEC;
#endif
            _context=(void *)sws_getContext(
                        orgWidth,orgHeight,
                        PIX_FMT_YUV420P,
                        destWidth,destHeight,
                        PIX_FMT_YUV420P,
                        flags, NULL, NULL,NULL);
            
}

ADMImageResizer::~ADMImageResizer()
{
   if(_context)
    {
        sws_freeContext((SwsContext*)_context);
    }
    _context=NULL;
}

uint8_t ADMImageResizer::resize(ADMImage *source,ADMImage *dest)
{
    ADM_assert(source->_width==orgWidth);
    ADM_assert(source->_height==orgHeight);
    ADM_assert(dest->_width==destWidth);
    ADM_assert(dest->_height==destHeight);


uint8_t *src[3];
uint8_t *dst[3];
int ssrc[3];
int ddst[3];

                src[0]=YPLANE(source);
                src[1]=UPLANE(source);
                src[2]=VPLANE(source);

                ssrc[0]=orgWidth;
                ssrc[1]=ssrc[2]=orgWidth>>1;

         
                dst[0]=YPLANE(dest);
                dst[1]=UPLANE(dest);
                dst[2]=VPLANE(dest);
                ddst[0]=destWidth;
                ddst[1]=ddst[2]=destWidth>>1;

                sws_scale((SwsContext*)_context,src,ssrc,0,orgHeight,dst,ddst);
                return 1;

}
