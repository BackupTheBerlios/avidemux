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
#include "ADM_videoFilter/ADM_vidTDeint_param.h"
/**
      \fn DIA_tdeint
      \brief Dialog for tdeint filter
*/

uint8_t  DIA_tdeint(TDEINT_PARAM *param)
{
         
         
diaMenuEntry meField[]={
  {0,_("Auto (might not work)")},
  {2,_("Top field first")},
  {1,_("Bottom field first")}
};       

diaMenuEntry meInterpolate[]={
  {0,_("Auto (might not work)")},
  {2,_("Bottom field (keep top)")},
  {1,_("Top field (keep bottom)")}
};       


diaMenuEntry meType[]={
  {0,_("Cubic interpolation")},
  {1,_("Modified ELA")},
  {2,_("Kernel interpolation")},
  {3,_("Modified ELA-2")},
  
};       

diaMenuEntry meMnt[]={
  {0,_("4 fields check")},
  {1,_("5 fields check")},
  {2,_("4 fields check (no avg)")},
  {3,_("5 fields check (no avg)")},
  
};       


diaMenuEntry meLink[]={
  {0,_("No link")},
  {1,_("Full link")},
  {2,_("Y to UV")},
  {3,_("UV to Y")}
};     

diaMenuEntry meAP[]={
  {0,_("0")},
  {1,_("1")},
  {2,_("2")}
};     
#define PX(x) &(param->x)
      uint32_t order=param->order+1;
      uint32_t field=param->field+1;
      
      
      diaElemMenu     menuFieldOrder(&(order),_("_Field order:"),3,meField);
      diaElemMenu     menuInterpolaye(&(field),_("_Interpolate:"),3,meInterpolate);
      
      diaElemMenu     menuType(PX(type),_("_Type:"),4,meType);
      diaElemMenu     menuMnt(PX(mtnmode),_("_MntMode:"),4,meMnt);
      diaElemMenu     menuLink(PX(link),_("_Link:"),4,meLink);
      diaElemMenu     menuAP(PX(APType),_("_AP type:"),3,meAP);

      // Toggle
      diaElemToggle    toggleUseChroma(PX(chroma),_("Use ch_roma to evalute"));
      diaElemToggle    toggleTryWeave(PX(tryWeave),_("Try _weave"));
      diaElemToggle    toggleDenoise(PX(denoise),_("_Denoise"));
      diaElemToggle    toggleSharp(PX(sharp),_("_Sharp"));
      diaElemToggle    toggleEvaluteAll(PX(full),_("_Evalute all frames"));
      
      // int
      diaElemUInteger  intMotionLuma(PX(mthreshL),_("Motion threshold, l_uma:"),0,255);
      diaElemUInteger  intMotionChroma(PX(mthreshC),_("Motion threshold, c_hroma:"),0,255);
      diaElemUInteger  intAreaCombing(PX(cthresh),_("Area com_bing threshold:"),0,255);
      diaElemUInteger  intCombed(PX(MI),_("Combe_d threshold:"),0,255);
      diaElemInteger   intArtefact(PX(AP),_("Artefact _protection threshold:"),-1,255);
      diaElemToggle    intDebug(PX(debug),_("Debug:"));

         diaElem *diaRC[]={&menuFieldOrder,&menuInterpolaye,&menuType,&menuMnt,&menuLink,&menuAP,
                            &toggleUseChroma,&toggleTryWeave,&toggleDenoise,&toggleSharp,&toggleEvaluteAll,
                            &intMotionLuma,&intMotionChroma,&intAreaCombing,&intCombed,&intArtefact,&intDebug
                  };

        
         
        if( diaFactoryRun(_("TDeint"),17,diaRC))
	{
           param->order=(int)order-1;
           param->field=(int)field-1;
          return 1;
        }
         return 0;
}
// EOF
