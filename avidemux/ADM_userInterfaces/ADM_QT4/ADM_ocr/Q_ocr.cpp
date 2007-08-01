/***************************************************************************
                          Q_SRT.cpp  -  description
                             -------------------

    Handle the QT specific part of the fontsize & position dialog box
    copyright            : (C) 2002/2007 by mean
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

#include <config.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

#include "ui_ocr.h"

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_image.h"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_colorspace/ADM_rgb.h"

#include "ADM_video/ADM_vobsubinfo.h"
#include "ADM_video/ADM_vidVobSub.h"

#include "ADM_assert.h"
#include "ADM_ocr/ADM_ocr.h"
#include "ADM_ocr/ADM_ocrInternal.h"
#include "ui_ocr.h"


//
//	Video is in YV12 Colorspace
//
//
class Ui_ocrWindow : public QDialog
 {
     Q_OBJECT
 protected :
 public:
     Ui_ocrWindow(void);
     ~Ui_ocrWindow();
     Ui_DialogOcr ui;
     
     void  gather(void);
 public slots:
 private slots:
 private:
     
 };
  Ui_ocrWindow::Ui_ocrWindow(void)
  {
        ui.setupUi(this);
      
        
  }
 
Ui_ocrWindow::~Ui_ocrWindow()
{
}

uint8_t ADM_ocrUpdateNbLines(void *ui,uint32_t cur,uint32_t total)
{
	return 1;
}
uint8_t ADM_ocrUpdateNbGlyphs(void *ui,uint32_t nbGlyphs)
{
	return 1;
}
uint8_t ADM_ocrUpdateTextAndTime(void *ui,char *decodedString,char *timeCode)
{
	return 1;
}
uint8_t ADM_ocrDrawFull(void *d,uint8_t *data)
{
	return 1;	
}
uint8_t ADM_ocrSetRedrawSize(uint32_t w,uint32_t h)
{
	return 1;
}


uint8_t ADM_ocrUiEnd(void *d)
{
	   Ui_ocrWindow *dialog=( Ui_ocrWindow *)d;
	   ADM_assert(dialog);
	   delete dialog;
	   return 1;	
}
void 	*ADM_ocrUiSetup(void)
{
	
	   Ui_ocrWindow *dialog=new Ui_ocrWindow;
	   return dialog;
}

ReplyType glyphToText(admGlyph *glyph,admGlyph *head,char *decodedString)
{
	return ReplyOk;
}

//____________________________________
// EOF


