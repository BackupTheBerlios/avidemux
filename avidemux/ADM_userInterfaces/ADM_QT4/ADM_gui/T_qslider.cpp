
/***************************************************************************
      Custom slider
      It is definitively ugly
      It should derive from QHorizontalSlider or something
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
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <sys/time.h>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QFrame>
#include <QtGui/QGraphicsView>
#include <QMessageBox>
#include <QPainter>
#include <QImage>
#include <QSlider>
// moc does not like includes.... #include <ADM_qslider.h>
class ADM_QSlider : public QWidget
{
        Q_OBJECT
signals:
  void valueChanged(int value);

public slots:
        void sliderMoved(int);
protected:
        QSlider *_slider;
        uint32_t _max,_A,_B;
        void paintEvent(QPaintEvent *ev);
        
public:
        ADM_QSlider(QWidget *father); 
        ~ADM_QSlider(); 
        
        void setA(uint32_t a);
        void setB(uint32_t a);
        void setNbFrames(uint32_t a);
        int  value() const;
         Qt::Orientation orientation() const;

    void setMinimum(int);
    int minimum() const;

    void setMaximum(int);
    int maximum() const;

public Q_SLOTS:
    void setValue(int);
    void setOrientation(Qt::Orientation);

};
//****************************************************************************************************
void ADM_QSlider::setOrientation(Qt::Orientation x)
{
  _slider->setOrientation(x); 
}
void ADM_QSlider::setValue(int x)
{
  _slider->setValue(x); 
}

void ADM_QSlider::setMinimum(int x)
{
  _slider->setMinimum(x); 
}
void ADM_QSlider::setMaximum(int x)
{
  _slider->setMaximum(x); 
}
 int ADM_QSlider::minimum(void) const
{
  return _slider->minimum(); 
}
int ADM_QSlider::maximum(void) const
{
  return _slider->maximum(); 
}
int ADM_QSlider::value(void) const
{
  return _slider->value(); 
}

ADM_QSlider::ADM_QSlider(QWidget *father) : QWidget(father)
{
  _max=_A=_B=0;
  _slider=new QSlider(this);
  _slider->show();
  connect( _slider,SIGNAL(valueChanged(int)),this,SLOT(sliderMoved(int)));
  
}
void ADM_QSlider::sliderMoved(int i)
{
  emit( valueChanged(i));
}
ADM_QSlider::~ADM_QSlider()
{
  if(_slider) delete _slider;
  _slider=NULL;
}
void ADM_QSlider::paintEvent(QPaintEvent *ev)
{
    printf("[*********************************PAINT************************]\n");
    _slider->repaint();
    if(_A && _B && _max)
    {
      printf("[QSLIDER] %u %u %u\n",_A,_B,_max); 
    }
}
void ADM_QSlider::setA(uint32_t a)
{
  _A=a;
  printf("[QSLIDER] Set A (%u,%u)\n",_A,_max);  
  if(_A>_max)
  {
    printf("[QSLIDER] A out of bound(%u,%u)\n",_A,_max);  
  }
  repaint();
}
void ADM_QSlider::setB(uint32_t a)
{
    _B=a;
    printf("[QSLIDER] Set B (%u,%u)\n",_B,_max);  
  if(_B>_max)
  {
    printf("[QSLIDER] B out of bound(%u,%u)\n",_B,_max);  
  }
  repaint();
}
void ADM_QSlider::setNbFrames(uint32_t a)
{
  _max=a; 
  repaint();
}

//EOF 
