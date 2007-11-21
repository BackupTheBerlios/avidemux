
// C++ Interface: 
//
// Description: 
//
//
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"
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
    const char *s=fileName.toUtf8().constData();
    *name=ADM_strdup(s);
    prefs->set(pref_entry,(ADM_filename *)*name);
  }
  if(str) delete [] str;
  
}
/**
      \fn FileSel_SelectWrite
      \brief select file, write mode
      @param title window title
      @param target where to store result
      @param max Max buffer size in bytes
      @param source Original value
      @return 1 on success, 0 on failure

*/
uint8_t FileSel_SelectWrite(const char *title,char *target,uint32_t max, const char *source)
{
  char *dir=NULL,*tmpname=NULL;
   QString fileName;
 
  fileName=QFileDialog::getSaveFileName(QuiMainWindows,title,source);
                
  if(!fileName.isNull() )
  {
    const char *s=fileName.toUtf8().constData();
    strncpy(target,s,max);
  }
  
  
}
/**
      \fn FileSel_SelectRead
      \brief select file, read mode
      @param title window title
      @param target where to store result
      @param max Max buffer size in bytes
      @param source Original value
      @return 1 on success, 0 on failure

*/
uint8_t FileSel_SelectRead(const char *title,char *target,uint32_t max, const char *source)
{
  char *dir=NULL,*tmpname=NULL;
   QString fileName;
 
  fileName=QFileDialog::getOpenFileName(QuiMainWindows,title,source);
                
  if(!fileName.isNull() )
  {
    const char *s=fileName.toUtf8().constData();
    strncpy(target,s,max);

	return 1;
  }
  
  return 0;
}
/**
      \fn FileSel_SelectDir
      \brief select directory
      @param title window title
      @param target where to store result
      @param max Max buffer size in bytes
      @param source Original value
      @return 1 on success, 0 on failure

*/

uint8_t FileSel_SelectDir(const char *title,char *target,uint32_t max, const char *source)
{
   char *dir=NULL,*tmpname=NULL;
  
   QString fileName;
 
  fileName=QFileDialog::getExistingDirectory(QuiMainWindows,title,source);
                
  if(!fileName.isNull() )
  {
    const char *s=fileName.toUtf8().constData();
    strncpy(target,s,max);
	return 1;
  }

  return 0;
}



//****************************************************************************************************
//EOF
