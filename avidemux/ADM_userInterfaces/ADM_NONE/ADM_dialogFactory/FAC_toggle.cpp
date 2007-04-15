/***************************************************************************
  FAC_toggle.cpp
  Handle dialog factory element : Toggle
  (C) 2006 Mean Fixounet@free.fr 
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

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"




diaElemToggle::diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip)
  : diaElem(ELEM_TOGGLE)
{
}

diaElemToggle::~diaElemToggle()
{
  
}
void diaElemToggle::setMe(void *dialog, void *opaque,uint32_t l)
{
}
void diaElemToggle::getMe(void)
{
}
void diaElemToggle::enable(uint32_t onoff) {}
//******************************************************
diaElemInteger::diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
 }

diaElemInteger::~diaElemInteger()
{
  
}
void diaElemInteger::setMe(void *dialog, void *opaque,uint32_t line)
{
 
}
void diaElemInteger::getMe(void)
{
 
}
void diaElemInteger::enable(uint32_t onoff) {}
//******************************************************
diaElemUInteger::diaElemUInteger(uint32_t *intValue,const char *toggleTitle, uint32_t min, uint32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
 }

diaElemUInteger::~diaElemUInteger()
{
  
}
void diaElemUInteger::setMe(void *dialog, void *opaque,uint32_t line)
{
 
}
void diaElemUInteger::getMe(void)
{
 
}
void diaElemUInteger::enable(uint32_t onoff) {}
//******************************************************

diaElemFloat::diaElemFloat(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, 
                            ELEM_TYPE_FLOAT min, ELEM_TYPE_FLOAT max,const char *tip)
  : diaElem(ELEM_FLOAT)
{
}

diaElemFloat::~diaElemFloat()
{
  
}
void diaElemFloat::setMe(void *dialog, void *opaque,uint32_t line)
{
  
}
void diaElemFloat::getMe(void)
{
 
}
void diaElemFloat::enable(uint32_t onoff) {}
//******************************************************

diaElemBar::diaElemBar(uint32_t percent,const char *toggleTitle)
  : diaElem(ELEM_BAR)
{
}

diaElemBar::~diaElemBar()
{
  
}
void diaElemBar::setMe(void *dialog, void *opaque,uint32_t line)
{
  
}
void diaElemBar::getMe(void)
{
 
}

//******************************************************

diaElemMenu::diaElemMenu(uint32_t *intValue,const char *itle, uint32_t nb, 
               const diaMenuEntry *menu,const char *tip)
  : diaElem(ELEM_MENU)
{
}

diaElemMenu::~diaElemMenu()
{
  
}
void diaElemMenu::setMe(void *dialog, void *opaque,uint32_t line)
{
  
}
void diaElemMenu::getMe(void)
{
 
}
uint8_t   diaElemMenu::link(diaMenuEntry *entry,uint32_t onoff,diaElem *w)
{
}
void   diaElemMenu::enable(uint32_t onoff)
{}
void diaElemMenu::finalize(void)
{ 
}
//*****************
diaElemFileRead::diaElemFileRead(char **filename,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_FILE_READ)
{
 
}

diaElemFileRead::~diaElemFileRead()
{
  
}
void diaElemFileRead::setMe(void *dialog, void *opaque,uint32_t line)
{
 
  
}
void diaElemFileRead::getMe(void)
{
 
}
void   diaElemFileRead::enable(uint32_t onoff)
{}
#include "ADM_encoder/ADM_vidEncode.hxx"
  diaElemBitrate::diaElemBitrate(COMPRES_PARAMS *p,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_BITRATE)
{
  
}
diaElemBitrate::~diaElemBitrate()
{
  
}
void diaElemBitrate::setMe(void *dialog, void *opaque,uint32_t line)
{
 
  
}
void diaElemBitrate::getMe(void)
{
 
}
//******************************************************
diaElemReadOnlyText::diaElemReadOnlyText(char *readyOnly,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_ROTEXT)
{
 
} 
diaElemReadOnlyText::~diaElemReadOnlyText()
{
  
}
void diaElemReadOnlyText::setMe(void *dialog, void *opaque,uint32_t line)
{
 
  
}
void diaElemReadOnlyText::getMe(void)
{
 
}

//******************************************************
diaElemNotch::diaElemNotch(uint32_t yes,const char *toggleTitle, const char *tip)
  : diaElem(ELEM_NOTCH)
{
 
}

diaElemNotch::~diaElemNotch()
{
  
}
void diaElemNotch::setMe(void *dialog, void *opaque,uint32_t line)
{
  
}
//******************************************************
diaElemDirSelect::diaElemDirSelect(char **filename,const char *toggleTitle,const char *tip)  : diaElem(ELEM_NOTCH) {}
diaElemDirSelect::~diaElemDirSelect() {}
void diaElemDirSelect::setMe(void *dialog, void *opaque,uint32_t line) {}
void diaElemDirSelect::getMe(void) {}
  
void diaElemDirSelect::changeFile(void) {}
void   diaElemDirSelect::enable(uint32_t onoff){}
//******************************************************
diaElemText::diaElemText(char **text,const char *toggleTitle,const char *tip)
    : diaElem(ELEM_NOTCH) {}
diaElemText::~diaElemText() {}
void diaElemText::setMe(void *dialog, void *opaque,uint32_t line) {}
void diaElemText::getMe(void) {}  
void diaElemText::enable(uint32_t onoff) {};
            
//******************************************************
diaElemMenuDynamic::diaElemMenuDynamic(uint32_t *intValue,const char *itle, uint32_t nb, 
                diaMenuEntryDynamic **menu,const char *tip)
  : diaElem(ELEM_MENU)
{
}

diaElemMenuDynamic::~diaElemMenuDynamic()
{ 
}
void diaElemMenuDynamic::setMe(void *dialog, void *opaque,uint32_t line)
{
}

void diaElemMenuDynamic::getMe(void)
{ 
}
uint8_t   diaElemMenuDynamic::link(diaMenuEntryDynamic *entry,uint32_t onoff,diaElem *w)
{
}
void   diaElemMenuDynamic::enable(uint32_t onoff)
{}
void diaElemMenuDynamic::finalize(void)
{ 
}
//******************************************************
diaElemFrame::diaElemFrame(const char *toggleTitle, const char *tip)
  : diaElem(ELEM_FRAME)
{
  
}
void diaElemFrame::swallow(diaElem *widget)
{
 
}
diaElemFrame::~diaElemFrame()
{
  
}
void diaElemFrame::setMe(void *dialog, void *opaque,uint32_t line)
{
 
}
void diaElemFrame::getMe(void)
{
  
}
void diaElemFrame::enable(uint32_t onoff)
{

}
void diaElemFrame::finalize(void)
{
}
//EOF
