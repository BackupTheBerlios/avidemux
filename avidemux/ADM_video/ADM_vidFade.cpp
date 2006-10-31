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
#include <math.h>
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

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_osSupport/ADM_debug.h"
#include "ADM_filter/video_filters.h"
#include "ADM_vidFade_param.h"

class AVDM_Fade : public AVDMGenericVideoStream
{
  VideoCache      *vidCache;
  VIDFADE_PARAM   *_param;
  uint16_t         lookupLuma[256][256];
  uint16_t         lookupChroma[256][256];
  uint8_t         buildLut(void);
  public:
                                
                    AVDM_Fade(AVDMGenericVideoStream *in,CONFcouple *couples);    
                    ~AVDM_Fade(void);
    uint8_t         getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                          ADMImage *data,uint32_t *flags);
        
    char            *printConf( void );
    uint8_t         configure(AVDMGenericVideoStream *in);
    uint8_t         getCoupledConf( CONFcouple **couples);
};

static FILTER_PARAM fadeParam={4,{"startFade","endFade","inOut","toBlack"}};

BUILD_CREATE(fade_create,AVDM_Fade);
SCRIPT_CREATE(fade_script,AVDM_Fade,fadeParam);

/*************************************/
uint8_t DIA_fade(VIDFADE_PARAM *param);
uint8_t AVDM_Fade::configure(AVDMGenericVideoStream *in)
{
  _in=in;
  return DIA_fade(_param);
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
    GET(toBlack);
    
  }else
  {
    _param->startFade=0; 
    _param->endFade=_info.nb_frames-1;
    _param->inOut=0;
    _param->toBlack=0;
  }
  buildLut();
}
//________________________________________________________
uint8_t AVDM_Fade::getCoupledConf( CONFcouple **couples)
{
  *couples=new CONFcouple(4);
  CSET(startFade);
  CSET(endFade);
  CSET(inOut);
  CSET(toBlack);
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

  tgt=frame+_info.orgFrame;
  if(frame>=num_frames)
  {
    printf("[Fade] out of bound\n");
    return 0;
  }

  src=vidCache->getImage(frame);
  if(!src) return 0;
  if(tgt>_param->endFade || tgt <_param->startFade ||_param->endFade==_param->startFade )
  {
    //printf("Cur %u start %u end %u\n",tgt,_param->startFade,_param->endFade);
    data->duplicate(src);
    vidCache->unlockAll();
    return 1;
  }
  uint8_t *s,*d,*s2;
  uint16_t *index,*invertedIndex;
  uint32_t count=_info.width*_info.height,w;
  float num,den;
  
  den=_param->endFade-_param->startFade;
  
  num=tgt-_param->startFade;
  
  num=num/den;
  num*=255.;
  w=(uint32_t)floor(num+0.4);
  
//printf("w :%u\n",w);

  s=src->data;
  d=data->data;
  if(_param->toBlack)
  {
        index=lookupLuma[w];
        for(int i=0;i<count;i++)
        {
          *d++=(index[*s++]>>8);
        }
        // Now do chroma
        count>>=2;
        s=UPLANE(src);
        d=UPLANE(data);
        index=lookupChroma[w];
        for(int i=0;i<count;i++)
        {
          *d++=(index[*s++]>>8);
        }
        s=VPLANE(src);
        d=VPLANE(data);
        for(int i=0;i<count;i++)
        {
          *d++=(index[*s++]>>8);
        }
  }
  else
  {
        uint32_t x,alpha;
        ADMImage *final;

        final=vidCache->getImage(_param->endFade-_info.orgFrame);
        if(!final)
        {
              data->duplicate(src);
              vidCache->unlockAll();
              return 1;
        }

        s2=final->data;

        index=lookupLuma[w];
        
        invertedIndex=lookupLuma[255-w];
        for(int i=0;i<count;i++)
        {
          *d++=(index[*s++]+invertedIndex[*s2++])>>8;
        }
        // Now do chroma
        count>>=2;
        s=UPLANE(src);
        d=UPLANE(data);
        s2=UPLANE(final);
        index=lookupChroma[w];
        invertedIndex=lookupChroma[255-w];
        for(int i=0;i<count;i++)
        {
            *d++=(index[*s++]+invertedIndex[*s2++]-(128<<8))>>8;
        }
        s=VPLANE(src);
        d=VPLANE(data);
        s2=VPLANE(final);
        for(int i=0;i<count;i++)
        {
            *d++=(index[*s++]+invertedIndex[*s2++]-(128<<8))>>8;
            
        }
  }
  vidCache->unlockAll();
  return 1;
}

uint8_t AVDM_Fade::buildLut(void)
{
  float f,ration;
  for(int i=0;i<256;i++)
  {
    if(!_param->inOut) ration=255-i;
    else ration=i;
    for(int r=0;r<256;r++)
    {
      f=r;
      f=f*ration;
      lookupLuma[i][r]=(uint16_t)(f+0.4);

      f=r-128;
      f=f*ration;
      lookupChroma[i][r]=(128<<8)+(uint16_t)(f+0.4);

    }
    
  }
  return 1;
}
//EOF



