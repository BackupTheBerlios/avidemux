/***************************************************************************
  FAC_integer.cpp
  Handle dialog factory element : Integer
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

class SpinSlider : public QWidget
{
    Q_OBJECT

public:
    SpinSlider (QWidget * parent = 0);
    int value() const { return spinner->value(); }

signals:
    void valueChanged (int value);

public slots:
    void setValue (int value);
    void setMinimum (int value);
    void setMaximum (int value);

private:
    QSlider * slider;
    QSpinBox * spinner;
};

SpinSlider::SpinSlider (QWidget * parent)
    : QWidget (parent)
{
    slider = new QSlider (Qt::Horizontal);
    spinner = new QSpinBox;

    QWidget::connect (slider, SIGNAL(valueChanged(int)), spinner, SLOT(setValue(int)));
    QWidget::connect (spinner, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)));
    QWidget::connect (spinner, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));

    QHBoxLayout * hbox = new QHBoxLayout;
    hbox->addWidget (slider);
    hbox->addWidget (spinner);

    setLayout (hbox);
}

void SpinSlider::setValue (int value)
{
    spinner->setValue (value);
}

void SpinSlider::setMinimum (int value)
{
    spinner->setMinimum (value);
    slider->setMinimum (value);
}

void SpinSlider::setMaximum (int value)
{
    spinner->setMaximum (value);
    slider->setMaximum (value);
}

//----------------------------------------------------------------------

template <typename T>
diaElemGenericSlider<T>::diaElemGenericSlider(T *value,const char *toggleTitle, T min,T max,T incr,const char *tip)
  : diaElem(ELEM_SLIDER)
{
  param=(void *)value;
  paramTitle=shortkey(toggleTitle);
  this->min=min;
  this->max=max;
  this->incr=incr;
  this->tip=tip;
  size = 2;
}

template <typename T>
diaElemGenericSlider<T>::~diaElemGenericSlider()
{ 
  if(paramTitle)
    delete paramTitle;
}

template <typename T>
void diaElemGenericSlider<T>::setMe(void *dialog, void *opaque,uint32_t line)
{
  SpinSlider *slider = new SpinSlider ((QWidget *)dialog);
  slider->setMinimum(min);
  slider->setMaximum(max);
  slider->setValue(*(T *)param);
  slider->show();
 
  myWidget = (void *)slider;

  QLabel *text = new QLabel (QString::fromUtf8(paramTitle), (QWidget *)dialog);
  text->setBuddy (slider);

  QGridLayout *layout = (QGridLayout*) opaque;
  layout->addWidget(text,line,0,1,2);
  layout->addWidget(slider,line + 1,0,1,2);
}

template <typename T>
void diaElemGenericSlider<T>::getMe(void)
{
  SpinSlider *box=(SpinSlider *)myWidget;
  T val=box->value();
  if(val<min) val=min;
  if(val>max) val=max;
  *(T *)param=val;
}

template <typename T>
void diaElemGenericSlider<T>::enable(uint32_t onoff) 
{
  SpinSlider *box=(SpinSlider *)myWidget;
  ADM_assert(box);
  if(onoff)
    box->setEnabled(1);
  else
    box->setDisabled(1);
}

template class diaElemGenericSlider <int32_t>;
template class diaElemGenericSlider <uint32_t>;
template class diaElemGenericSlider <ELEM_TYPE_FLOAT>;

// float doesn't really work right.  If anyone ever uses it, we can fix it by
// providing a template specialization here that uses QDoubleSpinBox and
// intercepts the signals to convert between an integer used by the slider and
// the actual value (a double) used by the spinbox.  I don't have time to do
// that right now (already spent too long on this) and no one is using a
// diaElemFSlider yet anyway.

//EOF

