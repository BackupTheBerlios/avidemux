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

#include "ADM_codecs/ADM_codec.h"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_assert.h" 
#include "DIA_factory.h"
#include "../../ADM_libraries/ADM_libmpeg2enc/ADM_mpeg2enc.h"

/**
      \fn DIA_SVCDParam
      \brief Dialog to set encoding options for SVCD/DVD mpeg2enc based
*/
uint8_t DIA_SVCDParam(COMPRES_PARAMS *incoming)
{	

Mpeg2encParam *conf=(Mpeg2encParam *)incoming->extraSettings;
ADM_assert(incoming->extraSettingsLen==sizeof(Mpeg2encParam));

diaMenuEntry wideM[]={
  {0,_("4:3")},
  {1,_("16:9")}};
diaMenuEntry matrixM[]={
  {0,_("Default")},
  {1,_("TMPGEnc")},
  {2,_("Anime")},
  {3,_("KVCD")}
};
diaMenuEntry interM[]={
  {0,_("Progressive")},
  {1,_("Interlaced TFF")},
  {2,_("Interlaced BFF")}
};
                      
         diaElemBitrate bitrate(incoming,NULL);
         diaElemMenu      widescreen(&(conf->widescreen),_("Aspect _ratio:"),2,wideM);
         diaElemMenu      matrix(&(conf->user_matrix),_("_Matrices:"),4,matrixM);
         diaElemUInteger gop(&(conf->gop_size),_("_GOP size:"),1,30);
         diaElemUInteger maxb(&(conf->maxBitrate),_("Ma_x. bitrate:"),100,9000);

uint32_t inter;
          if(!conf->interlaced) inter=0;
            else if(conf->bff) inter=2;
                else inter=1;
         diaElemMenu      interW(&inter,_("_Interlacing:"),3,interM);
  
      diaElem *elems[6]={&bitrate,&widescreen,&interW,&matrix,&gop,&maxb};
    
  if( diaFactoryRun("MPEG-2 Configuration",6,elems))
  {
    switch(inter)
    {
      case 0: conf->interlaced=0;conf->bff=0;break;
      case 1: conf->interlaced=1;conf->bff=0;break;
      case 2: conf->interlaced=1;conf->bff=1;break;
      default: ADM_assert(0);
    }
    return 1;
  }
  return 0;
}	
// EOF
