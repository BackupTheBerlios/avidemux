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
diaElemFloat::diaElemFloat(ELEM_TYPE_FLOAT *intValue,const char *toggleTitle, ELEM_TYPE_FLOAT min, ELEM_TYPE_FLOAT max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)intValue;
  paramTitle=shortkey(toggleTitle);
  this->min=min;
  this->max=max;
  this->tip=tip;
 }

diaElemFloat::~diaElemFloat()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemFloat::setMe(void *dialog, void *opaque,uint32_t line)
{
  QDoubleSpinBox *box=new QDoubleSpinBox((QWidget *)dialog);
  QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
   
 box->setMinimum(min);
 box->setMaximum(max);
 box->setValue(*(ELEM_TYPE_FLOAT *)param);
 
 box->show();
 
 QLabel *text=new QLabel( this->paramTitle,(QWidget *)dialog);
 text->setBuddy(box);
 layout->addWidget(text,line,0);
 layout->addWidget(box,line,1);
 
}
void diaElemFloat::getMe(void)
{
  double val;
 QDoubleSpinBox *box=(QDoubleSpinBox *)myWidget;
 val=box->value();
 if(val<min) val=min;
 if(val>max) val=max;
 *(ELEM_TYPE_FLOAT *)param=val;
 
}