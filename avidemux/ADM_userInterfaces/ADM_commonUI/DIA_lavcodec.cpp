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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>
#include <math.h>
#include <iconv.h>
#include "config.h"


#include "fourcc.h"
#include "avio.hxx"

#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h" 
#include "ADM_lavcodec.h"
#include "ADM_codecs/ADM_ffmpegConfig.h"
#include "ADM_encoder/ADM_vidEncode.hxx"
/**
      \fn getFFCompressParams
      \brief Dialog for lavcodec mpeg4/... codec settings
*/
uint8_t getFFCompressParams(COMPRES_PARAMS *incoming)
{
int ret=0;	
	FFcodecSetting *conf=(FFcodecSetting *)incoming->extraSettings;
	ADM_assert(incoming->extraSettingsLen==sizeof(FFcodecSetting));
#define PX(x) &(conf->x)
         
         
         
diaMenuEntry meE[]={
  {1,_("None")},
  {2,_("Full")},
  {3,_("Log")},
  {4,_("Phods")},
  {5,_("EPZS")},
  {6,_("X1")}
};       

diaMenuEntry qzE[]={
  {0,_("H.263")},
  {1,_("MPEG")}
};       

diaMenuEntry rdE[]={
  {0,_("MB comparison")},
  {1,_("Fewest bits (vhq)")},
  {2,_("Rate distortion")}
};     

uint32_t me=(uint32_t)conf->me_method;  

         diaElemBitrate bitrate(incoming,NULL);
         diaElemMenu      meM(&me,_("Matrices"),4,meE);
         diaElemUInteger  qminM(PX(qmin),_("Mi_n. quantizer:"),1,31);
         diaElemUInteger  qmaxM(PX(qmax),_("Ma_x. quantizer:"),1,31);
         diaElemUInteger  qdiffM(PX(max_qdiff),_("Max. quantizer _difference:"),1,31);
         
         diaElemToggle    fourMv(PX(_4MV),_("4_MV"));
         diaElemToggle    trellis(PX(_TRELLIS_QUANT),_("_Trellis quantization"));
         
         diaElemToggle    qpel(PX(_QPEL),_("_Quarter pixel"));
         diaElemToggle    gmc(PX(_GMC),_("_GMC"));

         
         diaElemUInteger  max_b_frames(PX(max_b_frames),_("_Number of B frames:"),0,32);
         diaElemMenu     qzM(PX(mpeg_quant),_("_Quantization type:"),2,qzE);
         
         diaElemMenu     rdM(PX(mb_eval),_("_Macroblock decision:"),3,rdE);
         
         diaElemUInteger filetol(PX(vratetol),_("_Filesize tolerance (kb):"),0,100000);
         
         diaElemFloat    qzComp(PX(qcompress),_("_Quantizer compression:"),0,1);
         diaElemFloat    qzBlur(PX(qblur),_("Quantizer _blur:"),0,1);
         
         
          /* First Tab : encoding mode */
        diaElem *diamode[]={&bitrate};
        diaElemTabs tabMode(_("User Interface"),1,diamode);
        
        /* 2nd Tab : ME */
        diaElemFrame frameMe(_("Advanced Simple Profile"));
        
        frameMe.swallow(&max_b_frames);
        frameMe.swallow(&qpel);
        frameMe.swallow(&gmc);
        
        diaElem *diaME[]={&fourMv,&frameMe};
        diaElemTabs tabME(_("Motion Estimation"),2,diaME);
        /* 3nd Tab : Qz */
        
         diaElem *diaQze[]={&qzM,&rdM,&qminM,&qmaxM,&qdiffM,&trellis};
        diaElemTabs tabQz(_("Quantization"),6,diaQze);
        
        /* 4th Tab : RControl */
        
         diaElem *diaRC[]={&filetol,&qzComp,&qzBlur};
        diaElemTabs tabRC(_("Rate Control"),3,diaRC);
        
         diaElemTabs *tabs[]={&tabMode,&tabME,&tabQz,&tabRC};
        if( diaFactoryRunTabs(_("libavcodec MPEG-4 configuration"),4,tabs))
	{
          conf->me_method=(Motion_Est_ID)me;
          return 1;
        }
         return 0;
}
// EOF
