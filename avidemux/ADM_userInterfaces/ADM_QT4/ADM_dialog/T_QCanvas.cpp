/***************************************************************************
    copyright            : (C) 2006 by mean
    email                : fixounet@free.fr
***************************************************************************/

/***************************************************************************
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
***************************************************************************///

#include "config.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



#include <QtGui/QGraphicsView>
#include <QtGui/QSlider>

#include "default.h"
class  ADM_QCanvas : public QWidget
{
     Q_OBJECT
     signals:
        
        
   public slots:
   public :
         uint8_t *dataBuffer;
  protected:
         uint32_t _w,_h;
  public:
        ADM_QCanvas(QWidget *z,uint32_t w,uint32_t h)  ;
        ~ADM_QCanvas() ;
        void paintEvent(QPaintEvent *ev);
        void changeSize(uint32_t w,uint32_t h);
};

void ADM_QCanvas::changeSize(uint32_t w,uint32_t h)
{
	_w=w;
	_h=h;
	dataBuffer=NULL;
    resize(w,h);
}

ADM_QCanvas::ADM_QCanvas(QWidget *z,uint32_t w,uint32_t h) : QWidget(z) 
{
        _w=w;
        _h=h;
        dataBuffer=NULL;
        resize(w,h);
}
ADM_QCanvas::~ADM_QCanvas() 
{
}
/**
    \fn paintEvent( QPaintEvent *ev))
    \brief Repaint our "video" widget, ignore when accelRender is on
*/
void ADM_QCanvas::paintEvent(QPaintEvent *ev)
{
    if(!dataBuffer)
    {
      printf("Nothing to draw\n");
      return ;
    }
    QImage image(dataBuffer,_w,_h,QImage::Format_RGB32);
      QPainter painter(this);
      painter.drawImage(QPoint(0,0),image);
      painter.end();
}
//EOF
