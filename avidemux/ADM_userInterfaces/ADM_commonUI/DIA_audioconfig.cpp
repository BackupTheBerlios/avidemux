/***************************************************************************
                          DIA_audioConfig.cpp
  
  (C) Mean 2007 fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"
#include "ADM_default.h"

#include "DIA_factory.h"
#include "../../ADM_audiofilter/audioprocess.hxx"
#include "../../ADM_audiofilter/audioeng_process.h"
#include "ADM_coreAudio/include/audioencoder.h"

/**
      \fn DIA_defaultSettings
      \brief Dialog to set lame settings
      @return 1 on success, 0 on failure

*/
int DIA_defaultSettings(ADM_audioEncoderDescriptor *descriptor)
  {
    int ret=0;
    char string[400];
    uint32_t mmode,ppreset;
#define SZT(x) sizeof(x)/sizeof(diaMenuEntry )
#define BITRATE(x) {x,QT_TR_NOOP(#x)}
    diaMenuEntry bitrateM[]={
                              BITRATE(56),
                              BITRATE(64),
                              BITRATE(80),
                              BITRATE(96),
                              BITRATE(112),
                              BITRATE(128),
                              BITRATE(160),
                              BITRATE(192),
                              BITRATE(224),
                              BITRATE(384)
                          };
    diaElemMenu bitrate(&(descriptor->bitrate),   QT_TR_NOOP("_Bitrate:"), SZT(bitrateM),bitrateM);
  
      diaElem *elems[]={&bitrate};
    
  return ( diaFactoryRun(QT_TR_NOOP("Audio Configuration"),1,elems));

} 
//EOF 

