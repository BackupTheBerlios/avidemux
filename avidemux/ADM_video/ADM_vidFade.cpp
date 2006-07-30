/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "avi_vars.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include"ADM_video/ADM_vidField.h"
#include"ADM_video/ADM_cache.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"
#include "ADM_filter/video_filters.h"
#include "ADM_vidFade_param.h"

class AVDM_Fade : public AVDMGenericVideoStream
{
  VideoCache      *vidCache;
  VIDFADE_PARAM   *_param;
  public:
                                
                    AVDM_Fade(AVDMGenericVideoStream *in,CONFcouple *couples);    
                    ~AVDM_Fade(void);
    uint8_t         getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                          ADMImage *data,uint32_t *flags);
        
    char            *printConf( void );
    uint8_t         configure(AVDMGenericVideoStream *in);
    uint8_t         getCoupledConf( CONFcouple **couples);
};

static FILTER_PARAM fadeParam={3,{"startFade","endFade","inOut"}};

BUILD_CREATE(fade_create,AVDM_Fade);
SCRIPT_CREATE(fade_script,AVDM_Fade,fadeParam);

/*************************************/

uint8_t AVDM_Fade::configure(AVDMGenericVideoStream *in)
{
  _in=in;
  return 1;
        
}

char *AVDM_Fade::printConf( void )
{
  static char buf[50];

        //ADM_assert(_param);
  sprintf((char *)buf," Fade : Start %u End %u",_param->startFade,_param->endFade);
  if(_param->inOut) strcat(buf," In"); else strcat(buf," Out");
  return buf;
}


AVDM_Fade::AVDM_Fade(AVDMGenericVideoStream *in,CONFcouple *couples)

{
                
  int count = 0;
  char buf[80];
  unsigned int *p;

  _in=in;         
  memcpy(&_info,_in->getInfo(),sizeof(_info));    
  _info.encoding=1;
  vidCache=new VideoCache(3,in);
  
  _param=new VIDFADE_PARAM;
  if(couples)
  {
    GET(startFade);
    GET(endFade);
    GET(inOut);
    
  }else
  {
    _param->startFade=0; 
    _param->endFade=_info.nb_frames;
    _param->inOut=0;
  }
  
}
//________________________________________________________
uint8_t AVDM_Fade::getCoupledConf( CONFcouple **couples)
{
  *couples=new CONFcouple(3);
  CSET(startFade);
  CSET(endFade);
  CSET(inOut);
  return 1;
}
//________________________________________________________
AVDM_Fade::~AVDM_Fade(void)
{
                
  if(vidCache) delete vidCache;                
  vidCache=NULL;   
  if(_param) delete _param;
  _param=NULL;
}
uint8_t AVDM_Fade::getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                            ADMImage *data,uint32_t *flags)
{

  uint32_t num_frames,tgt;
  ADMImage *src;

  num_frames=_info.nb_frames;   // ??
  if(frame>=num_frames) return 0;
  
  tgt=num_frames;

  src=vidCache->getImage(tgt);
  if(!src) return 0;
  data->duplicate(src);

  vidCache->unlockAll();
  return 1;
}
//EOF



