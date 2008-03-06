/***************************************************************************
                    
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
    
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include <unistd.h>

#ifdef __MINGW32__
#include <glib.h>
#endif

#include "ADM_default.h"


#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit.hxx"

//*****************************
static char basedir[1024]={0};
static char jobdir[1024]={0};
static char customdir[1024]={0};
int baseDirDone=0;
int jobDirDone=0;
int customDirDone=0;
#ifdef __WIN32
const char *ADM_DIR_NAME="\\avidemux";
#else
const char *ADM_DIR_NAME="/.avidemux";
#endif
/*

*/
/*
      Get the  directory where jobs are stored
******************************************************/

char *ADM_getCustomDir(void)
{
  if(customDirDone) return customdir;

  char *rootDir;
  rootDir=ADM_getBaseDir();
  strncpy(customdir,rootDir,1023);
#if defined(__WIN32)
  strcat(customdir,"\\custom"); 
#else
  strcat(customdir,"/custom");
#endif
  if(!ADM_mkdir(customdir))
  {
    GUI_Error_HIG(QT_TR_NOOP("Oops"),QT_TR_NOOP("can't create custom directory (%s)."),customdir);
                return NULL;
  }
  customDirDone=1;
  return customdir;
}
/*
      Get the  directory where jobs are stored
******************************************************/

char *ADM_getJobDir(void)
{
  if(jobDirDone) return jobdir;

  char *rootDir;
  rootDir=ADM_getBaseDir();
  strncpy(jobdir,rootDir,1023);
#if defined(__WIN32)
  strcat(jobdir,"\\jobs"); 
#else
  strcat(jobdir,"/jobs");
#endif
  if(!ADM_mkdir(jobdir))
  {
    GUI_Error_HIG(QT_TR_NOOP("Oops"),QT_TR_NOOP("can't create job directory (%s)."),jobdir);
                return NULL;
  }
  jobDirDone=1;
  return jobdir;
}
/*
      Get the root directory for .avidemux stuff
******************************************************/
char *ADM_getBaseDir(void)
{
char *dirname=NULL;
DIR *dir=NULL;
char *home;
//
        if(baseDirDone) return basedir;
// Get the base directory
#if defined(__WIN32)
        if( ! (home=getenv("USERPROFILE")) )
        {
          GUI_Error_HIG(QT_TR_NOOP("Oops"),QT_TR_NOOP("can't determine $USERPROFILE."));
                    home="c:\\";
        }

#else
        if( ! (home=getenv("HOME")) )
        {
          GUI_Error_HIG(QT_TR_NOOP("Oops"),QT_TR_NOOP("can't determine $HOME."));
                return NULL;
        }
#endif

 // Try to open the .avidemux directory
        dirname=new char[strlen(home)+strlen(ADM_DIR_NAME)+2];
        strcpy(dirname,home);
        strcat(dirname,ADM_DIR_NAME);
        if(!ADM_mkdir(dirname))
        {
          GUI_Error_HIG(QT_TR_NOOP("Oops"),QT_TR_NOOP("Cannot create the .avidemux directory"), NULL);
                        delete [] dirname;
                        return NULL;
        }
        delete [] dirname;

        // Now built the filename
        strncpy(basedir,home,1023);
        strncat(basedir,ADM_DIR_NAME,1023-strlen(basedir));
        baseDirDone=1;
        printf("Using %s as base directory for prefs/jobs/...\n",basedir);
        return basedir;
}
