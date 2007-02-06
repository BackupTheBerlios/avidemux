
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
/* Probably on unix/X11 ..*/
#ifndef CYG_MANGLING
#include <QX11Info>
#endif
#include "fourcc.h"
#include "avio.hxx"

#include "ADM_colorspace/ADM_rgb.h"
#include "GUI_render.h"
#include "GUI_accelRender.h"
#if defined(USE_XV)
#include "GUI_xvRender.h"
#endif
#if defined (USE_SDL)
#include "GUI_sdlRender.h"
#endif
#include "prefs.h"
#include <ADM_assert.h>
    
void UI_QT4VideoWidget(QFrame *host);
static QFrame *hostFrame=NULL;
static AccelRender *accelRender=NULL;
static uint8_t *lastImage=NULL;

 ColYuvRgb rgbConverter(640,480,1);
 ColYuvRgb rgbConverter2(640,480,1);

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
uint8_t GUI_PreviewStillAlive( void )
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
        /**
            \fn paintEvent( QPaintEvent *ev))
            \brief Repaint our "video" widget, ignore when accelRender is on
        */
        void paintEvent(QPaintEvent *ev)
        {
          if(!displayW || !displayH || !rgbDataBuffer || accelRender)
          {
            printf("Nothing to draw\n");
            return ;
          }
          if(accelRender) 
          {
            if(lastImage)
            {
              accelRender->display(lastImage,displayW,displayH);
            }
          }else
          {
            QImage image(rgbDataBuffer,displayW,displayH,QImage::Format_RGB32);
              QPainter painter(this);
              painter.drawImage(QPoint(0,0),image);
              painter.end();
          }
        }
};
ADM_Qvideo *videoWindow=NULL;
//****************************************************************************************************
void UI_QT4VideoWidget(QFrame *host)
{
   videoWindow=new ADM_Qvideo(host);
   hostFrame=host;
   hostFrame->resize(320,200);
   videoWindow->resize(320,200);
   videoWindow->show();
   
}
//*************************
/**
    \brief return pointer to the drawing widget that displays video
*/
void *UI_getDrawWidget(void)
{
  return (void *) videoWindow;
}
/**
    \brief Display to widget in RGB32
*/
void UI_rgbDraw(void *widg,uint32_t w, uint32_t h,uint8_t *ptr)
{
      memcpy(rgbDataBuffer,ptr,w*h*4);
      videoWindow->repaint();
    
}
/**
      \brief Resize the window
*/
void  UI_updateDrawWindowSize(void *win,uint32_t w,uint32_t h)
{
  if(displayW==w && displayH==h && rgbDataBuffer) return ;
  if(rgbDataBuffer) delete [] rgbDataBuffer;
  rgbDataBuffer=NULL;
  rgbDataBuffer=new uint8_t [w*h*4]; // 32 bits / color
  displayW=w;
  displayH=h;
  hostFrame->resize(displayW,displayH);
  videoWindow->resize(displayW,displayH);
  printf("[RDR] Resizing to %u x %u\n",displayW,displayH);
  return ;
}
/**
      \brief Retrieve info from window, needed for accel layer
*/
void UI_getWindowInfo(void *draw, GUI_WindowInfo *xinfo)
{
#ifndef CYG_MANGLING
        const QX11Info &info=videoWindow->x11Info();
        xinfo->display=info.display();
        xinfo->window=videoWindow->winId();
         
#endif 

}


//****************************************************************************************************
//EOF 
