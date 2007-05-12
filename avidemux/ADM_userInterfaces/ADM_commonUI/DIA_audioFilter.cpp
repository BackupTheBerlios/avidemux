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
#include "ADM_audiofilter/audiofilter_normalize_param.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
/**
    \fn DIA_getAudioFilter
    \brief Dialog to manage audio filters
*/
int DIA_getAudioFilter(GAINparam *gain,
                        RESAMPLING *downsamplingmethod, 
                        int *tshifted,
                        int *shiftvalue, 
                        int *drc,
                        int *freqvalue,
                        FILMCONV *filmconv,
                        CHANNEL_CONF *channel)
{
  
  uint32_t vDRC=*drc;
  uint32_t vTshift=*tshifted;
  int32_t vTshiftValue=*shiftvalue;
  uint32_t vFreq=*freqvalue;
  uint32_t vChan=(uint32_t)*channel;
  uint32_t vDownsample=(uint32_t )*downsamplingmethod;
  uint32_t vFilm=(uint32_t )*filmconv;
  uint32_t vGainMode=(uint32_t)gain->mode;
  ELEM_TYPE_FLOAT vGainValue=gain->gain10;
  vGainValue/=10.;
  //**********************************
  diaMenuEntry menuTimeShift[]={
  {0,_("No Timeshift")},
  {1,_("TimeShift activated")}};
       
   diaElemToggle    eDRC(&vDRC,_("_Dynamic Range Compression:"));
   diaElemToggle    eTimeShift(&vTshift,_("_TimeShift:"));
   diaElemInteger  eShift(&vTshiftValue,_("TimeShift _Value:"),-10000,10000);
   
   eTimeShift.link(1,&eShift);
  
    diaElemFrame frameMisc(_("Misc"));
    frameMisc.swallow(&eDRC);
    frameMisc.swallow(&eTimeShift);
    frameMisc.swallow(&eShift);
    //**********************************
   
    diaElemToggle      eResample(&vDownsample,_("_Resampling:"));
    diaElemUInteger  eResampleValue(&vFreq,_("New Frequency:"),6000,64000);
  
    eResample.link(1,&eResampleValue);
    
       diaElemFrame frameResampling(_("Resampling"));
    frameResampling.swallow(&eResample);
    frameResampling.swallow(&eResampleValue);
    //**********************************
    diaMenuEntry menuFPS[]={
  {FILMCONV_NONE,     _("None")},
  {FILMCONV_FILM2PAL, _("Film to Pal")},
  {FILMCONV_PAL2FILM, _("Pal to Film")}
    };
  
   diaElemMenu      eFPS(&vFilm,_("_Fps convert:"),3,menuFPS);
      diaElemFrame frameFPS(_("Framerate Change"));
    frameFPS.swallow(&eFPS);
   //**********************************
    diaMenuEntry menuGain[]={
  {ADM_NO_GAIN,       _("None")},
  {ADM_GAIN_AUTOMATIC,_("Automatic (max -3 dB)")},
  {ADM_GAIN_MANUAL,   _("Manual")}};
  
   diaElemMenu      eGain(&vGainMode,_("Gain Mode:"),3,menuGain);
   
    diaElemFloat  eGainValue(&vGainValue,_("_Gain Value:"),-10,10);
     eGain.link(&(menuGain[2]),1,&eGainValue);
   diaElemFrame frameGain(_("Gain"));   
    frameGain.swallow(&eGain);
    frameGain.swallow(&eGainValue);
  //********************************
    diaMenuEntry menuMixer[]={
  {CHANNEL_INVALID,     _("No Change")},
  {CHANNEL_MONO,        _("Mono")},
  {CHANNEL_STEREO,      _("Stereo")},
  {CHANNEL_2F_1R,       _("Stereo+Surround")},
  {CHANNEL_3F,          _("Stereo+Center")},
  {CHANNEL_3F_1R,           _("Stereo+Center+Surround")},
  {CHANNEL_2F_2R,           _("Stereo front+Stereo Rear")},
  {CHANNEL_3F_2R,           _("5 Channels")},
  {CHANNEL_3F_2R_LFE,       _("5.1 ")},
  {CHANNEL_DOLBY_PROLOGIC,  _("Dolby Prologic")},
  {CHANNEL_DOLBY_PROLOGIC2, _("Dolby Prologic II")}
    };

 diaElemMenu      eMixer(&vChan,_("Mixer:"),11,menuMixer);
diaElemFrame frameMixer(_("Mixer"));   
    frameMixer.swallow(&eMixer);
 
 /************************************/
   diaElem *elems[]={&frameMisc,&frameResampling,&frameFPS,&frameGain,&frameMixer};
  if( diaFactoryRun(_("Audio Filter Configuration"),5,elems))
    {
        *drc=vDRC;
        *tshifted=vTshift;
        *shiftvalue=vTshiftValue;
        *freqvalue=vFreq;
        *channel=(CHANNEL_CONF)vChan;
        *downsamplingmethod=(RESAMPLING)vDownsample;
        *filmconv=(FILMCONV)vFilm;
        gain->mode=(ADM_GAINMode)vGainMode;
        gain->gain10=(uint32_t)(10.*vGainValue);
      return 1;
    }
    
    return 0;
 
}



// EOF
