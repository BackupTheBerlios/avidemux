
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
static ColYuvRgb rgbConverter(640,480,1);
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
  hostFrame->resize(displayW,displayH);
  videoWindow->resize(displayW,displayH);
  printf("[RDR] Resizing to %u x %u\n",displayW,displayH);
  return 1;
}
/**
    \fn renderRefresh(void)
    \brief Force a redraw of the screen
*/
uint8_t renderRefresh(void)
{
  
        if(accelRender)
        {
            if(lastImage)
                accelRender->display(lastImage, displayW, displayH);
        }
        else
        {
                renderExpose();
        }
  return 1;
}
//****************************************************************************************************
uint8_t renderExpose(void)
{
// TODO   if(videoWindow)
  if(!accelRender)
     videoWindow->repaint();
  return 1;
}
/**
      \fn renderUpdateImage(uint8_t *ptr)
      \brief Update display with the content of the pointer given as arg
*/
uint8_t renderUpdateImage(uint8_t *ptr)
{
  if(!accelRender) // Only needed for unaccelerated display
  {
      rgbConverter.scale(ptr,rgbDataBuffer);
      renderExpose();
  }else
  {
      accelRender->display(ptr,displayW,displayH);
      lastImage=ptr;
  }
  return 1;
}
/**
    \fn renderStartPlaying( void )
    \brief Start playing, create an alternate renderer (preferably hw accelerated such as Xv or SDL)
*/
  uint8_t renderStartPlaying( void )
{
char *displ;
unsigned int renderI;
ADM_RENDER_TYPE render;
        ADM_assert(!accelRender);
        // First check if local
        // We do it in a very wrong way : If DISPLAY!=:0.0 we assume remote display
        // in that case we do not even try to use accel
        
        // Win32 does not have display
#ifndef CYG_MANGLING	
        displ=getenv("DISPLAY");
        if(!displ)
        {
                return 0;
        }
        if(strcmp(displ,":0") && strcmp(displ,":0.0"))
        {
                printf("Looks like remote display, no Xv :%s\n",displ);
                return 1;
        }
#endif	
 
        if(prefs->get(DEVICE_VIDEODEVICE,&renderI)!=RC_OK)
        {       
                render=RENDER_GTK;
        }else
        {
                render=(ADM_RENDER_TYPE)renderI;
        }
        GUI_Info xinfo;
#ifndef CYG_MANGLING
        const QX11Info &info=videoWindow->x11Info();
        xinfo.display=info.display();
        xinfo.window=videoWindow->winId();
         
#endif 
        switch(render)
        {
        
#if defined(USE_XV)
              case RENDER_XV:
                accelRender=new XvAccelRender();
                if(!accelRender->init(&xinfo,displayW,displayH))
                {
                        delete accelRender;
                        accelRender=NULL;
                        printf("Xv init failed\n");
                }
                else
                {
                        printf("Xv init ok\n");
                }
                break;
#endif
#if defined(USE_SDL)
              case RENDER_SDL:
                printf("Trying SDL\n");
                accelRender=new sdlAccelRender();
                if(!accelRender->init(&xinfo,displayW,displayH))
                {
                        delete accelRender;
                        accelRender=NULL;
                        printf("sdl init failed\n");
                }
                else
                {
                        printf("SDL init ok\n");
                }
                break;
#endif
                default:break;
        }
        if(!accelRender)
        {
                rgbConverter.reset(displayW,displayH);
                printf("No accel used for rendering\n");
        }
	else printf("Using accelerated rendering\n");
	return 1;
}

/**
    \fn renderStopPlaying(void)
    \brief Switch back to regular slow display, destroy accelerated renderer if exists
*/
uint8_t renderStopPlaying( void )
{
      if(accelRender)
      {
            accelRender->end();
              delete accelRender;
      }
      accelRender=NULL;
      lastImage=NULL;
      return 1;
}



//****************************************************************************************************
//EOF 
