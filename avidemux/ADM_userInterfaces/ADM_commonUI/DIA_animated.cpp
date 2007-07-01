//
//
// C++ Implementation: DIA_SVCD
//
// Description: 
//
//
// Author: mean <fixounet@free.fr>, (C) 2003
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <config.h>
//#include "ADM_lavcodec.h"


#include <string.h>
#include <stdio.h>

# include <math.h>

#include "default.h"
#include "ADM_videoFilter/ADM_vidAnimated_param.h"
#include "ADM_assert.h" 
#include "DIA_factory.h"

/**
      \fn DIA_animated
      \brief Dialog to set params for the animated filter
*/
uint8_t DIA_animated(ANIMATED_PARAM *param,uint32_t w, uint32_t h,uint32_t n)
{	
  uint8_t r=0;
#define PX(x) &(param->x)
   diaElemFile      jpeg(0,(char **)PX(backgroundImg),_("_Background Image:"));
   diaElemToggle    isNtsc(PX(isNTSC),_("_NTSC(default is Pal):"));
   diaElemUInteger   vignetteW(PX(vignetteW),_("Vignette _Width:"),16,w/3);
   diaElemUInteger   vignetteH(PX(vignetteH),_("Vignette _Height:"),16,h/2);
   
   diaElemUInteger *timecode[MAX_VIGNETTE];
   diaElemFrame timecodes(_("Vignette frame number"));

   for(int i=0;i<MAX_VIGNETTE;i++)
   {
     timecode[i]=new diaElemUInteger(&(param->timecode[i]),_("Timecode:"),0,n);
     timecodes.swallow(timecode[i]);
   }

 
      diaElem *elems[5]={&jpeg,&isNtsc,&vignetteW,&vignetteH,&timecodes};
    
  if( diaFactoryRun("MPEG-2 Configuration",5,elems))
  {
    r=1;
  }
  for(int i=0;i<MAX_VIGNETTE;i++)
  {
    delete timecode[i]; 
  }
  
  return r;
}	
// EOF
