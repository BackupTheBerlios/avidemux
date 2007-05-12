/***************************************************************************
    copyright            : (C) 2006 by mean
    email                : fixounet@free.fr
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************///

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

#include "default.h"

#include "ADM_colorspace/colorspace.h"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_commonUI//GUI_render.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "DIA_flyDialog.h"

#include "ADM_assert.h"

/**
    \fn ADM_flyDialog
*/
 ADM_flyDialog::ADM_flyDialog(uint32_t width,uint32_t height,AVDMGenericVideoStream *in,
                                void *canvas, void *slider,int yuv)
{
  _w=width;
  _h=height;
  _isYuvProcessing=yuv;
  _in=in;
  _rgb=new ColYuvRgb(_w,_h);
  _rgb->reset(_w,_h);
  ADM_assert(in);
  _yuvBuffer=new ADMImage(_w,_h);
  if(_isYuvProcessing)
  {
     _yuvBufferOut=new ADMImage(_w,_h);
     _rgbBuffer=NULL;
  }
  else
  {
    _rgbBuffer =new uint8_t [_w*_h*4];
    _yuvBufferOut=NULL;
  }
    _rgbBufferOut =new uint8_t [_w*_h*4];
  _slider=slider;
  _canvas=canvas;
  
  ADM_assert(_slider);
  ADM_assert(_canvas);
  
}
/**
    \fn cleanup
    \brief deallocate
*/
uint8_t ADM_flyDialog::cleanup(void)
{
#define DEL1(x)    if(x) {delete [] x;x=NULL;}
#define DEL2(x)    if(x) {delete  x;x=NULL;}
  
        DEL2(_yuvBufferOut);
        DEL2(_yuvBuffer);
        DEL1(_rgbBuffer);
        DEL1(_rgbBufferOut);
        DEL2(_rgb);
}
/**
    \fn ~ADM_flyDialog
    \brief destructor
*/
ADM_flyDialog::~ADM_flyDialog(void)
{
  cleanup(); 
}

/**
      \fn sliderChanged
      \brief callback to handle image changes
*/
uint8_t    ADM_flyDialog::sliderChanged(void)
{
  uint32_t fn= sliderGet(),nb;
  uint32_t len,flags;
  
    ADM_assert(_yuvBuffer);
    ADM_assert(_rgbBufferOut);
    ADM_assert(_in);
    
    nb=_in->getInfo()->nb_frames;
    fn=fn*nb;
    fn/=100;
    if(fn>=nb) fn=nb-1;
    
    if(!_in->getFrameNumberNoAlloc(fn,&len,_yuvBuffer,&flags))
    {
      printf("[FlyDialog] Cannot get frame %u\n",fn); 
      return 0;
    }
    /* Process...*/
    
    if(_isYuvProcessing)
    {
        process();
        _rgb->scale(_yuvBufferOut->data,_rgbBufferOut);
        
    } else // RGB Processing
      
    {
        ADM_assert(_rgbBuffer);
        _rgb->scale(_yuvBuffer->data,_rgbBuffer);
        process();
    }
    display();
}


//EOF

