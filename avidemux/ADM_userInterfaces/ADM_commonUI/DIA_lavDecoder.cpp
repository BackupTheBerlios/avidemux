//
// C++ Implementation: ADM_vidForcedPP
//
// Description: 
//
//	Force postprocessing assuming constant quant & image type
//	Uselefull on some badly authored DVD for example
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include <config.h>


#include <string.h>
#include <stdio.h>
#include <math.h>
#include "ADM_lavcodec.h"
#include "default.h"
#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidASharp_param.h"
#include "DIA_flyDialog.h"
#include "DIA_flyAsharp.h"
#include "ADM_assert.h"


#include "ADM_toolkit/toolkit.hxx"

#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
#include "ADM_codecs/ADM_ffmpegConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"
/**
      \fn DIA_lavDecoder
      \brief Dialog for lavcodec *DECODER* option
*/
uint8_t DIA_lavDecoder(uint32_t *swapUv, uint32_t *showU)
{
int ret=0;	
         
         diaElemToggle    swap(swapUv,_("Swap U & V:"));
         diaElemToggle    show(showU,_("Show _Vectors:"));
         diaElem *tabs[]={&swap,&show};
        if( diaFactoryRun(_("Lavcodec Decoder Configuration"),2,tabs))
	{
          return 1;
        }
         return 0;
}
// EOF
