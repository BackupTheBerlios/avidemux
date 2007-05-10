
// Author: mean <fixounet@free.fr>, (C) 2007
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
#ifdef USE_XVID_4
#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_codecs/ADM_xvid4param.h"
/**
      \fn getFFCompressParams
      \brief Dialog for lavcodec mpeg4/... codec settings
*/
uint8_t DIA_xvid4(COMPRES_PARAMS *incoming)
{
int b;
int ret=0;
int code;
      xvid4EncParam localParam;
      ADM_assert(incoming->extraSettingsLen==sizeof(localParam));
      memcpy(&localParam,incoming->extraSettings,sizeof(localParam));
#define PX(x) &(localParam.x)
         // Our tabs
         /* Tab 1 main */
           diaElemBitrate bitrate(incoming,NULL);
#define MKTOGGLE(y,x)           diaElemToggle  t_##x(PX(x),#y)
        MKTOGGLE(Interlaced,	    interlaced);
        MKTOGGLE(Cartoon Mode,      cartoon);
        MKTOGGLE(Greyscale,         greyscale);
        MKTOGGLE(Cartoon Mode,      turbo);
        MKTOGGLE(Greyscale,         chroma_opt);
        diaElem *main[]={&bitrate,&t_interlaced,&t_cartoon,&t_greyscale,&t_turbo,&t_chroma_opt};
        diaElemTabs tabMain("Main",6,main);
         /* Tab 2 motion */
         diaMenuEntry motionMenu[] = {
                             {0,       _("None"),NULL}
                            ,{1,      _("Very Low"),NULL}
                            ,{2,      _("Low"),NULL}
                            ,{3,      _("Medium"),NULL}
                            ,{4,      _("High"),NULL}
                            ,{5,      _("Very High"),NULL}
                            ,{6,      _("Ultra High"),NULL}};
        diaElemMenu motion(PX(guiLevel),_("Motion Search Precision"),7,motionMenu);
        
         diaMenuEntry vhqMenu[] = {
                             {0,       _("Off"),NULL}
                            ,{1,      _("Mode Decision"),NULL}
                            ,{2,      _("Limited Search"),NULL}
                            ,{3,      _("Medium Search"),NULL}
                            ,{4,      _("Wide Search"),NULL} };
         diaElemMenu vhq(PX(vhqmode),_("VHQ Mode"),5,vhqMenu);
        
        
        /* Tab2-ASP */
          diaElemUInteger  bframe(PX(bframes),_("Max B Frames"),0,3);
          diaElemToggle    qpel(PX(qpel),_("Quarter Pixel"));
          diaElemToggle    gmc(PX(gmc),_("GMC"));
          diaElemToggle    bvhq(PX(bvhq),_("BVHQ"));
          diaElemFrame  frameASP(_("Advanced Simple Profile"));
          frameASP.swallow(&bframe);
          frameASP.swallow(&qpel);
          frameASP.swallow(&gmc);
          frameASP.swallow(&bvhq);
        
         
          /* Tab 2 motion extra */
            diaElemToggle    inter4mv(PX(inter4mv),_("4MV"));
            diaElemToggle    chroma_me(PX(chroma_me),_("Chroma ME"));
            diaElemToggle    hqac(PX(chroma_me),_("HQ AC"));
          diaElemFrame  frameMore(_("More Search"));
          frameMore.swallow(&inter4mv);
          frameMore.swallow(&chroma_me);
          frameMore.swallow(&hqac);
          /* Tab 2 gop size */
          diaElemUInteger  min_key_interval(PX(min_key_interval),_("Min Gop Size"),1,900);
          diaElemUInteger  max_key_interval(PX(max_key_interval),_("Max Gop Size"),1,900);
            diaElemFrame  frameGop(_("GOP Size"));
              frameGop.swallow(&min_key_interval);
              frameGop.swallow(&max_key_interval);
            
           diaElem *motions[]={&motion,&vhq,&frameMore,&frameGop,&frameASP};
          diaElemTabs tabMotion("Motion",5,motions);
        /* Tab 3 Qz*/
          diaMenuEntry qzMenu[] = {
                             {0,       _("H263"),NULL}
                            ,{1,      _("Mpeg"),NULL}}                            ;
           diaElemMenu h263(PX(mpegQuantizer),_("Quantization Matrix"),2,qzMenu);
           diaElemToggle    trellis(PX(trellis),_("Trellis Quantization"));
           
           
          diaElem *qz[]={&h263,&trellis};
          diaElemTabs tabQz(_("Quantization"),2,qz);
          
          /* Tab 4 : 2nd pass */
#define MKENTRY(y,x) diaElemUInteger  x(PX(x),_(#y),0,100);frameOne.swallow(&x);
        diaElemFrame  frameOne(_("Two Pass Tuning")); 
         
        MKENTRY(Key Frame Boost(%),	keyframe_boost		);
        
        MKENTRY(I-frames closer than...	,		kfthreshold			);
        MKENTRY(.. are reduced by(%),	kfreduction			);
        MKENTRY(Max Overflow Improvement(%),	max_overflow_improvement	);
        MKENTRY(Max Overglow Degradation(%),	max_overflow_degradation	);

#undef MKENTRY
#define MKENTRY(y,x) diaElemUInteger  x(PX(x),_(#y),0,100);frameTwo.swallow(&x);
        diaElemFrame  frameTwo(_("Curve Compression"));  

        MKENTRY(High Bitrate Scenes (%),curve_compression_high	);
        MKENTRY(Low Bitrate Scenes (%),curve_compression_low	);
        MKENTRY(Overflow Control Strength,overflow_control_strength	);
                
         diaElem *twopass[]={&frameOne,&frameTwo};
          diaElemTabs tabPass("Two Pass",2,twopass);
        /**/
        
        
          
          /* End of tabs */
        diaElemTabs *tabs[4]={&tabMain,&tabMotion,&tabQz,&tabPass};
        if( diaFactoryRunTabs(_("Xvid4 Configuration"),4,tabs))
	{
           memcpy(incoming->extraSettings,&localParam,sizeof(localParam));
           return 1;
        }
         return 0;
}
#endif
// EOF
