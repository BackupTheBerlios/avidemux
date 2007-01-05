/***************************************************************************
                         
  Load external filter through dynamic loading (.so /.dll)
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
#include <dlfcn.h>

#include <ADM_assert.h>

#include "fourcc.h"
#include "avio.hxx"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_editor/ADM_edit.hxx"
#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_video/ADM_vidPartial.h"
#include "avi_vars.h"

#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_PREVIEW
#include "ADM_osSupport/ADM_debug.h"
#include "ADM_osSupport/ADM_quota.h"

typedef char *(ADM_getStringT)(void);

static void tryLoading(void *handle);
static void *loadSym(void *handle,const char *sym);
static uint32_t dynTag=0xF0000000;
/**
    \fn  loadSym(void *handle,const char *sym)
    \brief Get a pointer to the function name given as parameter

*/
void *loadSym(void *handle,const char *sym)
{
  void *f;
   
  f=dlsym(handle,sym);
  if(!f) 
  {
    printf("\t Symbold %s not found\n",sym); 
  }
  return f;
  
}

/**
    \fn  tryLoadin(void *handle)
    \brief try do use the dynlib pointer by handle

*/
void tryLoading(void *handle)
{
  ADM_assert(handle);
  
  void *f;
  int success=1;
  ADM_createT *createP;
  ADM_create_from_scriptT *createFromScriptP;
  ADM_getStringT *nameP;
  ADM_getStringT *descP;
  
#define LOADSYM(x,y,z) if(!(f=loadSym(handle,x))) { success=0;printf("%s",dlerror());}   else {y=(z *)f;}
  
  
    LOADSYM("FILTER_create",createP,ADM_createT);
    LOADSYM("FILTER_create_fromscript",createFromScriptP,ADM_create_from_scriptT);
    LOADSYM("FILTER_getName",nameP,ADM_getStringT);
    LOADSYM("FILTER_getDesc",descP,ADM_getStringT);
    
    if(!success)
    {
      dlclose(handle);
      return;
    }
    
    char *name,*desc;
    
    name=nameP();
    desc=descP();
    
    ADM_assert(name);
    ADM_assert(desc);
    
    printf("Loaded filter %s\n",name);
  
   registerFilterEx(name,(VF_FILTERS)dynTag,1,createP,name,createFromScriptP,desc);

}

/**
    \fn filterDynLoad(char *path)
    \brief load dynamically filters by scanning the path directory

*/
#define MAX_EXTERNAL_FILTER 50
uint8_t filterDynLoad(const char *path)
{
  char *files[MAX_EXTERNAL_FILTER];
  uint32_t nbFile;
  printf("** Registering dynamic filters (%s) **\n",path);
  memset(files,0,sizeof(char *)*MAX_EXTERNAL_FILTER);
  if(!buildDirectoryContent(&nbFile,path, files,MAX_EXTERNAL_FILTER,".so"))
  {
      GUI_Error_HIG("External Filter","Loading external filters failed.");
      return 0;
  }
  printf("Found %u candidated\n",nbFile);
  // Try to dyload them
  for(int i=0;i<nbFile;i++)
  {
    void *h=dlopen(files[i],RTLD_NOW);
    if(!h)
    {
        printf("Dlopened failed for %s er:%s\n",files[i],dlerror());
        continue;
    }
    tryLoading(h);
    
  }
  // Cleanup
  for(int i=0;i<nbFile;i++)
  {
    ADM_dealloc(files[i]); 
  }
  printf("** Done registering dynamic filters**\n");
  return 1;
  
}
