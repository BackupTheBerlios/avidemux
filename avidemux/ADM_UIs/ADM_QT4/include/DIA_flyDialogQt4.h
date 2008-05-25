/** **************************************************************************
        \fn DIA_flyDialogQt4.h
 copyright            : (C) 2007 by mean
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
#ifndef ADM_FLY_DIALOG_QT4H
#define ADM_FLY_DIALOG_QT4H
#include "ADM_image.h"
#include "ADM_videoFilter.h"
#include "DIA_flyDialog.h"
class ADM_flyDialogQt4 : public ADM_flyDialog
{
public:
  ADM_flyDialogQt4(uint32_t width, uint32_t height, AVDMGenericVideoStream *in,
                              void *canvas, void *slider, int yuv, ResizeMethod resizeMethod);

  virtual uint8_t  cleanup2(void);
  virtual uint8_t  isRgbInverted(void);
  virtual uint8_t  display(void);
  virtual float   calcZoomFactor(void);
  virtual uint32_t sliderGet(void);
  virtual uint8_t  sliderSet(uint32_t value);
  virtual void    postInit(uint8_t reInit);
  virtual void    setupMenus (const void * params,
                         const MenuMapping * menu_mapping,
                        uint32_t menu_mapping_count) ;
  virtual void  getMenuValues ( void * mm,
                    const MenuMapping * menu_mapping,
                    uint32_t menu_mapping_count) ;
  virtual  const MenuMapping *lookupMenu (const char * widgetName,
                                               const MenuMapping * menu_mapping,
                                               uint32_t menu_mapping_count) ;
  virtual uint32_t  getMenuValue (const MenuMapping * mm) ;          
};


class ADM_QCanvas : public QWidget
{
protected:
	uint32_t _w,_h;
public:
	uint8_t *dataBuffer;

	ADM_QCanvas(QWidget *z, uint32_t w, uint32_t h);
	~ADM_QCanvas();
	void paintEvent(QPaintEvent *ev);
	void changeSize(uint32_t w, uint32_t h);
};

#endif
