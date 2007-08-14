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

#if 0
	printf("Contrast   :%f\n",param.contrast);
	printf("brightness :%f\n",param.brightness);
	printf("saturation :%f\n",param.saturation);
	
	printf("gamma_weight :%f\n",param.gamma_weight);
	printf("gamma :%f\n",param.gamma);
	
	
	
	printf("rgamma :%f\n",param.rgamma);
	printf("bgamma :%f\n",param.bgamma);
	printf("ggamma :%f\n",param.ggamma);
	printf("******************\n");
#endif	
	
	        update_lut(&mySettings,&param);
	        
typedef void lutMeType(oneSetting *par, unsigned char *dst, unsigned char *src, unsigned int w, unsigned int h);

			lutMeType *lutMe=apply_lut;
			

#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
	        if(CpuCaps::hasMMX())
	        {
	        		lutMe=affine_1d_MMX;
	        }
#endif	
	        lutMe(&(mySettings.param[0]),YPLANE(_yuvBufferOut),YPLANE(_yuvBuffer),_w,_h);
	        lutMe(&(mySettings.param[2]),UPLANE(_yuvBufferOut),UPLANE(_yuvBuffer),_w>>1,_h>>1);
	        lutMe(&(mySettings.param[1]),VPLANE(_yuvBufferOut),VPLANE(_yuvBuffer),_w>>1,_h>>1);       

	        	
	        	#if 1
	        _yuvBuffer->copyLeftSideTo(_yuvBufferOut);
#endif
		return 1;
}

/************* COMMON PART *********************/
