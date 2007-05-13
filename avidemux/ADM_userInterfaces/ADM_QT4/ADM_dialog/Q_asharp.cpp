/***************************************************************************
                          DIA_crop.cpp  -  description
                             -------------------

			    GUI for cropping including autocrop
			    +Revisted the Gtk2 way
			     +Autocrop now in RGB space (more accurate)

    begin                : Fri May 3 2002
    copyright            : (C) 2002/2007 by mean
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

#include <config.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#define Ui_Dialog Ui_asharpDialog
#include "ui_asharp.h"
#undef Ui_Dialog

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_assert.h"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"
#include "ADM_video/ADM_vidASharp_param.h"
#include "DIA_flyAsharp.h"

//
//	Video is in YV12 Colorspace
//
//
class Ui_asharpWindow : public QDialog
 {
     Q_OBJECT
 protected : 
    int lock;
 public:
     flyASharp *myCrop;
     ADM_QCanvas *canvas;
     Ui_asharpWindow(ASHARP_PARAM *param,AVDMGenericVideoStream *in);
     ~Ui_asharpWindow();
     Ui_asharpDialog ui;
 public slots:
      void gather(ASHARP_PARAM *param);
      //void update(int i);
 private slots:
   void sliderUpdate(int foo);
   void valueChanged(int foo);

 private:
     
 };
  Ui_asharpWindow::Ui_asharpWindow(ASHARP_PARAM *param,AVDMGenericVideoStream *in)
  {
    uint32_t width,height;
        ui.setupUi(this);
        lock=0;
        // Allocate space for green-ised video
        width=in->getInfo()->width;
        height=in->getInfo()->height;
        ui.graphicsView->resize(width,height);
        canvas=new ADM_QCanvas(ui.graphicsView,width,height);
        
        myCrop=new flyASharp( width, height,in,canvas,ui.horizontalSlider);
        memcpy(&(myCrop->param),param,sizeof(ASHARP_PARAM));
        myCrop->_cookie=&ui;
        myCrop->upload();
        myCrop->sliderChanged();


        connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderUpdate(int)));
#define SPINNER(x) connect( ui.doubleSpinBox##x,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int))); 
          SPINNER(Treshold);
          SPINNER(Strength);
          SPINNER(Block);
          connect( ui.checkBox,SIGNAL(stateChanged(int)),this,SLOT(valueChanged(int))); 

  }
  void Ui_asharpWindow::sliderUpdate(int foo)
  {
    myCrop->sliderChanged();
  }
  void Ui_asharpWindow::gather(ASHARP_PARAM *param)
  {
    
        myCrop->download();
        memcpy(param,&(myCrop->param),sizeof(ASHARP_PARAM));
  }
Ui_asharpWindow::~Ui_asharpWindow()
{
  if(myCrop) delete myCrop;
  myCrop=NULL; 
  if(canvas) delete canvas;
  canvas=NULL;
}
void Ui_asharpWindow::valueChanged( int f )
{
  if(lock) return;
  lock++;
  myCrop->update();
  lock--;
}

#define MYSPIN(x) w->doubleSpinBox##x
//************************
uint8_t flyASharp::upload(void)
{
      Ui_asharpDialog *w=(Ui_asharpDialog *)_cookie;
        
        MYSPIN(Treshold)->setValue(param.t);
        MYSPIN(Strength)->setValue(param.d);
        MYSPIN(Block)->setValue(param.b);
        
        //w->bf->w->checkBox->isChecked();
        w->checkBox->setChecked(param.bf);
        return 1;
}
uint8_t flyASharp::download(void)
{
       Ui_asharpDialog *w=(Ui_asharpDialog *)_cookie;
       param.t= MYSPIN(Treshold)->value();
       param.d= MYSPIN(Strength)->value();
       param.b= MYSPIN(Block)->value();
       
       //w->spinBoxBottom->setValue(bottom);
       param.bf=w->checkBox->isChecked();
}

/**
      \fn     DIA_getCropParams
      \brief  Handle crop dialog
*/
uint8_t DIA_getASharp(ASHARP_PARAM *param, AVDMGenericVideoStream *in)
{
        uint8_t ret=0;
        
        Ui_asharpWindow dialog(param,in);        
        if(dialog.exec()==QDialog::Accepted)
        {
            dialog.gather(param); 
            ret=1;
        }
        return ret;
}
//____________________________________
// EOF


