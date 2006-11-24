
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

#include <QDialog>
#include <QMessageBox>


#include "fourcc.h"
#include "avio.hxx"

#include "ADM_colorspace/ADM_rgb.h"
#include "GUI_render.h"
#include <ADM_assert.h>
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

*/
static uint8_t *rgbDataBuffer=NULL;
static uint32_t displayW=0,displayH=0;
static ColYuvRgb rgbConverter(640,480);
static uint8_t GUI_ConvertRGB(uint8_t * in, uint8_t * out, uint32_t w, uint32_t h);
//****************************************************************************************************
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
  return 1;
}
//****************************************************************************************************
uint8_t renderRefresh(void)
{
  return 1;
}
//****************************************************************************************************
uint8_t renderExpose(void)
{
  return 1;
}
//****************************************************************************************************
uint8_t renderUpdateImage(uint8_t *ptr)
{
  rgbConverter.scale(ptr,rgbDataBuffer);
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
//EOF 
