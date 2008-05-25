/***************************************************************************
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

#include <math.h>

#include "resizing.h"

static double aspectRatio[2][3]={
                              {1.,0.888888,1.19}, // NTSC 1:1 4:3 16:9
                              {1.,1.066667,1.43} // PAL  1:1 4:3 16:9
                            };
#define aprintf printf
resizeWindow::~resizeWindow()
  {
    
  }
resizeWindow::resizeWindow(resParam *param)     : QDialog()
 {
     ui.setupUi(this);
     _param=param;
     ui.spinBoxWidth->setValue(_param->width);
     ui.spinBoxHeight->setValue(_param->height);
     ui.horizontalSlider->setValue(100);
     update(0);
     connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(update(int)));
 
 }
 void resizeWindow::gather(void)
 {
    _param->width=ui.spinBoxWidth->value();
    _param->height=ui.spinBoxHeight->value();
    _param->algo=ui.comboBoxAlgo->currentIndex();
 }
 
 void resizeWindow::update(int foo)
 {

  double percent;
  float x,y;
  float sr_mul,dst_mul;

 int32_t xx,yy;
 float erx,ery;
  int sar,dar;

  percent = ui.horizontalSlider->value();;
  sar=ui.comboBoxSource->currentIndex();
  dar=ui.comboBoxDestination->currentIndex();
  if(percent<10.0) percent=10.;

  aprintf("drag called : %f \n",percent);
  x=_param->originalWidth;
  y=_param->originalHeight;
  erx=0;
  ery=0;
  sr_mul=1.;
  if(sar)
  	{  // source is 4/3 or 16/9
			sr_mul=aspectRatio[_param->pal][sar];

	}

  dst_mul=1.;
  if(dar)
        {  // dst is 4/3 or 16/9
  
                        dst_mul=1/aspectRatio[_param->pal][dar];
        }
        aprintf("source mul %02.2f , dst mul : %02.2f\n",sr_mul,dst_mul);
        x=x*sr_mul*dst_mul;
        y=y;
  
        // normalize it to recover 100% width
        y=y/(x/_param->originalWidth);
        x=_param->originalWidth;
  
        aprintf("AR:x,y  : %03f %03f \n",x,y);
  
        percent/=100.;
        x=x*percent;
        y=y*percent;
  
  
        aprintf("AR x,y  : %03f %03f \n",x,y);
        xx=(uint32_t)floor(x+0.5);
        yy=(uint32_t)floor(y+0.5);
  
        if(xx&1) xx--;
        if(yy&1) yy--;
  
  
        if(ui.checkBoxRoundup->checkState())
        {
                int32_t ox=xx,oy=yy;
                xx=(xx +7) & 0xfffff0;
                yy=(yy +7) & 0xfffff0;
  
                erx=(xx-ox);
                erx=erx/xx;
                ery=(yy-oy);
                ery=ery/yy;
  
                aprintf("x: %d -> %d : err %f\n",ox,xx,erx);
                aprintf("y: %d -> %d : err %f\n",oy,yy,ery);
        }
  
        //
        ui.spinBoxWidth->setValue(xx);
        ui.spinBoxHeight->setValue(yy);

   
}
/**
    \fn DIA_resize
    \brief Handle resize dialo
*/
uint8_t DIA_resize(uint32_t *width,uint32_t *height,uint32_t *alg,uint32_t originalw, uint32_t originalh,uint32_t fps1000)
{
uint8_t r=0;
      resParam param={*width,*height,originalw,originalh,fps1000,*alg,0};
      //
      if(fps1000>24600 && fps1000<25400)
        {
                param.pal=1;
        }
       

     // Fetch info
     resizeWindow resizewindow(&param) ;
     ;
     if(resizewindow.exec()==QDialog::Accepted)
     {
       resizewindow.gather();
       *width=param.width;
       *height=param.height;
       *alg=param.algo;
       r=1;
     }
     return r;
}  
//********************************************
//EOF
