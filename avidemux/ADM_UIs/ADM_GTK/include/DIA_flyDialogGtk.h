/****************************************************************************
 copyright            : (C) 2007 by mean
 email                : fixounet@free.fr
 
 Simple class to do filter that have a configuration that have real time effect
 
 Case 1: The filter process YUV
 
 YUV-> Process->YUVOUT->YUV2RGB->RGBUFFEROUT
 
 Case 2: The filter process RGV
 
 YUV-> YUV2RGB->RGB->Process->RGBUFFEROUT
 
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef ADM_FLY_DIALOG_GTKH
#define ADM_FLY_DIALOG_GTKH
#include "DIA_flyDialog.h"
class ADM_flyDialogGtk : public ADM_flyDialog
{
public:
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

#endif
