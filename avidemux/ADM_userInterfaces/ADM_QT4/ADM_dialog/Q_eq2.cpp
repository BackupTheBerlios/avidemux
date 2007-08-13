/***************************************************************************
                          Q_eq2.cpp  -  description

                flyDialog for MPlayer EQ2 filter
    copyright            : (C) 2002/2007 by mean Fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ui_eq2.h"

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_assert.h"
#include "ADM_video/ADM_vidEq2.h"

#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"
#include "ADM_video/ADM_vidEq2.h"
#include "DIA_flyEq2.h"

//
//	Video is in YV12 Colorspace
//
//
class Ui_eq2Window : public QDialog
 {
     Q_OBJECT
 protected : 
    int lock;
 public:
     flyEq2 *myCrop;
     ADM_QCanvas *canvas;
     Ui_eq2Window(Eq2_Param *param,AVDMGenericVideoStream *in);
     ~Ui_eq2Window();
     Ui_DialogEq2 ui;
 public slots:
      void gather(Eq2_Param *param);
      //void update(int i);
 private slots:
   void sliderUpdate(int foo);
   void valueChanged(int foo);

 private:
     
 };
  Ui_eq2Window::Ui_eq2Window(Eq2_Param *param,AVDMGenericVideoStream *in)
  {
    uint32_t width,height;
        ui.setupUi(this);
        lock=0;
        // Allocate space for green-ised video
        width=in->getInfo()->width;
        height=in->getInfo()->height;

        canvas=new ADM_QCanvas(ui.graphicsView,width,height);
        
        myCrop=new flyEq2( width, height,in,canvas,ui.horizontalSlider);
        memcpy(&(myCrop->param),param,sizeof(Eq2_Param));
        myCrop->_cookie=&ui;
        myCrop->upload();
        myCrop->sliderChanged();


        connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderUpdate(int)));
#define SPINNER(x) connect( ui.horizontalSlider##x,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int))); 
        SPINNER(Red);
        SPINNER(Blue);
        SPINNER(Green);
         
        SPINNER(Contrast);
        SPINNER(Brightness);
        SPINNER(Saturation);

        SPINNER(Initial);
        SPINNER(Weight);
  }
  void Ui_eq2Window::sliderUpdate(int foo)
  {
    myCrop->sliderChanged();
  }
  void Ui_eq2Window::gather(Eq2_Param *param)
  {
    
        myCrop->download();
        memcpy(param,&(myCrop->param),sizeof(Eq2_Param));
  }
Ui_eq2Window::~Ui_eq2Window()
{
  if(myCrop) delete myCrop;
  myCrop=NULL; 
  if(canvas) delete canvas;
  canvas=NULL;
}
void Ui_eq2Window::valueChanged( int f )
{
  if(lock) return;
  lock++;
  myCrop->update();
  lock--;
}

#define sliderSet(x,y) w->horizontalSlider##x->setValue((int)(param.y*10));
#define sliderGet(x,y) param.y=w->horizontalSlider##x->value()/10.;
//************************
uint8_t flyEq2::upload(void)
{
Ui_DialogEq2 *w=(Ui_DialogEq2 *)_cookie;

        sliderSet(Contrast,contrast);
        sliderSet(Brightness,brightness);
        sliderSet(Saturation,saturation);

        sliderSet(Red,rgamma);
        sliderSet(Green,ggamma);
        sliderSet(Blue,bgamma);

        sliderSet(Initial,gamma);
        sliderSet(Weight,gamma);
       return 1;
}
uint8_t flyEq2::download(void)
{
Ui_DialogEq2 *w=(Ui_DialogEq2 *)_cookie;

        sliderGet(Contrast,contrast);
        sliderGet(Brightness,brightness);
        sliderGet(Saturation,saturation);

        sliderGet(Red,rgamma);
        sliderGet(Green,ggamma);
        sliderGet(Blue,bgamma);

        sliderGet(Initial,gamma);
        sliderGet(Weight,gamma);

return 1;
}
/**
      \fn     DIA_getEQ2Param
      \brief  Handle MPlayer EQ2 flyDialog
*/
uint8_t DIA_getEQ2Param(Eq2_Param *param, AVDMGenericVideoStream *in)
{
        uint8_t ret=0;
        
        Ui_eq2Window dialog(param,in);        
        if(dialog.exec()==QDialog::Accepted)
        {
            dialog.gather(param); 
            ret=1;
        }
        return ret;
}
//____________________________________
// EOF


