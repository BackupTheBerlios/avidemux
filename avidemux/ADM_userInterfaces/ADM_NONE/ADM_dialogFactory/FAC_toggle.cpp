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
//EOF
