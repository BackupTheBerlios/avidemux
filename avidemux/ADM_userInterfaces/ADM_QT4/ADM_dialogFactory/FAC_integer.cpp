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
#include <QSlider>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"


extern const char *shortkey(const char *);



//********************************************************************
diaElemInteger::diaElemInteger(int32_t *intValue,const char *toggleTitle, int32_t min, int32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)intValue;
  paramTitle=shortkey(toggleTitle);
  this->min=min;
  this->max=max;
  this->tip=tip;
 }

diaElemInteger::~diaElemInteger()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemInteger::setMe(void *dialog, void *opaque,uint32_t line)
{
  QSpinBox *box=new QSpinBox((QWidget *)dialog);
  QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
   
 box->setMinimum(min);
 box->setMaximum(max);
 box->setValue(*(int32_t *)param);
 
 box->show();
 
 QLabel *text=new QLabel( this->paramTitle,(QWidget *)dialog);
 text->setBuddy(box);
 layout->addWidget(text,line,0);
 layout->addWidget(box,line,1);
 
}
void diaElemInteger::getMe(void)
{
  int32_t val;
 QSpinBox *box=(QSpinBox *)myWidget;
 val=box->value();
 if(val<min) val=min;
 if(val>max) val=max;
 *(int32_t *)param=val;
 
}
void diaElemInteger::enable(uint32_t onoff) 
{
 QSpinBox *box=(QSpinBox *)myWidget;
  ADM_assert(box);
  if(onoff)
    box->setEnabled(1);
  else
    box->setDisabled(1);
}
//******************************************************
diaElemUInteger::diaElemUInteger(uint32_t *intValue,const char *toggleTitle, uint32_t min, uint32_t max,const char *tip)
  : diaElem(ELEM_TOGGLE)
{
  param=(void *)intValue;
  paramTitle=shortkey(toggleTitle);
  this->min=min;
  this->max=max;
  this->tip=tip;
 }
 

diaElemUInteger::~diaElemUInteger()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemUInteger::setMe(void *dialog, void *opaque,uint32_t line)
{
  QSpinBox *box=new QSpinBox((QWidget *)dialog);
  QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
   
 box->setMinimum(min);
 box->setMaximum(max);
 box->setValue(*(uint32_t *)param);
 
 box->show();
 
 QLabel *text=new QLabel( this->paramTitle,(QWidget *)dialog);
 text->setBuddy(box);
 layout->addWidget(text,line,0);
 layout->addWidget(box,line,1);
}
void diaElemUInteger::getMe(void)
{
   uint32_t val;
 QSpinBox *box=(QSpinBox *)myWidget;
 val=box->value();
 if(val<min) val=min;
 if(val>max) val=max;
 *(uint32_t *)param=val;

}

void diaElemUInteger::enable(uint32_t onoff) 
{
 QSpinBox *box=(QSpinBox *)myWidget;
  ADM_assert(box);
  if(onoff)
    box->setEnabled(1);
  else
    box->setDisabled(1);
}
//********************************************
#if 0
class diaElemSlider : public diaElem
{
  protected:
    
    uint32_t _min,_max;
public:
            diaElemSlider(uint32_t *value,const char *toggleTitle, uint32_t min,uint32_t max,const char *tip=NULL);
  virtual   ~diaElemSlider() ;
  void      setMe(void *dialog, void *opaque,uint32_t line);
  void      getMe(void);
  void      enable(uint32_t onoff) ;
};
#endif
#include "../ADM_gui/ADM_qslider.h"

#define ADM_TEST_ADM_SLIDER
#ifdef ADM_TEST_ADM_SLIDER
#define MYSLIDERTYPE  ADM_QSlider
#else
#define MYSLIDERTYPE  QSlider
#endif
             
diaElemSlider::diaElemSlider(uint32_t *value,const char *toggleTitle, uint32_t min,uint32_t max,const char *tip)
  : diaElem(ELEM_SLIDER)
{
  param=(void *)value;
  paramTitle=shortkey(toggleTitle);
  this->_min=min;
  this->_max=max;
  this->tip=tip;
  
 }
 

diaElemSlider::~diaElemSlider()
{ 
  if(paramTitle)
    delete paramTitle;
}
void diaElemSlider::setMe(void *dialog, void *opaque,uint32_t line)
{
  MYSLIDERTYPE *box=new MYSLIDERTYPE((QWidget *)dialog);
  box->setOrientation ( Qt::Horizontal );
  
  QGridLayout *layout=(QGridLayout*) opaque;
 myWidget=(void *)box; 
   
 box->setMinimum(_min);
 box->setMaximum(_max);
 box->setValue(*(uint32_t *)param);
#ifdef ADM_TEST_ADM_SLIDER
  box->setNbFrames(_max); 
  box->setA(_max/3);
  box->setB((_max*2)/3);
#endif
 box->show();
 
 QLabel *text=new QLabel( this->paramTitle,(QWidget *)dialog);
 text->setBuddy(box);
 layout->addWidget(text,line,0);
 layout->addWidget(box,line,1);
}
void diaElemSlider::getMe(void)
{
  uint32_t val;
 MYSLIDERTYPE *box=(MYSLIDERTYPE *)myWidget;
 val=box->value();
 if(val<_min) val=_min;
 if(val>_max) val=_max;
 *(uint32_t *)param=val;
}

void diaElemSlider::enable(uint32_t onoff) 
{
  MYSLIDERTYPE *box=(MYSLIDERTYPE *)myWidget;
  ADM_assert(box);
  if(onoff)
    box->setEnabled(1);
  else
    box->setDisabled(1);
}
//EOF

