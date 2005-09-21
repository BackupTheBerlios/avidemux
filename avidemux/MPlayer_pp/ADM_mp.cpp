/***************************************************************************

    copyright            : (C) 2005 by mean
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
#include <ADM_assert.h>

#include "fourcc.h"
#include "avio.hxx"
#include "ADM_library/default.h"

#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
extern "C" {
#include "../../adm_lavcodec/avcodec.h"
}
#endif

#include "../MPlayer_pp/img_format.h"
#include "../MPlayer_pp/swscale.h"
#include "ADM_toolkit/ADM_cpuCap.h"

#include "ADM_mp.h"


#define Context ((SwsContext *)_context)

ADM_MplayerResize::ADM_MplayerResize(uint32_t fromw, uint32_t fromh, uint32_t tow, uint32_t toh)
{
SwsFilter                  *srcFilter=NULL;
SwsFilter                  *dstFilter=NULL;
int                       flags=0;

                           flags=SWS_BILINEAR;

#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
                
                #define ADD(x,y) if( CpuCaps::has##x()) flags|=SWS_CPU_CAPS_##y;
                ADD(MMX,MMX);           
                ADD(3DNOW,3DNOW);
                ADD(MMXEXT,MMX2);
#endif  
#ifdef USE_ALTIVEC
                flags|=SWS_CPU_CAPS_ALTIVEC;
#endif

                srcW=fromw;
                srcH=fromh;
                tgtW=tow;
                tgtH=toh;

                _context=(void *)sws_getContext(
                                                srcW,srcH,
                                                IMGFMT_YV12,
                                                tgtW,tgtH,
                                                IMGFMT_YV12,
                                                flags, srcFilter, dstFilter,NULL);

                if(!_context) ADM_assert(0);
}
ADM_MplayerResize::~ADM_MplayerResize()
{
        sws_freeContext(Context);
        _context=NULL;
}

uint8_t ADM_MplayerResize::resize(uint8_t *datasrc, uint8_t *datadst)
{
uint8_t *src[3];
uint8_t *dst[3];
int ssrc[3];
int ddst[3];

uint32_t page;

                        page=srcW*srcH;
                        src[0]=datasrc;
                        src[1]=datasrc+page;
                        src[2]=datasrc+((page*5)/4);

                        ssrc[0]=srcW;
                        ssrc[1]=ssrc[2]=srcW>>1;

                        page=tgtW*tgtH;
                        dst[0]=datadst;
                        dst[1]=datadst+page;
                        dst[2]=datadst+(page*5)/4;
                        ddst[0]=tgtW;
                        ddst[1]=ddst[2]=tgtW>>1;

                        sws_scale(Context,src,ssrc,0,srcH,dst,ddst);

                        return 1;
}
