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
       
   diaElemToggle    eDRC(&vDRC,_("_Dynamic range compression"));
   
   diaElemToggleInt eTimeShift(&vTshift,_("_Time shift (ms):"),&vTshiftValue,_("Time shift value (ms)"),-1000*100,1000*100);
   
//    diaElemToggle    eTimeShift(&vTshift,_("Enable _time shift"));
//    diaElemInteger  eShift(&vTshiftValue,_("Time shift _value (ms):"),-10000,10000);
//    
//    eTimeShift.link(1,&eShift);
//   
    //**********************************
   diaElemToggleUint eResample(&vDownsample,_("R_esampling (Hz):"),&vFreq,_("Resampling frequency (Hz)"),6000,64000);
    //diaElemToggle      eResample(&vDownsample,_("R_esampling (Hz)"));
    //diaElemUInteger  eResampleValue(&vFreq,_("_Resampling frequency (Hz):"),6000,64000);
  
//    eResample.link(1,&eResampleValue);
    
    //**********************************
    diaMenuEntry menuFPS[]={
  {FILMCONV_NONE,     _("None")},
  {FILMCONV_FILM2PAL, _("Film to PAL")},
  {FILMCONV_PAL2FILM, _("PAL to Film")}
    };
  
   diaElemMenu      eFPS(&vFilm,_("_Frame rate change:"),3,menuFPS);

   //**********************************
    diaMenuEntry menuGain[]={
  {ADM_NO_GAIN,       _("None")},
  {ADM_GAIN_AUTOMATIC,_("Automatic (max -3 dB)")},
  {ADM_GAIN_MANUAL,   _("Manual")}};
  
   diaElemMenu      eGain(&vGainMode,_("_Gain mode:"),3,menuGain);
   
    diaElemFloat  eGainValue(&vGainValue,_("G_ain value:"),-10,10);
     eGain.link(&(menuGain[2]),1,&eGainValue);
   diaElemFrame frameGain(_("Gain"));   
    frameGain.swallow(&eGain);
    frameGain.swallow(&eGainValue);
  //********************************
    diaMenuEntry menuMixer[]={
  {CHANNEL_INVALID,     _("No change")},
  {CHANNEL_MONO,        _("Mono")},
  {CHANNEL_STEREO,      _("Stereo")},
  {CHANNEL_2F_1R,       _("Stereo+surround")},
  {CHANNEL_3F,          _("Stereo+center")},
  {CHANNEL_3F_1R,           _("Stereo+center+surround")},
  {CHANNEL_2F_2R,           _("Stereo front+stereo rear")},
  {CHANNEL_3F_2R,           _("5 channels")},
  {CHANNEL_3F_2R_LFE,       _("5.1")},
  {CHANNEL_DOLBY_PROLOGIC,  _("Dolby Pro Logic")},
  {CHANNEL_DOLBY_PROLOGIC2, _("Dolby Pro Logic II")}
    };

 diaElemMenu      eMixer(&vChan,_("_Mixer:"),11,menuMixer);
 
 /************************************/
 diaElem *elems[]={&eFPS, &eMixer, &eTimeShift,  &eResample, &eDRC, &frameGain};
  if( diaFactoryRun(_("Audio Filters"),6,elems))
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
