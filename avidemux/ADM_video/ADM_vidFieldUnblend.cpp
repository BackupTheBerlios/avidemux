/***************************************************************************
                          toolkit.hxx  -  description
                             -------------------
    begin                : Fri Dec 14 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "default.h"
#include "math.h"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_FILTER
#include "ADM_toolkit/ADM_debug.h"
#include "ADM_video/ADM_cache.h"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_enter.h"


typedef struct FIELDUNBLEND_PARAM
{
        uint32_t threshold;
        uint32_t show;
}FIELDUNBLEND_PARAM;

class vidFieldUnblend:public AVDMGenericVideoStream
{

protected:
  virtual char *printConf (void);
  VideoCache *vidCache;
  FIELDUNBLEND_PARAM *_param;
public:

                        vidFieldUnblend (AVDMGenericVideoStream * in, CONFcouple * setup);
        virtual         ~vidFieldUnblend ();
  virtual uint8_t getFrameNumberNoAlloc (uint32_t frame, uint32_t * len,
                                         ADMImage * data, uint32_t * flags);
  uint8_t configure (AVDMGenericVideoStream * instream);
  virtual uint8_t getCoupledConf (CONFcouple ** couples);

};

static FILTER_PARAM field_unblend_template =
  { 2,"threshold","show"};

BUILD_CREATE (fieldunblend_create, vidFieldUnblend);
SCRIPT_CREATE (fieldunblend_script, vidFieldUnblend, field_unblend_template);
//*************************************
uint8_t vidFieldUnblend::configure (AVDMGenericVideoStream * in)
{
int v;
        _param->show=GUI_YesNo("Metrics","Do you want to print metrics on screen ?" );
        v=_param->threshold;
        if(DIA_GetIntegerValue(&v, 10, 99,"Treshold","Treshold value"))
                _param->threshold=v;
        return 1;
}
/*************************************/
char *vidFieldUnblend::printConf (void)
{
  static char buf[50];
  sprintf ((char *) buf, " Field Unblend Thresh:%d",_param->threshold);
  return buf;
}

#define MAX_BLOCKS 50
/*************************************/
vidFieldUnblend::vidFieldUnblend (AVDMGenericVideoStream * in, CONFcouple * couples)
{

  _in = in;
  memcpy (&_info, _in->getInfo (), sizeof (_info));
  _info.encoding = 1;
  vidCache = new VideoCache (10, in);
  _uncompressed=new ADMImage(_info.width,_info.height);
 _param=new FIELDUNBLEND_PARAM;
 if(couples)
 {
#undef GET
#define GET(x) couples->getCouple(#x,&(_param->x))
      GET (threshold);
      GET (show);
  }
  else
  {
        _param->threshold=71;
        _param->show=0;
  }
}
//____________________________________________________________________
vidFieldUnblend::~vidFieldUnblend ()
{

  delete vidCache;
  vidCache = NULL;
  delete _uncompressed;
  _uncompressed=NULL;
  delete _param;
  _param=NULL;
}
static void merge(ADMImage *src1,ADMImage *src2,ADMImage *tgt)
{
uint32_t ww,hh;
uint8_t *s1,*s2,*out;
int o;

        s1=YPLANE(src1);
        s2=YPLANE(src2);
        out=YPLANE(tgt);
        ww=src1->_width;
        hh=src1->_height;
        for(int y=0;y<hh;y++)
                for(int x=0;x<ww;x++)
                {
                        o=*s1+*s2;
                        o>>=1;
                        *out=o;
                        s1++;
                        s2++;
                        out++;

                }




}
static float computeDiff(ADMImage *src1,ADMImage *src2)
{
float df=0;
int delta;
uint32_t ww,hh;
uint8_t *s1,*s2;

        s1=YPLANE(src1);
        s2=YPLANE(src2);
        ww=src1->_width;
        hh=src1->_height;

          for(int y=0;y<hh;y++)
                for(int x=0;x<ww;x++)
                {
                        delta=abs(*s1-*s2);
                        df+=delta;
                        s1++;
                        s2++;

                }
        return df;
}
static float computeDiff2(ADMImage *src1,ADMImage *src2,ADMImage *cand)
{
float df=0;
int delta;
uint32_t ww,hh;
uint8_t *s1,*s2,*d1;
int a1,a2,t1;
        s1=YPLANE(src1);
        s2=YPLANE(src2);
        
        d1=YPLANE(cand);
        ww=src1->_width;
        hh=src1->_height;

          for(int y=0;y<hh;y++)
                for(int x=0;x<ww;x++)
                {
                        a1=*s1;
                        a2=*s2;
                        t1=*d1;
                        if(a1==a2) ;
                        else
                        if(a1>a2)
                        {
                                if(t1 <=a1 && t1>=a2) df+=1;
                                        else df-=1;
                        }else
                                if(t1 <=a2 && t1>=a1) df+=1;
                                        else df-=1;
                        s1++;
                        s2++;
                        d1++;
                }
        return df;
}
uint8_t vidFieldUnblend::getFrameNumberNoAlloc (uint32_t inframe,
                                uint32_t * len,
                                ADMImage * data, uint32_t * flags)
{

	
	ADMImage *srcP,*srcN,*src,*final;
        float distMerged, distN,distP,distM;
        char txt[255];

        if(inframe<2 || inframe>inframe>_info.nb_frames-3)
        {
                data->duplicate(vidCache->getImage(inframe));
                vidCache->unlockAll();
                return 1;
        }
        srcP=vidCache->getImage(inframe-2);
        srcN=vidCache->getImage(inframe+2);
        src=vidCache->getImage(inframe);

//        merge(srcP,srcN,_uncompressed);
        distMerged=computeDiff2(srcP,srcN,src);
        final=src;
        
        distMerged/=_info.width*_info.height;
        distMerged*=100;

        if(distMerged>_param->threshold)
        {
                // which is closer N or P ?
                if(computeDiff(srcN,src)>computeDiff(srcP,src))
                        final=srcP;     
                else
                        final=srcN;
        }
        if(_param->show)
        {
                sprintf(txt," M %02.1f",distMerged);
                drawString(final,2,2,txt);
                if(final!=src)
                {
                sprintf(txt," Replaced");
                drawString(final,2,6,txt);
                }
        }
         

        data->duplicate(final);
        vidCache->unlockAll();
	return 1;
}
uint8_t vidFieldUnblend::getCoupledConf (CONFcouple ** couples)
{

  ADM_assert (_param);
  *couples = new CONFcouple (2);
#undef CSET
#define CSET(x)  (*couples)->setCouple(#x,(_param->x))
  CSET (threshold);
  CSET (show);
  
  return 1;
}


//EOF
