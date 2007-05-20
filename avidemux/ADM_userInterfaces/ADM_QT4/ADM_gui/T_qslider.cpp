
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
#include <math.h>

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
#include <QResizeEvent>
// moc does not like includes.... #include <ADM_qslider.h>
#define MIN_BOTTOM 0


#define zprintf(prt, ...) ;

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
        int paintCount;
        void resizeEvent ( QResizeEvent * event ) ;
        QSize maximumS;
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
    
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
 //   bool event(QEvent *event);
    void setSizePolicy ( QSizePolicy horizontal);
    QSizePolicy sizePolicy () const;
    void setMaximumSize ( const QSize & );
    QSize maximumSize () const;
public Q_SLOTS:
    void setValue(int);
    void setOrientation(Qt::Orientation);

};
//****************************************************************************************************
void ADM_QSlider::setOrientation(Qt::Orientation x)
{
  zprintf("Orientation\n");
  _slider->setOrientation(x); 
}
void ADM_QSlider::setValue(int x)
{
  zprintf("Value\n");
  _slider->setValue(x); 
}

void ADM_QSlider::setMinimum(int x)
{
  zprintf("Min\n");
  _slider->setMinimum(x); 
}
void ADM_QSlider::setMaximum(int x)
{
  zprintf("Max\n");
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
  paintCount=0;
  _slider=new QSlider(this);
  _slider->setTickInterval(1);
  _slider->setSingleStep(1);

  
  _slider->show();
  connect( _slider,SIGNAL(valueChanged(int)),this,SLOT(sliderMoved(int)));
  
}
void ADM_QSlider::sliderMoved(int i)
{
  zprintf("Moved\n");
  emit( valueChanged(i));
}
ADM_QSlider::~ADM_QSlider()
{
  if(_slider) delete _slider;
  _slider=NULL;
}
void ADM_QSlider::paintEvent(QPaintEvent *ev)
{
  int a=_A,b=_B;
  if(paintCount) return;
    zprintf("[*********************************PAINT************************]\n");
    paintCount++;
    //_slider->repaint();
    if( _max)
    {
      if(_A>_B)
      {
        b=_A;
        a=_B; 
      }
     zprintf("[QSLIDER] %u %u %u\n",_A,_B,_max); 
      // Paint!
      QPainter painter(this);
      int Wwidth,Wheight,dA,dB;
      float f;
        Wwidth=width();
        Wheight=height();

        
        f=a*Wwidth;
        f/=_max;
        dA=(int)floor(f+0.49);
        
        f=b*Wwidth;
        f/=_max;
        dB=(int)floor(f+0.49);
        
        QRectF r(dA,0,dB-dA+1,Wheight-1);
        
        painter.setPen(Qt::blue);
        
//         painter.drawLine(dA,1,dA,Wheight-MIN_BOTTOM+1);
//         painter.drawLine(dB,1,dB,Wheight-MIN_BOTTOM+1);
//         painter.drawLine(dA,Wheight-MIN_BOTTOM+2,dB,Wheight-MIN_BOTTOM+2);
        painter.drawRect(dA,0,dB-dA+1,Wheight-1);
        painter.end();
    }
    paintCount--;
}
void ADM_QSlider::setA(uint32_t a)
{
  _A=a;
  zprintf("[QSLIDER] Set A (%u,%u)\n",_A,_max);  
  if(_A>_max)
  {
    printf("[QSLIDER] A out of bound(%u,%u)\n",_A,_max);  
  }
  repaint();
}
void ADM_QSlider::setB(uint32_t a)
{
    _B=a;
    zprintf("[QSLIDER] Set B (%u,%u)\n",_B,_max);  
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
QSize ADM_QSlider::sizeHint() const
{
  return _slider->sizeHint(); 
}
QSize ADM_QSlider::minimumSizeHint() const
{
   return _slider->minimumSizeHint(); 
}
void ADM_QSlider::resizeEvent ( QResizeEvent * event ) 
{
  QSize nw,old;
  nw=event->size();
  old=event->oldSize();
  zprintf("RESIZE %dx%d->%dx%d\n",old.width(),old.height(),nw.width(),nw.height());
  if(nw.height()>MIN_BOTTOM) nw.setHeight(nw.height()-MIN_BOTTOM);
  zprintf("RESIZE2 %dx%d->%dx%d\n",old.width(),old.height(),nw.width(),nw.height());
  _slider->resize(nw);
  
}
void ADM_QSlider::setSizePolicy ( QSizePolicy horizontal )
{
    zprintf("PolicySet\n");
    _slider->setSizePolicy(horizontal);
}
QSizePolicy ADM_QSlider::sizePolicy () const
{
  return _slider->sizePolicy(); 
}
void ADM_QSlider::setMaximumSize ( const QSize &sz )
{
    maximumS=sz;
    _slider->setMaximumSize(sz);
}
/*bool ADM_QSlider::event(QEvent *xevent)
{
  return  _slider->event(xevent);
}*/

QSize ADM_QSlider::maximumSize () const
{
  return maximumS; 
}

//EOF 
