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
#include <QMessageBox>
#include <QGridLayout>
#include <QCheckBox>

#include "ADM_default.h"
#include "DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);

diaElemNotch::diaElemNotch(uint32_t yes,const char *toggleTitle, const char *tip)
  : diaElem(ELEM_NOTCH)
{
  yesno=yes;
  paramTitle=toggleTitle;
  this->tip=tip;
}

diaElemNotch::~diaElemNotch()
{
  
}
void diaElemNotch::setMe(void *dialog, void *opaque,uint32_t line)
{
  QCheckBox *box=new QCheckBox(QString::fromUtf8(paramTitle),(QWidget *)dialog);
 QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
 if( yesno)
 {
    box->setCheckState(Qt::Checked); 
 }
 box->show();
 layout->addWidget(box,line,0);
}
//******************************************************

//EOF
