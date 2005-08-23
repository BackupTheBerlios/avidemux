/***************************************************************************
                         
        Blending remover

        A B C D E -> A AB B 

    copyright            : (C) 2005 by mean
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
#include "ADM_toolkit/ADM_cpuCap.h"

#include "ADM_vidBlendRemoval_param.h"
#define MUL 1
// Set it to 2 for post separate field



class vidBlendRemoval:public AVDMGenericVideoStream
{

protected:
  virtual char *printConf (void);
  VideoCache *vidCache;
  BLEND_REMOVER_PARAM *_param;
  uint32_t              _lastRemoved;
  ADMImage              *rebuild;
public:

                        vidBlendRemoval (AVDMGenericVideoStream * in, CONFcouple * setup);
        virtual         ~vidBlendRemoval ();
  virtual uint8_t getFrameNumberNoAlloc (uint32_t frame, uint32_t * len,
                                         ADMImage * data, uint32_t * flags);
  uint8_t configure (AVDMGenericVideoStream * instream);
  virtual uint8_t getCoupledConf (CONFcouple ** couples);

};

static FILTER_PARAM field_unblend_template =
  { 4,"threshold","show","noise","identical"};

BUILD_CREATE (blendremove_create, vidBlendRemoval);
SCRIPT_CREATE (blendremove_script, vidBlendRemoval, field_unblend_template);
//*************************************
uint8_t vidBlendRemoval::configure (AVDMGenericVideoStream * in)
{
        if(DIA_blendRemoval(_param))
        {
                _lastRemoved=0xFFFFFFF;
                return 1;
        }
        return 0;
}
/*************************************/
char *vidBlendRemoval::printConf (void)
{
  static char buf[50];
  sprintf ((char *) buf, " Blend Removal Thresh:%d Noise:%d Ident:%d",_param->threshold,_param->noise,_param->identical);
  return buf;
}

#define MAX_BLOCKS 50
/*************************************/
vidBlendRemoval::vidBlendRemoval (AVDMGenericVideoStream * in, CONFcouple * couples)
{

  _in = in;
  memcpy (&_info, _in->getInfo (), sizeof (_info));
  _info.encoding = 1;
  vidCache = new VideoCache (10, in);
  _uncompressed=new ADMImage(_info.width,_info.height);
  rebuild=new ADMImage(_info.width,_info.height);

 _param=new BLEND_REMOVER_PARAM;
 _lastRemoved=0xFFFF;
 if(couples)
 {
#undef GET
#define GET(x) couples->getCouple(#x,&(_param->x))
      GET (threshold);
      GET (show);
      GET (noise);
      GET (identical);
  }
  else
  {
        _param->threshold=10;
        _param->show=0;
        _param->noise=5;
        _param->identical=2;
  }
}
//____________________________________________________________________
vidBlendRemoval::~vidBlendRemoval ()
{

  delete vidCache;
  vidCache = NULL;
  delete _uncompressed;
  _uncompressed=NULL;
  delete _param;
  _param=NULL;
  delete rebuild;
  rebuild=NULL;
}





uint8_t vidBlendRemoval::getFrameNumberNoAlloc (uint32_t inframe,
                                uint32_t * len,
                                ADMImage * data, uint32_t * flags)
{

	
	ADMImage *srcP,*srcN,*src,*final,*display;
        float distMerged, distN,distP,distM,distR,skip=0;
        char txt[255];

        
        if(inframe<1 || inframe>inframe>_info.nb_frames-2 )
        {
                skip=1;
        }
        if(inframe && _lastRemoved==inframe-1)
        {
                skip=1;
        }
        
        if(skip)
        {
                data->duplicate(vidCache->getImage(inframe));
                vidCache->unlockAll();
                return 1;
        }
        
        

        srcP=vidCache->getImage(inframe-1);
        src=vidCache->getImage(inframe);
        srcN=vidCache->getImage(inframe+1);
        if(!srcP || !src || !srcN)
        {
                data->duplicate(vidCache->getImage(inframe));
                vidCache->unlockAll();
                return 1;
        }
       
        rebuild->merge(srcP,srcN);
        
        distP=ADMImage::lumaDiff(src,srcN,_param->noise);
        distN=ADMImage::lumaDiff(src,srcP,_param->noise);
        distM=ADMImage::lumaDiff(src,rebuild,_param->noise);
        
        
        
        
        double medium;

         if(distN>1&&distP>1)
        {
                if(distP>distN) medium=distN;
                          else  medium=distP;
                  //medium=min(distM,distR);
                 
                 medium/=100;
                 distN/=medium;
                 distP/=medium;
                 distM/=medium;
         
         }
        
        medium=medium/(_info.width*_info.height);
        medium*=1000;
        if(distM<_param->threshold && medium>_param->identical) // medium is how close pictures are, too close, we discard
        {
                data->duplicate(srcP);
                _lastRemoved=inframe;
                if(_param->show )
                {
                        sprintf(txt,"Dupe");
                        drawString(data,2,5,txt);      
                }
        }
        else
                data->duplicate(src);
        if(_param->show)
        {
                display=data;

                sprintf(txt," N %02.1f",distN);
                drawString(display,2,0,txt);

                sprintf(txt," P %02.1f",distP);
                drawString(display,2,1,txt);

                sprintf(txt," M %02.1f",distM);
                drawString(display,2,2,txt);

                sprintf(txt," %% %02.1f",medium);
                drawString(display,2,3,txt);
        }


          
        vidCache->unlockAll();
	return 1;
}
uint8_t vidBlendRemoval::getCoupledConf (CONFcouple ** couples)
{

  ADM_assert (_param);
  *couples = new CONFcouple (4);
#undef CSET
#define CSET(x)  (*couples)->setCouple(#x,(_param->x))
  CSET (threshold);
  CSET (show);
  CSET (noise);
  CSET (identical);
  
  return 1;
}


//EOF
