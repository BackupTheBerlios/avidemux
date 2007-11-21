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

#include "config.h"
#include <string.h>
#include <stdio.h>
#include <math.h>

#define Ui_Dialog Ui_hueDialog
#include "ui_hue.h"
#undef Ui_Dialog

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/ADM_rgb.h"
#include "ADM_assert.h"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"
#include "ADM_videoFilter/ADM_vidHue.h"
#include "DIA_flyHue.h"

//
//	Video is in YV12 Colorspace
//
//
class Ui_hueWindow : public QDialog
 {
     Q_OBJECT
 protected : 
    int lock;
 public:
     flyHue *myCrop;
     ADM_QCanvas *canvas;
     Ui_hueWindow(Hue_Param *param,AVDMGenericVideoStream *in);
     ~Ui_hueWindow();
     Ui_hueDialog ui;
 public slots:
      void gather(Hue_Param *param);
      //void update(int i);
 private slots:
   void sliderUpdate(int foo);
   void valueChanged(int foo);

 private:
     
 };
  Ui_hueWindow::Ui_hueWindow(Hue_Param *param,AVDMGenericVideoStream *in)
  {
    uint32_t width,height;
        ui.setupUi(this);
        lock=0;
        // Allocate space for green-ised video
        width=in->getInfo()->width;
        height=in->getInfo()->height;

        canvas=new ADM_QCanvas(ui.graphicsView,width,height);
        
        myCrop=new flyHue( width, height,in,canvas,ui.horizontalSlider);
        memcpy(&(myCrop->param),param,sizeof(Hue_Param));
        myCrop->_cookie=&ui;
        myCrop->upload();
        myCrop->sliderChanged();


        connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderUpdate(int)));
#define SPINNER(x) connect( ui.horizontalSlider##x,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int))); 
          SPINNER(Hue);
          SPINNER(Saturation);

  }
  void Ui_hueWindow::sliderUpdate(int foo)
  {
    myCrop->sliderChanged();
  }
  void Ui_hueWindow::gather(Hue_Param *param)
  {
    
        myCrop->download();
        memcpy(param,&(myCrop->param),sizeof(Hue_Param));
  }
Ui_hueWindow::~Ui_hueWindow()
{
  if(myCrop) delete myCrop;
  myCrop=NULL; 
  if(canvas) delete canvas;
  canvas=NULL;
}
void Ui_hueWindow::valueChanged( int f )
{
  if(lock) return;
  lock++;
  myCrop->update();
  lock--;
}

#define MYSPIN(x) w->horizontalSlider##x
#define MYCHECK(x) w->checkBox##x
//************************
uint8_t flyHue::upload(void)
{
      Ui_hueDialog *w=(Ui_hueDialog *)_cookie;

        MYSPIN(Saturation)->setValue((int)param.saturation);
        MYSPIN(Hue)->setValue((int)param.hue);
        return 1;
}
uint8_t flyHue::download(void)
{
       Ui_hueDialog *w=(Ui_hueDialog *)_cookie;
         param.hue=MYSPIN(Hue)->value();
         param.saturation=MYSPIN(Saturation)->value();
return 1;
}

/**
      \fn     DIA_getCropParams
      \brief  Handle crop dialog
*/
uint8_t DIA_getHue(Hue_Param *param,AVDMGenericVideoStream *in)
{
        uint8_t ret=0;
        
        Ui_hueWindow dialog(param,in);        
        if(dialog.exec()==QDialog::Accepted)
        {
            dialog.gather(param); 
            ret=1;
        }
        return ret;
}
//____________________________________
// EOF


