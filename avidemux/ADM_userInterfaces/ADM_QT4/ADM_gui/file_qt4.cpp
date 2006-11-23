
// C++ Interface: 
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include "default.h"
#include <QtCore/QVariant>
#include <QtGui/qfiledialog.h>

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_assert.h"
    
extern QWidget *QuiMainWindows;
//****************************************************************************************************
void GUI_FileSelRead(const char *label,SELFILE_CB cb) 
{
  QString fileName = QFileDialog::getOpenFileName(QuiMainWindows);
  if(!fileName.isNull() )
  {
    char *s=ADM_strdup(fileName.toLatin1()); // Fixme utf8 ?
    
    printf("File selected :%s\n",s); 
    cb(s);
    ADM_dealloc(s);
  }
}
//****************************************************************************************************
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) 
{
  QString fileName = QFileDialog::getSaveFileName(QuiMainWindows);
  if(!fileName.isNull() )
  {
    char *s=ADM_strdup(fileName.toLatin1()); // Fixme utf8 ?
    printf("File selected :%s\n",s); 
    cb(s);
    ADM_dealloc(s);
  }

}
//****************************************************************************************************
void GUI_FileSelRead(const char *label, char * * name)
{
  *name=NULL;
  QString fileName = QFileDialog::getOpenFileName(QuiMainWindows);
  if(!fileName.isNull() )
  {
    const char *s=fileName.toLatin1(); // Fixme utf8 ?
    *name=ADM_strdup(s);
  }

}
//****************************************************************************************************
void GUI_FileSelWrite(const char *label, char * * name)
{
  *name=NULL;
  QString fileName = QFileDialog::getSaveFileName(QuiMainWindows);
  if(!fileName.isNull() )
  {
    const char *s=fileName.toLatin1(); // Fixme utf8 ?
    *name=ADM_strdup(s);
  }

}
//****************************************************************************************************
//EOF
