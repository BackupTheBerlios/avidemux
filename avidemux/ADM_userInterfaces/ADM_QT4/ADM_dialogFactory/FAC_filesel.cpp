/***************************************************************************
  FAC_toggle.cpp
  Handle dialog factory element : Toggle
  (C) 2006 Mean Fixounet@free.fr 
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
#include <stdlib.h>
#include <math.h>

#include <QDialog>
#include <QMessageBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);


class  ADM_Qfilesel : public QWidget
{
     Q_OBJECT
    
  signals:
        
        
   public slots:
        
  public:
        QLineEdit *edit;
        QDialogButtonBox *button;
        QLabel *text;
            
        ADM_Qfilesel(QWidget *z,const char *title,char *entry,QGridLayout *layout,int line) : QWidget(z) 
        {
          
  
          edit=new QLineEdit(entry,z);
          
          edit->show();
          
          button=new QDialogButtonBox(QDialogButtonBox::Open,Qt::Horizontal,z);
          button->show();
          
        
        
          text=new QLabel( title,z);
          text->setBuddy(edit);
          layout->addWidget(text,line,0);
          layout->addWidget(edit,line,1);
          layout->addWidget(button,line,2);
          //QObject::connect(&button, SIGNAL(accepted()), NULL, SLOT(accept())); 
        }
        ~ADM_Qfilesel() 
            {
                if(edit) delete edit;
                if(button) delete button;
                if(text) delete text;
            };
};


diaElemFileRead::diaElemFileRead(char **filename,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_FILE_READ)
{
  param=(void *)filename;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
}

diaElemFileRead::~diaElemFileRead()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemFileRead::setMe(void *dialog, void *opaque,uint32_t line)
{
 QGridLayout *layout=(QGridLayout*) opaque;
  
  ADM_Qfilesel *fs=new ADM_Qfilesel((QWidget *)dialog,paramTitle,*(char **)param,layout, line);
  myWidget=(void *)fs; 
}
void diaElemFileRead::getMe(void)
{
  
  char **n=(char **)param;
  if(*n) ADM_dealloc(*n);
  
  ADM_Qfilesel *fs=(ADM_Qfilesel *)myWidget;
  QString s=(fs->edit)->text();
  *n=ADM_strdup( s.toLatin1() );
}

//EOF

