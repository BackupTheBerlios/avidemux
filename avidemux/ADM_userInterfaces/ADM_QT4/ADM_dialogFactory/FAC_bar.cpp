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

#include "config.h"


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QDialog>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>
#include <QProgressBar>

#include "ADM_default.h"
#include "DIA_factory.h"
#include "ADM_assert.h"


extern const char *shortkey(const char *);



//********************************************************************
diaElemBar::diaElemBar(uint32_t percent,const char *toggleTitle)
  : diaElem(ELEM_BAR)
{
  per=percent;
  paramTitle=shortkey(toggleTitle);
 }

diaElemBar::~diaElemBar()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemBar::setMe(void *dialog, void *opaque,uint32_t line)
{
  QProgressBar *box=new QProgressBar((QWidget *)dialog);
  QGridLayout *layout=(QGridLayout*) opaque;
 
  box->setMinimum(0);
  box->setMaximum(100);
  box->setValue(per);
  box->show();
 
 QLabel *text=new QLabel(QString::fromUtf8(this->paramTitle),(QWidget *)dialog);
 text->setBuddy(box);
 layout->addWidget(text,line,0);
 layout->addWidget(box,line,1);
 
}
void diaElemBar::getMe(void)
{
}

//EOF
