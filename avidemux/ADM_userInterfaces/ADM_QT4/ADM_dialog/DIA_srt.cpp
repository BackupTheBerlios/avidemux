
// Author: mean <fixounet@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>
#include <iconv.h>
#include "default.h"

#include "ADM_toolkit/toolkit.hxx"

#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h" 
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"



#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_encoder/ADM_vidEncode.hxx"
//#include "ADM_video/ADM_vidFont.h"
class ADMfont;
#include "ADM_videoFilter/ADM_vidSRT.h"
/**
      \fn DIA_x264
      \brief Dialog for x264 codec settings
*/
uint8_t DIA_srt(ADMImage *source, SUBCONF *param)
{
#if 0
int b;
int ret=0;
int code;
      ADM_x264Param localParam;
      ADM_assert(config->extraSettingsLen==sizeof(localParam));
      memcpy(&localParam,config->extraSettings,sizeof(localParam));
#define PX(x) &(localParam.x)
         
      
      // Our tabs
         /* Tab 1 main */
           diaElemBitrate bitrate(config,NULL);
           bitrate.setMaxQz(51);
           diaElem *main[]={&bitrate};
           diaElemTabs tabMain("Main",1,main);

           
        /* Tab 2 Motion */


         diaMenuEntry partitionM[] = {
                  {0,   _("1  - Extremely Low (Fastest)")}
                ,{1,    _("2  - Very Low")}
                ,{3,    _("3  - Low")}
                ,{4,    _("4  - Medium")}
                ,{5,    _("5  - High (Default)")}
                ,{6,    _("6  - Very High")}
                ,{7,    _("6B - Very High (RDO on Bframes)")}
                ,{8,    _("7 - Ultra High")}
                ,{9,    _("7B - Ultra High (RDO on Bframes)")}};

                            
        diaElemMenu parition(PX(PartitionDecision),_("Partition Decision"),10,partitionM);
        
         diaMenuEntry meM[] = {
                             {0,    _("Diamond search")},
                             {1,    _("Hexagonal search")},
                             {2,    _("Uneven multi hexagon")},
                             {3,    _("Exhaustive search")}}
                             ;
         diaElemMenu      me(PX(Method),_("VHQ Mode"),4,meM);
         diaElemUInteger  rframe(PX(MaxRefFrames),_("Max Ref Frames"),0,15);
         diaElemUInteger  range(PX(Range),_("Max B Frames"),0,64);
         diaElemToggle    chromaMe(PX(ChromaME),_("Chroma ME"));
         diaElemToggle    mixedRef(PX(MixedRefs),_("Mixed Refs"));
         diaElemToggle    fastPSkip(PX(fastPSkip),_("Fast P Skip"));
         diaElemToggle    dctDecimate(PX(DCTDecimate),_("DCT Decimate"));
         diaElemToggle    interlaced(PX(interlaced),_("Interlaced"));
         
         diaElemFrame  frameMe(_("Motion Estimation"));
         frameMe.swallow(&parition);
         frameMe.swallow(&me);
         frameMe.swallow(&rframe);
         frameMe.swallow(&range);
         frameMe.swallow(&chromaMe);
         frameMe.swallow(&mixedRef);
         frameMe.swallow(&fastPSkip);
         frameMe.swallow(&dctDecimate);
         frameMe.swallow(&interlaced);
         
           diaElem *motions[]={&frameMe};
          diaElemTabs tabMotion("Motion",1,motions);
         
         
        /* Tab3-Misc  */
           diaMenuEntry trellisM[] = {
                  {0,   _("Disabled")}
                ,{1,    _("Low")}
                ,{2,    _("High")}};
           
          diaElemMenu trellis(PX(Trellis),_("Trellis"),3,trellisM);
          diaElemToggle    cabac(PX(CABAC),_("CABAC"));
          diaElemUInteger  noise(PX(NoiseReduction),_("Noise Reduction"),0,255);
          diaElemToggle    deblock(PX(DeblockingFilter),_("Deblocking Filter"));
          diaElemInteger  deblockStrength(PX(Strength),_("Strength"),-6,6);
          diaElemInteger  deblockThreshold(PX(Threshold),_("Threshold"),-6,6);
          deblock.link(1,&deblockStrength);
          deblock.link(1,&deblockThreshold);
          
          
          diaElemFrame  frameMisc(_("Misc"));
          frameMisc.swallow(&trellis);
          frameMisc.swallow(&cabac);
          frameMisc.swallow(&noise);
          frameMisc.swallow(&deblock);
          frameMisc.swallow(&deblockStrength);
          frameMisc.swallow(&deblockThreshold);
            
           diaElem *misc[]={&frameMisc};
          diaElemTabs tabMisc("Misc",1,misc);
        /* Tab 4 Partition & frame*/
         
             diaElemToggle    _8x8(PX(_8x8),_("8x8 Transform"));
             diaElemToggle    _8x8P(PX(_8x8P),_("8x8P Transform"));
             diaElemToggle    _8x8B(PX(_8x8B),_("8x8B Transform"));
             diaElemToggle    _4x4(PX(_4x4),_("_4x4 Transform"));
             diaElemToggle    _8x8I(PX(_8x8I),_("_8x8I Transform"));
             diaElemToggle    _4x4I(PX(_4x4I),_("_4x4I Transform"));
          diaElemFrame  frameTransform(_("Transform"));
           frameTransform.swallow(&_8x8);
           frameTransform.swallow(&_8x8P);
           frameTransform.swallow(&_8x8B);
           frameTransform.swallow(&_4x4);
           frameTransform.swallow(&_8x8I);
           frameTransform.swallow(&_4x4I);
          
           diaElemUInteger  bframe(PX(MaxBFrame),_("Max B Frames"),0,3);
           diaElemFrame frameB(_("B Frames"));
           frameB.swallow(&bframe);
           
            diaElem *bfr[]={&frameTransform,&frameB};
          diaElemTabs tabTransform(_("Transform"),2,bfr);
           
          /* End of tabs */
        diaElemTabs *tabs[4]={&tabMain,&tabMotion,&tabMisc,&tabTransform};
        if( diaFactoryRunTabs(_("X264 Configuration"),4,tabs))
	{
           memcpy(config->extraSettings,&localParam,sizeof(localParam));
           return 1;
        }
#endif
         return 0;
}

// EOF
