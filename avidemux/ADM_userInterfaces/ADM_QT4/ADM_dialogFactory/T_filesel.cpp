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
#include "ADM_toolkit/filesel.h"
extern const char *shortkey(const char *);
void GUI_FileSelRead(const char *label, char * * name);
#define MAX_SEL 2040

typedef enum ADM_fileMode
{
    ADM_FILEMODE_DIR,
    ADM_FILEMODE_READ,
    ADM_FILEMODE_WRITE
};

class  ADM_Qfilesel : public QWidget
{
     Q_OBJECT
    
  signals:
        
        
   public slots:
        void buttonPressed(QAbstractButton *s)
        { 
          uint8_t r=0;
          char buffer[MAX_SEL+1],*txt;
          txt="";
          switch(fileMode)
          {
            case ADM_FILEMODE_READ:
                r=FileSel_SelectRead(_("Select File to Read"),buffer,MAX_SEL,txt);
                break;
            case ADM_FILEMODE_WRITE:
                r=FileSel_SelectWrite(_("Select File to Write"),buffer,MAX_SEL,txt);
                break;

            case ADM_FILEMODE_DIR:
                r=FileSel_SelectDir(_("Select Directory"),buffer,MAX_SEL,txt);
                break;
            default: ADM_assert(0);
          }
          if(r)
          {
            edit->setText(buffer);
          }
        }
  public:
        QLineEdit *edit;
        QDialogButtonBox *button;
        QLabel *text;
        ADM_fileMode fileMode;
            
        ADM_Qfilesel(QWidget *z,const char *title,char *entry,QGridLayout *layout,int line, ADM_fileMode mode) : QWidget(z) 
        {
          
          fileMode=mode;
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
          connect( button,SIGNAL(clicked(QAbstractButton  *)),this,SLOT(buttonPressed(QAbstractButton  *)));
        }
        ~ADM_Qfilesel() 
            {
#if 1 //Memleak or autoclean ?
                if(edit) delete edit;
                if(button) delete button;
                if(text) delete text;
#endif
            };
};


diaElemFile::diaElemFile(uint32_t writemode,char **filename,const char *toggleTitle,const char *tip)
  : diaElem(ELEM_FILE_READ)
{
  param=(void *)filename;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
  _write=writemode;
}

diaElemFile::~diaElemFile()
{
  if(paramTitle)
    delete paramTitle;
}
void diaElemFile::setMe(void *dialog, void *opaque,uint32_t line)
{
 QGridLayout *layout=(QGridLayout*) opaque;
 ADM_Qfilesel *fs;
  if(_write)
      fs=new ADM_Qfilesel((QWidget *)dialog,paramTitle,*(char **)param,layout, line,ADM_FILEMODE_WRITE);
  else
      fs=new ADM_Qfilesel((QWidget *)dialog,paramTitle,*(char **)param,layout, line,ADM_FILEMODE_READ);
  myWidget=(void *)fs; 
}
void diaElemFile::getMe(void)
{
  
  char **n=(char **)param;
  if(*n) ADM_dealloc(*n);
  
  ADM_Qfilesel *fs=(ADM_Qfilesel *)myWidget;
  QString s=(fs->edit)->text();
  *n=ADM_strdup( s.toLatin1() );
}
void diaElemFile::enable(uint32_t onoff)
{
  ADM_Qfilesel *fs=(ADM_Qfilesel *)myWidget;
  fs->setEnabled(onoff);
}
//****************************
diaElemDirSelect::diaElemDirSelect(char **filename,const char *toggleTitle,const char *tip)  : diaElem(ELEM_DIR_SELECT) 
{
  param=(void *)filename;
  paramTitle=shortkey(toggleTitle);
  this->tip=tip;
}
diaElemDirSelect::~diaElemDirSelect() 
{
if(paramTitle)
    delete paramTitle;
}
void diaElemDirSelect::setMe(void *dialog, void *opaque,uint32_t line)
{
 QGridLayout *layout=(QGridLayout*) opaque;
  
  ADM_Qfilesel *fs=new ADM_Qfilesel((QWidget *)dialog,paramTitle,*(char **)param,layout, line,ADM_FILEMODE_DIR);
  myWidget=(void *)fs; 
}
void diaElemDirSelect::getMe(void) 
{
 char **n=(char **)param;
  if(*n) ADM_dealloc(*n);
  *n=NULL;
  ADM_Qfilesel *fs=(ADM_Qfilesel *)myWidget;
  QString s=(fs->edit)->text();
  *n=ADM_strdup( s.toLatin1() );
}
void diaElemDirSelect::enable(uint32_t onoff)
{
  
}
  
void diaElemDirSelect::changeFile(void) 
{
}
//EOF

