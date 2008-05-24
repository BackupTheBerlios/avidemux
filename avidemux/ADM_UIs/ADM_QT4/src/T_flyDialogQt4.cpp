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


#include <QtGui/QGraphicsView>
#include <QtGui/QSlider>

#include "ADM_default.h"

#include "ADM_videoFilter.h"
#include "DIA_flyDialog.h"
#include "DIA_flyDialogQt4.h"

void ADM_flyDialogQt4::postInit(uint8_t reInit)
{
	QWidget *graphicsView = ((ADM_QCanvas*)_canvas)->parentWidget();
	QSlider  *slider=(QSlider *)_slider;
	
	graphicsView->setMinimumSize(_w, _h);
	graphicsView->resize(_w, _h);
	uint32_t nbFrames=_in->getInfo()->nb_frames; 
	slider->setMaximum(nbFrames);
}

float ADM_flyDialogQt4::calcZoomFactor(void) {return 1;}

uint8_t  ADM_flyDialogQt4::display(void)
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
uint32_t ADM_flyDialogQt4::sliderGet(void)
{
  QSlider  *slide=(QSlider *)_slider;
  ADM_assert(slide);
  return slide->value();
  
}
uint8_t     ADM_flyDialogQt4::sliderSet(uint32_t value)
{
  QSlider  *slide=(QSlider *)_slider;
  ADM_assert(slide);
  slide->setValue(value);
  return 1; 
}
uint8_t  ADM_flyDialogQt4::isRgbInverted(void)
{
  return 1; 
}
uint8_t ADM_flyDialogQt4::cleanup2(void)
{
  return 1; 
}
/** WTF are these ???? */
  void    ADM_flyDialogQt4::setupMenus (const void * params,
                       const MenuMapping * menu_mapping,
                      uint32_t menu_mapping_count) 
  {ADM_assert(0);return ;};
  void  ADM_flyDialogQt4::getMenuValues ( void * mm,
                  const MenuMapping * menu_mapping,
                  uint32_t menu_mapping_count)
  {ADM_assert(0);return;};
  uint32_t  ADM_flyDialogQt4::getMenuValue (const MenuMapping * mm)  
  {ADM_assert(0);return 0;};                    
   const MenuMapping *ADM_flyDialogQt4::lookupMenu (const char * widgetName,
                                             const MenuMapping * menu_mapping,
                                             uint32_t menu_mapping_count)  
  {ADM_assert(0);return NULL;};

//EOF
