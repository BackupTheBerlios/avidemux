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
#include <QMessageBox>
#include <QGridLayout>
#include <QCheckBox>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);

diaElemToggle::diaElemToggle(uint32_t *toggleValue,const char *toggleTitle, const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)toggleValue;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
  myWidget=NULL;
}

diaElemToggle::~diaElemToggle()
{
  QCheckBox *box=(QCheckBox *)myWidget;
 // if(box) delete box;
  myWidget=NULL;
  if(paramTitle)
    delete paramTitle;
}
void diaElemToggle::setMe(void *dialog, void *opaque,uint32_t l)
{
 QCheckBox *box=new QCheckBox(paramTitle,(QWidget *)dialog);
 QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
 if( *(uint32_t *)param)
 {
    box->setCheckState(Qt::Checked); 
 }
 box->show();
  layout->addWidget(box,l,0);
}
void diaElemToggle::getMe(void)
{
  QCheckBox *box=(QCheckBox *)myWidget;
  uint32_t *val=(uint32_t *)param;
  if(Qt::Checked==box->checkState())
  {
    *val=1; 
  }else
    *val=0;
}
void diaElemToggle::enable(uint32_t onoff) 
{
  QCheckBox *box=(QCheckBox *)myWidget;
  ADM_assert(box);
  if(onoff)
    box->setEnabled(1);
  else
    box->setDisabled(1);
}



//******************************************************

//EOF
