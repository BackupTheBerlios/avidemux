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
#include <stdlib.h>
#include <math.h>

#include <QDialog>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"


extern const char *shortkey(const char *);



//********************************************************************
diaElemReadOnlyText::diaElemReadOnlyText(char *readyOnly,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)readyOnly;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
 }

diaElemReadOnlyText::~diaElemReadOnlyText()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemReadOnlyText::setMe(void *dialog, void *opaque,uint32_t line)
{

  QGridLayout *layout=(QGridLayout*) opaque;

   
 
 QLabel *text=new QLabel( this->paramTitle,(QWidget *)dialog);
  QLabel *text2=new QLabel( (char *)param,(QWidget *)dialog);
 text->setBuddy(text2);
 layout->addWidget(text,line,0);
 layout->addWidget(text2,line,1);
 myWidget=(void *)text2;  
}
void diaElemReadOnlyText::getMe(void)
{

 
}
//EOF