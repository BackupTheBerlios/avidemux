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
#include <QLabel>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);

diaElemFrame::diaElemFrame(const char *toggleTitle, const char *tip)
  : diaElem(ELEM_FRAME)
{
  param=NULL;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
   nbElems=0;
  frameSize=0;
  setSize(2);
}
void diaElemFrame::swallow(diaElem *widget)
{
   elems[nbElems]=widget;
  frameSize+=widget->getSize();
 // setSize(frameSize);
  nbElems++;
  ADM_assert(nbElems<DIA_MAX_FRAME); 
}
diaElemFrame::~diaElemFrame()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemFrame::setMe(void *dialog, void *opaque,uint32_t line)
{
  
   QGridLayout *layout=(QGridLayout*) opaque;  
   QGridLayout *layout2;
   
   layout2=new QGridLayout((QWidget *)dialog);
   myWidget=(void *)layout2; 

    QLabel *text=new QLabel( (QWidget *)dialog);
    QString string(paramTitle);
    
    string="<b>"+string+"</b>";
    text->setText(string);
 layout->addWidget(text,line,0);
 layout->addLayout(layout2,line+1,0);
 int  v=0;
  for(int i=0;i<nbElems;i++)
  {
    elems[i]->setMe(dialog,layout2,v); 
    v+=elems[i]->getSize();
  }
  myWidget=(void *)layout2;
}
//*****************************
void diaElemFrame::getMe(void)
{
   for(int i=0;i<nbElems;i++)
  {
    elems[i]->getMe(); 
  }
}
void diaElemFrame::finalize(void)
{
   for(int i=0;i<nbElems;i++)
  {
    elems[i]->finalize(); 
  }
}
void diaElemFrame::enable(uint32_t onoff)
{
  
}

//******************************************************

//EOF
