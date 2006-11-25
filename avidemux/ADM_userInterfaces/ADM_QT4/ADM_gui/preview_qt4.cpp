
/***************************************************************************
    Handle all redraw operation for QT4 
    
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

#include "fourcc.h"
#include "avio.hxx"

#include "ADM_colorspace/ADM_rgb.h"
#include "GUI_render.h"
    

#include <ADM_assert.h>
    
void UI_QT4VideoWidget(QFrame *host);

//****************************************************************************************************
void GUI_PreviewInit(uint32_t w , uint32_t h, uint32_t modal)
{}
//****************************************************************************************************
uint8_t 	GUI_PreviewUpdate(uint8_t *data)
{
  return 1;
}
//****************************************************************************************************
void 	GUI_PreviewEnd( void)
{}
//****************************************************************************************************
uint8_t  	GUI_PreviewRun(uint8_t *data)
{
  return 1;
}
//****************************************************************************************************
uint8_t GUI_StillAlive( void )
{
  return 1;
}
//****************************************************************************************************
void GUI_PreviewShow(uint32_t w, uint32_t h, uint8_t *data)
{
}
//****************************************************************************************************
/*
  Function to display
  Warning the incoming data are YUV!
  They are translated to RGB32 by the colorconv instance.

*/
static uint8_t *rgbDataBuffer=NULL;
static uint32_t displayW=0,displayH=0;
static ColYuvRgb rgbConverter(640,480);
static uint8_t GUI_ConvertRGB(uint8_t * in, uint8_t * out, uint32_t w, uint32_t h);
//****************************************************************************************************
/*
  This is the class that will display the video images.
  It is a base QWidget where the image will be put by painter.

*/
class  ADM_Qvideo : public QWidget
{
     Q_OBJECT
    
  signals:
        
        
   public slots:
        
  public:
        ADM_Qvideo(QWidget *z) : QWidget(z) 
        {
          
        }
        ~ADM_Qvideo() {};
        void paintEvent(QPaintEvent *ev)
        {
          if(!displayW || !displayH || !rgbDataBuffer)
          {
            printf("Nothing to draw\n");
            return ;
          }
          QImage image(rgbDataBuffer,displayW,displayH,QImage::Format_RGB32);
            QPainter painter(this);
            painter.drawImage(QPoint(0,0),image);
            painter.end();
        }
};
ADM_Qvideo *videoWindow=NULL;
//****************************************************************************************************
void UI_QT4VideoWidget(QFrame *host)
{
   videoWindow=new ADM_Qvideo(host);
   videoWindow->resize(320,200);
   videoWindow->show();
}
uint8_t renderInit( void )
{
  return 1;
}
//****************************************************************************************************
uint8_t renderResize(uint32_t w, uint32_t h,renderZoom newzoom)
{
  if(displayW==w && displayH==h && rgbDataBuffer) return 1;
  if(rgbDataBuffer) delete [] rgbDataBuffer;
  rgbDataBuffer=NULL;
  rgbDataBuffer=new uint8_t [w*h*4]; // 32 bits / color
  displayW=w;
  displayH=h;
  rgbConverter.reset(w,h);
  videoWindow->resize(displayW,displayH);
  printf("[RDR] Resizing to %u x %u\n",displayW,displayH);
  return 1;
}
//****************************************************************************************************
uint8_t renderRefresh(void)
{
  
  renderExpose();
  return 1;
}
//****************************************************************************************************
uint8_t renderExpose(void)
{
// TODO   if(videoWindow)
     videoWindow->repaint();
  return 1;
}
//****************************************************************************************************
uint8_t renderUpdateImage(uint8_t *ptr)
{
  rgbConverter.scale(ptr,rgbDataBuffer);
  renderExpose();
  return 1;
}
//****************************************************************************************************
uint8_t renderStartPlaying( void )
{
  return 1;
}
//****************************************************************************************************
uint8_t renderStopPlaying( void )
{
  return 1;
}
//****************************************************************************************************
#include "preview_qt4.moc"
//EOF 
