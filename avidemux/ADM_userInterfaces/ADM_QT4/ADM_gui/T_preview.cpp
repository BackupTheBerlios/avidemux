
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
#include <QtGui/QSlider>
#include <QMessageBox>
#include <QPainter>
#include <QImage>
/* Probably on unix/X11 ..*/
#if !defined(ADM_WIN32) && !defined(__APPLE__)
#include <QX11Info>
#endif
#include "avio.hxx"

#include "ADM_colorspace/ADM_rgb.h"
#include "GUI_render.h"
#include "GUI_accelRender.h"
#include "prefs.h"
#include "ADM_assert.h"
#include "ui_gui2.h"
    
void UI_QT4VideoWidget(QFrame *host);
static QFrame *hostFrame=NULL;
static AccelRender *accelRender=NULL;
static uint8_t *lastImage=NULL;
extern QWidget *QuiMainWindows;
 ColYuvRgb rgbConverter(640,480,1);
extern void UI_purge( void );

void DIA_previewInit(uint32_t width, uint32_t height) {}
uint8_t DIA_previewUpdate(uint8_t *data) {return 1;}
void DIA_previewEnd(void) {}
uint8_t DIA_previewStillAlive(void) {return 1;}
uint8_t	DIA_filterPreview(const char *captionText, AVDMGenericVideoStream *videoStream, uint32_t frame) {}

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
            return ;

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
   videoWindow->resize(hostFrame->size());
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
	if(displayW == w && displayH == h && rgbDataBuffer)
		return;

	if(rgbDataBuffer)
		delete[] rgbDataBuffer;

	rgbDataBuffer = new uint8_t[w * h * 4]; // 32 bits / color
	displayW = w;
	displayH = h;

	hostFrame->setMinimumSize(displayW, displayH);
	videoWindow->setMinimumSize(displayW, displayH);	

	hostFrame->resize(displayW, displayH);
	videoWindow->resize(displayW, displayH);

	UI_purge();

	// Resize only works every second time so both calls are necessary - don't know why ???
	QuiMainWindows->resize(1, 1);
	UI_purge();	
	QuiMainWindows->resize(1, 1);
	UI_purge();

	printf("[RDR] Resizing to %u x %u\n", displayW, displayH);
}
/**
      \brief Retrieve info from window, needed for accel layer
*/
void UI_getWindowInfo(void *draw, GUI_WindowInfo *xinfo)
{
#if defined(ADM_WIN32)
	xinfo->display=videoWindow->winId();
#elif !defined(__APPLE__)
    const QX11Info &info=videoWindow->x11Info();
    xinfo->display=info.display();
    xinfo->window=videoWindow->winId();
#endif 
}


//****************************************************************************************************
//EOF 
