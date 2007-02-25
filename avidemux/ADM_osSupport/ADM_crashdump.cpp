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
#include <ADM_assert.h>
#include <math.h>

#include "default.h"
#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"

#ifdef CYG_MANGLING
void installSigHandler()
{

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
     
     void *stack[20];
     char **functions;
     int count, i;
     static int running=0;
      if(running) 
      {
        signo=0;
        exit(1);
      }
      running=0; 
      
      
      
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
      diaElemReadOnlyText *txt[count];
      for(i=0;i<count;i++)
          txt[i]=new diaElemReadOnlyText(functions[i],"Function:");
      diaFactoryRun("BackTrace",count,(diaElem **)txt);
      
      //
     printf("Memory stat:\n");
     ADMImage_stat();
     signo=0; // will keep GCC happy
     exit(1); // _exit(1) ???
}
#endif
//EOF
