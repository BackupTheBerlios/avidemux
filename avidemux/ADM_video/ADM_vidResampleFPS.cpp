/***************************************************************************
                          Resample fps
                             -------------------
    begin                : Wed Nov 6 2002
    copyright            : (C) 2002 by mean
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

#include <stdio.h>
#include <math.h>

#include <stdlib.h>
#include <string.h>
#include <ADM_assert.h>

#include "config.h"
#include "fourcc.h"
#include "avio.hxx"
#include "config.h"
#include "avi_vars.h"
#ifdef HAVE_ENCODER


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_video/ADM_vidFlipV.h"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_enter.h"

static FILTER_PARAM ResampParam={1,{"newfps"}};
typedef struct FPS_Param
{
  uint32_t  newfps; 
}FPS_Param;
class  ADMVideoResampleFPS:public AVDMGenericVideoStream
{

  protected:
    AVDMGenericVideoStream  *_in;           
    virtual char            *printConf(void);
                            FPS_Param   *_param;     
  public:
                
                        ADMVideoResampleFPS(  AVDMGenericVideoStream *in,CONFcouple *setup);
    virtual             ~ADMVideoResampleFPS();
    virtual uint8_t     configure(AVDMGenericVideoStream *in);
    virtual uint8_t     getFrameNumberNoAlloc(uint32_t frame, uint32_t *len,
                                          ADMImage *data,uint32_t *flags);


}     ;

SCRIPT_CREATE(resamplefps_script,ADMVideoResampleFPS,ResampParam);
BUILD_CREATE(resamplefps_create,ADMVideoResampleFPS);
 
uint8_t ADMVideoResampleFPS::configure(AVDMGenericVideoStream *in)
{
  
  float f=_param->newfps;
  
  f/=1000;
  
  _in=in;     
  if(DIA_GetFloatValue(&f, 1.0, 200., "Enter new fps","New fps:"))
  {
    f*=1000;
    _param->newfps=(uint32_t)floor(f+0.4); 
    return 1;
  }      
  return 0;        
}
char *ADMVideoResampleFPS::printConf( void )
{
  static char buf[50];
        
  sprintf((char *)buf," Resample to %2.2f fps",(double)_param->newfps/1000.);
  return buf;
}

ADMVideoResampleFPS::ADMVideoResampleFPS(  AVDMGenericVideoStream *in,CONFcouple *couples)
{
  
  _in=in;         
  memcpy(&_info,_in->getInfo(),sizeof(_info));    
  _info.encoding=1; 
  _param=new  FPS_Param;
   
  if(couples)
  {                 
    GET(newfps);                            
  }
  else
  {
    _param->newfps =_info.fps1000;                
  }      
 
  double newlength;
  
  newlength=_info.nb_frames;
  newlength/=_info.fps1000;
  newlength*=_param->newfps;
  _info.nb_frames=(uint32_t)floor(newlength);
  
}
ADMVideoResampleFPS::~ADMVideoResampleFPS()
{
  delete _param;
  
}
uint8_t ADMVideoResampleFPS::getFrameNumberNoAlloc(uint32_t frame,
                                             uint32_t *len,
                                             ADMImage *data,
                                             uint32_t *flags)
{

  ADM_assert(frame<_info.nb_frames);
  // read uncompressed frame
  
  // What frame are we seeking ?
  double f;
  
  f=frame;
  f/=_in->getInfo()->fps1000;
  f*=_param->newfps;
  
  uint32_t nw;
  
  nw=(uint32_t)floor(f+0.4);
  if(nw>_in->getInfo()->nb_frames-1)
    nw=_in->getInfo()->nb_frames-1;
  if(!_in->getFrameNumberNoAlloc(nw, len,data,flags)) return 0;
  return 1;
 
}



#endif
