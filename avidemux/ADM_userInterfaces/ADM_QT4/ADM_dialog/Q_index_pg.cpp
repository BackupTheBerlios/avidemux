/***************************************************************************
                          DIA_dmx.cpp  -  description
                             -------------------     
Indexer progress dialog                             
    
    copyright            : (C) 2005 by mean
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
#include <math.h>
#include <assert.h>

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QProgressBar>


#include "default.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_encoder/adm_encoder.h"
#include "DIA_idx_pg.h"
#include "ADM_assert.h"

extern void UI_purge( void );

class Ui_indexingDialog : public QDialog
 {
     Q_OBJECT
 protected : 
    int lock;
    
 public:
   int abted;
   
     Ui_indexingDialog(const char *name);
     ~Ui_indexingDialog();
    void setTime(const char *f);
    void setImage(const char *f);
    void setETA(const char *f);
    void setPercent(float f);
 public slots:
 private slots:

 private:
     
 };
static Ui_indexingDialog *dialog=NULL; 
#if 0
static gint on_destroy_abort(GtkObject * object, gpointer user_data)
{
DIA_progressIndexing *pf;

        UNUSED_ARG(object);
        UNUSED_ARG(user_data);

        pf=(Ui_indexingDialog *)user_data;
        if(!GUI_Confirmation_HIG(_("Continue indexing"),_("Abort Requested"),_("Do you want to abort indexing ?")))
        {
         //       pf->abortRequest();
                abted=1;
        }

        return TRUE;

};
#endif

DIA_progressIndexing::DIA_progressIndexing(const char *name)
{
        dialog=new Ui_indexingDialog(name);
        clock.reset();
        aborted=0;
	_nextUpdate=0;

}
DIA_progressIndexing::~DIA_progressIndexing()
{
        ADM_assert(dialog);
        delete dialog;
        dialog=NULL;
}
uint8_t       DIA_progressIndexing::isAborted(void) 
{
        ADM_assert(dialog);
        return dialog->abted;
}
uint8_t DIA_progressIndexing::abortRequest(void)
{
        ADM_assert(dialog);
        aborted=1;
        dialog->abted=1;
        return 1;
}
uint8_t       DIA_progressIndexing::update(uint32_t done,uint32_t total, uint32_t nbImage, uint32_t hh, uint32_t mm, uint32_t ss)
{
        uint32_t tim;
	#define  GUI_UPDATE_RATE 1000

	tim=clock.getElapsedMS();
	if(tim>_nextUpdate)
	{
        char string[256];
        double f;
        	uint32_t   tom,zhh,zmm,zss;

		_nextUpdate=tim+GUI_UPDATE_RATE;
        sprintf(string,"%02d:%02d:%02d",hh,mm,ss);
        dialog->setTime(string);
        

        sprintf(string,"%0lu",nbImage);
        dialog->setImage(string);

        f=done;
        f/=total;

        dialog->setPercent(f);

        /* compute ETL */
       // Do a simple relation between % and time
        // Elapsed time =total time*percent
        if(f<0.01) return 1;
        f=tim/f;
        // Tom is total time
        tom=(uint32_t)floor(f);
        if(tim>tom) return 1;
        tom=tom-tim;
        ms2time(tom,&zhh,&zmm,&zss);
        sprintf(string,"%02d:%02d:%02d",zhh,zmm,zss);
        dialog->setETA(string);
        UI_purge();
        }
        return 1;
}
//****************************** CLASS ***********************
Ui_indexingDialog::Ui_indexingDialog(const char *name)
{
      abted=0;
}
Ui_indexingDialog::~Ui_indexingDialog()
{
  
}
void Ui_indexingDialog::setTime(const char *f)
{
    printf("Time:%s\n",f);
}
void Ui_indexingDialog::setETA(const char *f)
{
    printf("Eta:%s\n",f);
}
void Ui_indexingDialog::setPercent(float f)
{
    printf("Eta:%f\n",f);
}



// EOF
