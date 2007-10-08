
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

#include "ADM_encoder/ADM_vidEncode.hxx"

#include "ADM_codecs/ADM_x264param.h"
/**
      \fn DIA_x264
      \brief Dialog for x264 codec settings
*/
uint8_t DIA_x264(COMPRES_PARAMS *config)
{
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
           diaElemTabs tabMain(QT_TR_NOOP("Main"),1,main);

           
        /* Tab 2 Motion */


         diaMenuEntry partitionM[] = {
                  {0,   QT_TR_NOOP("1  - Extremely Low (Fastest)")}
                ,{1,    QT_TR_NOOP("2  - Very Low")}
                ,{3,    QT_TR_NOOP("3  - Low")}
                ,{4,    QT_TR_NOOP("4  - Medium")}
                ,{5,    QT_TR_NOOP("5  - High (Default)")}
                ,{6,    QT_TR_NOOP("6  - Very High")}
                ,{7,    QT_TR_NOOP("6B - Very High (RDO on Bframes)")}
                ,{8,    QT_TR_NOOP("7 - Ultra High")}
                ,{9,    QT_TR_NOOP("7B - Ultra High (RDO on Bframes)")}};

                            
        diaElemMenu parition(PX(PartitionDecision),QT_TR_NOOP("Partition Decision"),10,partitionM);
        
         diaMenuEntry meM[] = {
                             {0,    QT_TR_NOOP("Diamond search")},
                             {1,    QT_TR_NOOP("Hexagonal search")},
                             {2,    QT_TR_NOOP("Uneven multi hexagon")},
                             {3,    QT_TR_NOOP("Exhaustive search")}}
                             ;
         diaElemMenu      me(PX(Method),QT_TR_NOOP("VHQ Mode"),4,meM);
         diaElemUInteger  rframe(PX(MaxRefFrames),QT_TR_NOOP("Max Ref Frames"),0,15);
         diaElemUInteger  range(PX(Range),QT_TR_NOOP("Max B Frames"),0,64);
         diaElemToggle    chromaMe(PX(ChromaME),QT_TR_NOOP("Chroma ME"));
         diaElemToggle    mixedRef(PX(MixedRefs),QT_TR_NOOP("Mixed Refs"));
         diaElemToggle    fastPSkip(PX(fastPSkip),QT_TR_NOOP("Fast P Skip"));
         diaElemToggle    dctDecimate(PX(DCTDecimate),QT_TR_NOOP("DCT Decimate"));
         diaElemToggle    interlaced(PX(interlaced),QT_TR_NOOP("Interlaced"));
         
         diaElemFrame  frameMe(QT_TR_NOOP("Motion Estimation"));
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
          diaElemTabs tabMotion(QT_TR_NOOP("Motion"),1,motions);
         
         
        /* Tab3-Misc  */
           diaMenuEntry trellisM[] = {
                  {0,   QT_TR_NOOP("Disabled")}
                ,{1,    QT_TR_NOOP("Low")}
                ,{2,    QT_TR_NOOP("High")}};
           
          diaElemMenu trellis(PX(Trellis),QT_TR_NOOP("Trellis"),3,trellisM);
          diaElemToggle    cabac(PX(CABAC),QT_TR_NOOP("CABAC"));
          diaElemUInteger  noise(PX(NoiseReduction),QT_TR_NOOP("Noise Reduction"),0,255);
          diaElemToggle    deblock(PX(DeblockingFilter),QT_TR_NOOP("Deblocking Filter"));
          diaElemInteger  deblockStrength(PX(Strength),QT_TR_NOOP("Strength"),-6,6);
          diaElemInteger  deblockThreshold(PX(Threshold),QT_TR_NOOP("Threshold"),-6,6);
          deblock.link(1,&deblockStrength);
          deblock.link(1,&deblockThreshold);
          
          
          diaElemFrame  frameMisc(QT_TR_NOOP("Misc"));
          frameMisc.swallow(&trellis);
          frameMisc.swallow(&cabac);
          frameMisc.swallow(&noise);
          frameMisc.swallow(&deblock);
          frameMisc.swallow(&deblockStrength);
          frameMisc.swallow(&deblockThreshold);
            
           diaElem *misc[]={&frameMisc};
          diaElemTabs tabMisc(QT_TR_NOOP("Misc"),1,misc);
        /* Tab 4 Partition & frame*/
         
             diaElemToggle    _8x8(PX(_8x8),QT_TR_NOOP("8x8 Transform"));
             diaElemToggle    _8x8P(PX(_8x8P),QT_TR_NOOP("8x8P Transform"));
             diaElemToggle    _8x8B(PX(_8x8B),QT_TR_NOOP("8x8B Transform"));
             diaElemToggle    _4x4(PX(_4x4),QT_TR_NOOP("_4x4 Transform"));
             diaElemToggle    _8x8I(PX(_8x8I),QT_TR_NOOP("_8x8I Transform"));
             diaElemToggle    _4x4I(PX(_4x4I),QT_TR_NOOP("_4x4I Transform"));
          diaElemFrame  frameTransform(QT_TR_NOOP("Transform"));
           frameTransform.swallow(&_8x8);
           frameTransform.swallow(&_8x8P);
           frameTransform.swallow(&_8x8B);
           frameTransform.swallow(&_4x4);
           frameTransform.swallow(&_8x8I);
           frameTransform.swallow(&_4x4I);
          
           diaElemUInteger  bframe(PX(MaxBFrame),QT_TR_NOOP("Max B Frames"),0,3);
           diaElemFrame frameB(QT_TR_NOOP("B Frames"));
           frameB.swallow(&bframe);
           
            diaElem *bfr[]={&frameTransform,&frameB};
          diaElemTabs tabTransform(QT_TR_NOOP("Transform"),2,bfr);
           
          /* End of tabs */
        diaElemTabs *tabs[4]={&tabMain,&tabMotion,&tabMisc,&tabTransform};
        if( diaFactoryRunTabs(QT_TR_NOOP("X264 Configuration"),4,tabs))
	{
           memcpy(config->extraSettings,&localParam,sizeof(localParam));
           return 1;
        }
         return 0;
}

// EOF
