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

#define Ui_Dialog Ui_cropDialog
#include "ui_crop.h"
#undef Ui_Dialog

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_assert.h"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"
#include "DIA_flyCrop.h"

//
//	Video is in YV12 Colorspace
//
//
class Ui_cropWindow : public QDialog
 {
     Q_OBJECT
 protected : 
    int lock;
 public:
     flyCrop *myCrop;
     ADM_QCanvas *canvas;
     Ui_cropWindow(CROP_PARAMS *param,AVDMGenericVideoStream *in);
     ~Ui_cropWindow();
     Ui_cropDialog ui;
 public slots:
      void gather(CROP_PARAMS *param);
      //void update(int i);
 private slots:
   void sliderUpdate(int foo);
   void valueChanged(int foo);
   void autoCrop( bool f );
   void reset( bool f );

 private:
     
 };
  Ui_cropWindow::Ui_cropWindow(CROP_PARAMS *param,AVDMGenericVideoStream *in)
  {
    uint32_t width,height;
        ui.setupUi(this);
        lock=0;
        // Allocate space for green-ised video
        width=in->getInfo()->width;
        height=in->getInfo()->height;
        ui.graphicsView->resize(width,height);
        canvas=new ADM_QCanvas(ui.graphicsView,width,height);
        
        myCrop=new flyCrop( width, height,in,canvas,ui.horizontalSlider);
        myCrop->left=param->left;
        myCrop->right=param->right;
        myCrop->top=param->top;
        myCrop->bottom=param->bottom;
        myCrop->_cookie=&ui;
        myCrop->upload();
        myCrop->sliderChanged();


        connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderUpdate(int)));
        connect( ui.pushButtonAutoCrop,SIGNAL(clicked(bool)),this,SLOT(autoCrop(bool)));
        connect( ui.pushButtonReset,SIGNAL(clicked(bool)),this,SLOT(reset(bool)));
#define SPINNER(x) connect( ui.spinBox##x,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int))); 
          SPINNER(Left);
          SPINNER(Right);
          SPINNER(Top);
          SPINNER(Bottom);

  }
  void Ui_cropWindow::sliderUpdate(int foo)
  {
    myCrop->sliderChanged();
  }
  void Ui_cropWindow::gather(CROP_PARAMS *param)
  {
    
        myCrop->download();
        param->left=myCrop->left;
        param->right=myCrop->right;
        param->top=myCrop->top;
        param->bottom=myCrop->bottom;
  }
Ui_cropWindow::~Ui_cropWindow()
{
  if(myCrop) delete myCrop;
  myCrop=NULL; 
  if(canvas) delete canvas;
  canvas=NULL;
}
void Ui_cropWindow::valueChanged( int f )
{
  if(lock) return;
  lock++;
  myCrop->download();
  myCrop->process();
  myCrop->display();
  lock--;
}

void Ui_cropWindow::autoCrop( bool f )
{
  lock++;
  myCrop->autocrop();
  lock--;
}
void Ui_cropWindow::reset( bool f )
{
         myCrop->left=0;
         myCrop->right=0;
         myCrop->bottom=0;
         myCrop->top=0;
         lock++;
         myCrop->upload();
         myCrop->process();
         myCrop->display();
         lock--;
}

//************************
uint8_t flyCrop::upload(void)
{
      Ui_cropDialog *w=(Ui_cropDialog *)_cookie;
        
        w->spinBoxLeft->setValue(left);
        w->spinBoxRight->setValue(right);
        w->spinBoxTop->setValue(top);
        w->spinBoxBottom->setValue(bottom);
        
        return 1;
}
uint8_t flyCrop::download(void)
{
        int reject=0;
Ui_cropDialog *w=(Ui_cropDialog *)_cookie;
#define SPIN_GET(x,y) x=w->spinBox##y->value();
                        SPIN_GET(left,Left);
                        SPIN_GET(right,Right);
                        SPIN_GET(top,Top);
                        SPIN_GET(bottom,Bottom);
                        
                        printf("%d %d %d %d\n",left,right,top,bottom);
                        
                        left&=0xffffe;
                        right&=0xffffe;
                        top&=0xffffe;
                        bottom&=0xffffe;
                        
                        if((top+bottom)>_h)
                                {
                                        top=bottom=0;
                                        reject=1;
                                }
                        if((left+right)>_w)
                                {
                                        left=right=0;
                                        reject=1;
                                }
                        if(reject)
                                upload();
}

/**
      \fn     DIA_getCropParams
      \brief  Handle crop dialog
*/
int DIA_getCropParams(	char *name,CROP_PARAMS *param,AVDMGenericVideoStream *in)
{
        uint8_t ret=0;
        
        Ui_cropWindow dialog(param,in);        
        if(dialog.exec()==QDialog::Accepted)
        {
            dialog.gather(param); 
            ret=1;
        }
        return ret;
}
//____________________________________
// EOF

