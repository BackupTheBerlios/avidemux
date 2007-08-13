/***************************************************************************
                          ADM_guiContrast.cpp  -  description
                             -------------------
    begin                : Mon Sep 23 2002
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
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "default.h"
#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidContrast.h"
#include "DIA_flyDialog.h"
#include "DIA_flyEq2.h"
#include "ADM_osSupport/ADM_cpuCap.h"
/************* COMMON PART *********************/
uint8_t  flyEq2::update(void)
{
    download();
    process();
    copyYuvFinalToRgb();
    display();
    return 1;
}

uint8_t flyEq2::process(void)

{
	Eq2Settings mySettings;
	uint8_t *src,*dst;
	uint32_t stride;
#if 0
	printf("Contrast   :%f\n",param.contrast);
	printf("brightness :%f\n",param.brightness);
	printf("saturation :%f\n",param.saturation);
	printf("rgamma :%f\n",param.rgamma);
	printf("bgamma :%f\n",param.bgamma);
	printf("ggamma :%f\n",param.ggamma);
#endif	
	
	        update_lut(&mySettings,&param);
#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
	        if(CpuCaps::hasMMX())
	        {
	                affine_1d_MMX(&(mySettings.param[0]),YPLANE(_yuvBufferOut),YPLANE(_yuvBuffer),_w,_h);
	                affine_1d_MMX(&(mySettings.param[2]),UPLANE(_yuvBufferOut),UPLANE(_yuvBuffer),_w>>1,_h>>1);
	                affine_1d_MMX(&(mySettings.param[1]),VPLANE(_yuvBufferOut),VPLANE(_yuvBuffer),_w>>1,_h>>1);       
	        }
	        else
#endif	
	        {
	        	apply_lut(&(mySettings.param[0]),YPLANE(_yuvBufferOut),YPLANE(_yuvBuffer),_w,_h);
	        	apply_lut(&(mySettings.param[2]),UPLANE(_yuvBufferOut),UPLANE(_yuvBuffer),_w>>1,_h>>1);
	        	apply_lut(&(mySettings.param[1]),VPLANE(_yuvBufferOut),VPLANE(_yuvBuffer),_w>>1,_h>>1);       
	        }
#if 1
	    // copy Y half
	    dst=YPLANE(_yuvBufferOut);
	    src=YPLANE(_yuvBuffer);
	    stride=_w;
	    for(uint32_t y=0;y<_h;y++)   // We do both u & v!
	    {
	        memcpy(dst,src,stride>>1);
	        dst+=stride;
	        src+=stride;
	    }
	        // U
	    dst=UPLANE(_yuvBufferOut);
	    src=UPLANE(_yuvBuffer);
	    stride=_w>>1;
	    uint32_t h2=_h>>1;
	    for(uint32_t y=0;y<h2;y++)   // We do both u & v!
	    {
	        memcpy(dst,src,stride>>1);
	        dst+=stride;
	        src+=stride;
	    }
	        // V
	    dst=VPLANE(_yuvBufferOut);
	    src=VPLANE(_yuvBuffer);
	    for(uint32_t y=0;y<h2;y++)   // We do both u & v!
	    {
	        memcpy(dst,src,stride>>1);
	        dst+=stride;
	        src+=stride;
	    }
#endif
		return 1;
}

/************* COMMON PART *********************/
