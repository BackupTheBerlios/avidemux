/***************************************************************************
  Try to display interesting crash dump

    copyright            : (C) 2007 by mean
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
#include <stdarg.h>
#include <string.h>
#include <default.h>
#include <unistd.h>

#include <math.h>
#include <ADM_assert.h>

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
#include "ADM_editor/ADM_edit.hxx"
extern ADM_Composer *video_body;

#define CRASH_FILE "crash.js"

static void saveCrashProject(void);
extern char *ADM_getBaseDir(void);
extern void A_parseECMAScript(const char *name);
#ifdef CYG_MANGLING
void installSigHandler()
{

}
void ADM_backTrack(int lineno,const char *file)
{
 char bfr[1024];
  saveCrashProject();
 snprintf(bfr,1024,"Assert Failed at file %s, line %d\n",file,lineno);
 GUI_Error_HIG("Fatal Error",bfr);
 assert(0);
}
#else
#include <signal.h>
#include <execinfo.h>
void sig_segfault_handler(int signo);
void installSigHandler()
{
    signal(11, sig_segfault_handler); // show stacktrace on default
}
extern void ADMImage_stat( void );

static int lenCount(uint8_t *start,uint8_t *end,int *d)
{
  int val=0;
  int digit=0;
  *d=0;
  while(*start>='0' && *start<='9' && start<end) 
  {
    val=val*10+*start-'0'; 
    start++;
    digit++;
  }
  *d=digit;
  return val;
}

static int decodeOne(uint8_t *start, uint8_t *end,int *cons)
{
  *cons=0;
  int len,digit;
  uint8_t *org=start;
  if(start+2>=end) return 0;
  switch(*start )
  {
    case 'Z':
    case 'P':  
      
        len=lenCount(start+1,end,&digit);
        start+=1+digit;
        for(int z=0;z<len;z++) printf("%c",*start++);
        break;
  }
  return (int)((uint64_t)start-(uint64_t)org);
}
static int demangle(int i,  uint8_t *string)
{
 // Search 1st (
  if(!string) return 0;
  int len=strlen((char *)string);
  if(!len) return 0;
   
  uint8_t *end=string+len;
  uint8_t *start=string;
  
  while(*start!='(' && start+3<end) start++;
  if(*start!='(') return 0;
  start++;
  
  //  _qt(_Z9crashTestP9JSContextP8JSObjectjPlS3_+0) [0x4acf80]
  
  if(*start!='_' || start[1]!='Z')
  {
    return 0;
  }
  // Seems good !
  start++;
  start++;  
  int digit;
  // Function name..
  int l=lenCount(start,end,&digit);
  printf("\t<");
  for(int i=0;i<l;i++)
  {
    printf("%c",start[digit+i]); 
  }
  printf(">(");
  start+=digit+l;
  
  // Parama
  int first=0;
  while(start+2<end && *start=='P')
  {
    if(!first)  first=1;
    else
        printf(",");
    
      
    start++;
    l=lenCount(start,end,&digit);
    for(int i=0;i<l;i++)
    {
      printf("%c",start[digit+i]); 
    }
    start+=digit+l;
  }
  printf(")\n");
  return 1;
}

/**
      \fn sig_segfault_handler
      \brief our segfault handler

*/
void sig_segfault_handler(int signo)
{
     
     static int running=0;
      if(running) 
      {
        signo=0;
        exit(1);
      }
      running=0; 
      ADM_backTrack(0,"");
}
void ADM_backTrack(int lineno,const char *file)
{
     void *stack[20];
     char **functions;
     int count, i;
      
     saveCrashProject();
      printf("\n*********** BACKTRACK **************\n");
      count = backtrace(stack, 20);
      functions = backtrace_symbols(stack, count);
      
         for (i=0; i < count; i++) 
         {
            printf("Frame %2d: %s \n", i, functions[i]);
            demangle(i,(uint8_t *)functions[i]);
            
         }
      printf("*********** BACKTRACK **************\n");
     // Now use dialogFactory
      char bfr[30];
      snprintf(bfr,30,"Line:%u",lineno);
      diaElemReadOnlyText *txt[count+1];
      txt[0]=new diaElemReadOnlyText(bfr,file);
      
      for(i=0;i<count;i++)
          txt[i+1]=new diaElemReadOnlyText(functions[i],"Function:");
      const char *title="Crash BackTrace";
      if(lineno) title="Assert failed";
      diaFactoryRun(title,count+1,(diaElem **)txt);
      
      //
     printf("Memory stat:\n");
     ADMImage_stat();

     exit(1); // _exit(1) ???
}
#endif
/**
    \fn saveCrashProject
    \brief Try to save the current project, useful in case of crash
*/
void saveCrashProject(void)
{
  char *baseDir=ADM_getBaseDir();
  char *name=CRASH_FILE;
  static int crashCount=0;
  if(crashCount) return ; // avoid endless looping
  crashCount++;
  char *where=new char[strlen(baseDir)+strlen(name)+2];
  strcpy(where,baseDir);
  strcat(where,"/");
  strcat(where,name);
  printf("Saving crash file to %s\n",where);
  video_body->saveAsScript (where, NULL);
}
/**
    \fn checkCrashFile
    \brief Check if there i a crash file
*/

void checkCrashFile(void)
{
  char *baseDir=ADM_getBaseDir();
  char *name=CRASH_FILE;
  static int crashCount=0;
  char *where=new char[strlen(baseDir)+strlen(name)+2];
  strcpy(where,baseDir);
  strcat(where,"/");
  strcat(where,name);
  if(ADM_fileExist(where))
  {
    if(GUI_Confirmation_HIG(_("Load it"),_("Crash file"),
       _("I have detected a crash file. \nDo you want to load it  ?\n(It will be deleted in all cases, you should save it if you want to keep it)")))
    {
       A_parseECMAScript(where);
    }
    unlink(where);
  }else
  {
    printf("No crash file (%s)\n",where); 
  }
  delete [] where;
}
//EOF
