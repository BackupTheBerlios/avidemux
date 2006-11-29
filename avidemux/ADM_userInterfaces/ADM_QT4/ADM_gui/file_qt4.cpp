
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
#include <dirent.h>
#include "default.h"
#include <QtCore/QVariant>
#include <QtGui/qfiledialog.h>

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "prefs.h"
#include "ADM_assert.h"
    
extern QWidget *QuiMainWindows;
static void GUI_FileSelSelect(const char *label, char **name, uint32_t access) ;
//****************************************************************************************************
void GUI_FileSelRead(const char *label,SELFILE_CB cb) 
{
  char *name;
  GUI_FileSelRead(label,&name);
  if(name)
  {
    cb(name); 
    ADM_dealloc(name);
  }
}
//****************************************************************************************************
void GUI_FileSelWrite(const char *label,SELFILE_CB cb) 
{
  char *name;
  GUI_FileSelWrite(label,&name);
  if(name)
  {
    cb(name); 
    ADM_dealloc(name);
  }

}
//****************************************************************************************************
void GUI_FileSelRead(const char *label, char * * name)
{
 GUI_FileSelSelect(label, name, 0) ;
}
//****************************************************************************************************
void GUI_FileSelWrite(const char *label, char * * name)
{
 GUI_FileSelSelect(label, name, 1) ;

}
/**
  \fn GUI_FileSelWrite (const char *label, char **name, uint32_t access) 
  \brief Ask for a file for reading (access=0) or writing (access=1)

*/
void GUI_FileSelSelect(const char *label, char **name, uint32_t access) 
{
  char *dir=NULL,*tmpname=NULL;
  *name=NULL;
  char *str=NULL;
  options pref_entry=LASTDIR_READ;
  if(access) pref_entry=LASTDIR_WRITE;
  
  if(prefs->get(pref_entry,(ADM_filename **)&tmpname))
  {
     DIR *dir;
        str=PathCanonize(tmpname);
        PathStripName(str);
        /* LASTDIR may have gone; then do nothing and use current dir instead (implied) */
        if( (dir=opendir(str)) )
        {
                closedir(dir);
        } else 
        {
            delete [] str;
            str=NULL;
        }
  }
  QString fileName;
    if(access)  fileName=QFileDialog::getSaveFileName(QuiMainWindows,label,str);
        else    fileName=QFileDialog::getOpenFileName(QuiMainWindows,label,str);
                
  if(!fileName.isNull() )
  {
    const char *s=fileName.toLatin1(); // Fixme utf8 ?
    *name=ADM_strdup(s);
    prefs->set(pref_entry,(ADM_filename *)*name);
  }
  if(str) delete [] str;
  
}
//****************************************************************************************************
//EOF
