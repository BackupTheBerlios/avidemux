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

#include <QtGui/QGraphicsView>
#include <QtGui/QSlider>

#include "default.h"
#include "ADM_assert.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"

void ADM_flyDialog::postInit()
{
	QWidget *graphicsView = ((ADM_QCanvas*)_canvas)->parentWidget();

	graphicsView->setMinimumSize(_w, _h);
	graphicsView->resize(_w, _h);
}

float ADM_flyDialog::calcZoomFactor(void) {return 1;}

uint8_t  ADM_flyDialog::display(void)
{
   ADM_QCanvas *view=(ADM_QCanvas *)_canvas;
   ADM_assert(view);
   view->dataBuffer=_rgbBufferOut;
   if(!_rgbBufferOut)
   {
      printf("flyDialog: No rgbuffer ??\n"); 
   } 
   view->repaint();
  return 1; 
}
uint32_t ADM_flyDialog::sliderGet(void)
{
  QSlider  *slide=(QSlider *)_slider;
  ADM_assert(slide);
  return slide->value();
  
}
uint8_t     ADM_flyDialog::sliderSet(uint32_t value)
{
  QSlider  *slide=(QSlider *)_slider;
  ADM_assert(slide);
  slide->setValue(value);
  return 1; 
}
uint8_t  ADM_flyDialog::isRgbInverted(void)
{
  return 1; 
}

//EOF
