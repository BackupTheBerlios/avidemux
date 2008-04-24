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

#define Ui_Dialog Ui_chromashiftDialog
#include "ui_chromashift.h"
#undef Ui_Dialog

#include "ADM_default.h"


#include "ADM_image.h"
#include "ADM_videoFilter.h"
#include "ADM_colorspace/ADM_rgb.h"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"
#include "ADM_videoFilter/ADM_vidChromaShift_param.h"
#include "DIA_flyChromaShift.h"
class Ui_chromaShiftWindow : public QDialog
 {
     Q_OBJECT
 protected : 
    int lock;
 public:
     flyChromaShift *myCrop;
     ADM_QCanvas *canvas;
     Ui_chromaShiftWindow(CHROMASHIFT_PARAM *param,AVDMGenericVideoStream *in);
     ~Ui_chromaShiftWindow();
     Ui_chromashiftDialog ui;
 public slots:
      void gather(CHROMASHIFT_PARAM *param);
      //void update(int i);
 private slots:
   void sliderUpdate(int foo);
   void valueChanged(int foo);

 private:
     
 };
  Ui_chromaShiftWindow::Ui_chromaShiftWindow(CHROMASHIFT_PARAM *param,AVDMGenericVideoStream *in)
  {
    uint32_t width,height;
        ui.setupUi(this);
        lock=0;
        
        // Allocate space for green-ised video
        width=in->getInfo()->width;
        height=in->getInfo()->height;
        
        int boundary=width/2;
        
        ui.spinBoxU->setMaximum(boundary/2);
        ui.spinBoxU->setMinimum(-boundary);
        ui.spinBoxV->setMaximum(boundary/2);
        ui.spinBoxV->setMinimum(-boundary);
        
        canvas=new ADM_QCanvas(ui.graphicsView,width,height);
        
        myCrop=new flyChromaShift( width, height,in,canvas,ui.horizontalSlider);
        memcpy(&(myCrop->param),param,sizeof(CHROMASHIFT_PARAM));
        myCrop->_cookie=&ui;
        myCrop->upload();
        myCrop->sliderChanged();


        connect( ui.horizontalSlider,SIGNAL(valueChanged(int)),this,SLOT(sliderUpdate(int)));
#define SPINNER(x) connect( ui.spinBox##x,SIGNAL(valueChanged(int)),this,SLOT(valueChanged(int))); 
          SPINNER(U);
          SPINNER(V);
  }
  void Ui_chromaShiftWindow::sliderUpdate(int foo)
  {
    myCrop->sliderChanged();
  }
  void Ui_chromaShiftWindow::gather(CHROMASHIFT_PARAM *param)
  {
    
        myCrop->download();
        memcpy(param,&(myCrop->param),sizeof(CHROMASHIFT_PARAM));
  }
Ui_chromaShiftWindow::~Ui_chromaShiftWindow()
{
  if(myCrop) delete myCrop;
  myCrop=NULL; 
  if(canvas) delete canvas;
  canvas=NULL;
}
void Ui_chromaShiftWindow::valueChanged( int f )
{
  if(lock) return;
  lock++;
  myCrop->update();
  lock--;
}

#define MYSPIN(x) w->spinBox##x
//************************
uint8_t flyChromaShift::upload(void)
{
      Ui_chromashiftDialog *w=(Ui_chromashiftDialog *)_cookie;

        MYSPIN(U)->setValue(param.u);
        MYSPIN(V)->setValue(param.v);
        return 1;
}
uint8_t flyChromaShift::download(void)
{
       Ui_chromashiftDialog *w=(Ui_chromashiftDialog *)_cookie;
       param.u= MYSPIN(U)->value();
       param.v= MYSPIN(V)->value();
       
}

/**
      \fn     DIA_getChromaShift
      \brief  Handle crop dialog
*/
uint8_t DIA_getChromaShift( AVDMGenericVideoStream *in,CHROMASHIFT_PARAM    *param )
{
        uint8_t ret=0;
        
        Ui_chromaShiftWindow dialog(param,in);        
        if(dialog.exec()==QDialog::Accepted)
        {
            dialog.gather(param); 
            ret=1;
        }
        return ret;
}
//____________________________________
// EOF


