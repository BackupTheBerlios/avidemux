/***************************************************************************
  Try to display interesting crash dump

    copyright            : (C) 2007 by mean, (C) 2007 Gruntster
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

#if defined(ADM_WIN32)
#define WIN32_CLASH
#include <windows.h>
#include <excpt.h>
#include <imagehlp.h>
#endif

#include "default.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_userInterfaces/ADM_commonUI/DIA_factory.h"
#include "ADM_editor/ADM_edit.hxx"
extern ADM_Composer *video_body;

#define CRASH_FILE "crash.js"

static void saveCrashProject(void);
extern char *ADM_getBaseDir(void);
extern void A_parseECMAScript(const char *name);

#ifdef __APPLE__
void installSigHandler() {}

void ADM_backTrack(int lineno,const char *file)
{
	char bfr[1024];
	saveCrashProject();
	snprintf(bfr,1024,"Assert Failed at file %s, line %d\n",file,lineno);
	GUI_Error_HIG("Fatal Error",bfr);
	assert(0);
}
#elif defined(WIN32)
typedef struct STACK_FRAME
{
    STACK_FRAME* ebp;	// address of the calling function frame
    uint8_t* retAddr;	// return address
    uint32_t param[0];	// parameter list (could be empty)
} STACK_FRAME;

static void dumpFrame(void* processId, void* frameAddr)
{
	MEMORY_BASIC_INFORMATION mbi;
	char moduleName[MAX_PATH];
	HMODULE moduleAddr;
	DWORD symDisplacement;
	IMAGEHLP_SYMBOL* pSymbol;

	if (VirtualQuery(frameAddr, &mbi, sizeof(mbi)))
	{
		moduleName[0] = 0;
		moduleAddr = (HMODULE)mbi.AllocationBase;
		
		GetModuleFileName(moduleAddr, moduleName, MAX_PATH);

		printf("%s(", moduleName);

		if (SymGetSymFromAddr(processId, (uint32_t)frameAddr, &symDisplacement, pSymbol))
			printf("%s", pSymbol->Name);
		else
			printf("<unknown>");

		printf("+0x%X) [0x%08X]\n", (uint32_t)frameAddr - (uint32_t)moduleAddr, frameAddr);

		fflush(stdout);
	}
}
	
static void dumpExceptionInfo(void* processId, struct _EXCEPTION_RECORD* pExceptionRec, struct _CONTEXT* pContextRecord)
{
	printf("\n*********** EXCEPTION **************\n");
	printf("Registers:\n");
	printf("EAX: %08X  EBX: %08X  ECX: %08X  EDX: %08X  ESI: %08X\n", pContextRecord->Eax, pContextRecord->Ebx, pContextRecord->Ecx, pContextRecord->Edx, pContextRecord->Esi);
	printf("EDI: %08X  ESP: %08X  EBP: %08X  EIP: %08X  EFlags: %08X\n\n", pContextRecord->Edi, pContextRecord->Esp, pContextRecord->Ebp, pContextRecord->Eip, pContextRecord->EFlags);

	printf("Exception Code: ");

	switch (pExceptionRec->ExceptionCode)
	{
		case EXCEPTION_ACCESS_VIOLATION:
			printf("EXCEPTION_ACCESS_VIOLATION");
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			printf("EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
			break;
		case EXCEPTION_BREAKPOINT:
			printf("EXCEPTION_BREAKPOINT");
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			printf("EXCEPTION_DATATYPE_MISALIGNMENT");
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			printf("EXCEPTION_FLT_DENORMAL_OPERAND");
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			printf("EXCEPTION_FLT_DIVIDE_BY_ZERO");
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			printf("EXCEPTION_FLT_INEXACT_RESULT");
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			printf("EXCEPTION_FLT_INVALID_OPERATION");
			break;
		case EXCEPTION_FLT_OVERFLOW:
			printf("EXCEPTION_FLT_OVERFLOW");
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			printf("EXCEPTION_FLT_STACK_CHECK");
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			printf("EXCEPTION_FLT_UNDERFLOW");
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			printf("EXCEPTION_ILLEGAL_INSTRUCTION");
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			printf("EXCEPTION_IN_PAGE_ERROR");
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			printf("EXCEPTION_INT_DIVIDE_BY_ZERO");
			break;
		case EXCEPTION_INT_OVERFLOW:
			printf("EXCEPTION_INT_OVERFLOW");
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			printf("EXCEPTION_INVALID_DISPOSITION");
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			printf("EXCEPTION_NONCONTINUABLE_EXCEPTION");
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			printf("EXCEPTION_PRIV_INSTRUCTION");
			break;
		case EXCEPTION_SINGLE_STEP:
			printf("EXCEPTION_SINGLE_STEP");
			break;
		case EXCEPTION_STACK_OVERFLOW:
			printf("EXCEPTION_STACK_OVERFLOW");
			break;
		default:
			printf("UNKNOWN");
	}
	
	printf(" (%08X)\n", pExceptionRec->ExceptionCode);
	printf("Exception Flags: %08X\n", pExceptionRec->ExceptionFlags);

	printf("\nOrigin:\n");
	dumpFrame(processId, (void*)pContextRecord->Eip);
	printf("*********** EXCEPTION **************\n");
	fflush(stdout);
}

extern "C"
{
void dumpBackTrace(void* processId)
{
	if (!processId)
		processId = GetCurrentProcess();

	const int maxAddrCount = 32;

	printf("\n*********** BACKTRACE **************\n");

	// Get frame address using builtin GCC function.
	STACK_FRAME* stackFrame = (STACK_FRAME*)__builtin_frame_address(0);

    for (int retAddrCount = 0; (retAddrCount < maxAddrCount) && !IsBadReadPtr(stackFrame, sizeof(STACK_FRAME)) && !IsBadCodePtr(FARPROC(stackFrame->retAddr)); retAddrCount++, stackFrame = stackFrame->ebp)
    {
		printf("Frame %2d: ", retAddrCount);
		dumpFrame(processId, stackFrame->retAddr);
		fflush(stdout);
    }

	printf("*********** BACKTRACE **************\n\n");
}
}

void ADM_backTrack(int lineno, const char *file)
{	
	fflush(stderr);
	fflush(stdout);
	saveCrashProject();

	GUI_Error_HIG(QT_TR_NOOP("Fatal Error"),QT_TR_NOOP("A fatal error has occurred.\n\nClick OK to generate debug information. This may take a few minutes to complete."));

	void* currentProcessId = GetCurrentProcess();

	SymInitialize(currentProcessId, NULL, TRUE);
	dumpBackTrace(currentProcessId);
	SymCleanup(currentProcessId);

	printf("Assert failed at file %s, line %d\n\n",file,lineno);

	exit(1);
}

EXCEPTION_DISPOSITION exceptionHandler(struct _EXCEPTION_RECORD* pExceptionRec, void* pEstablisherFrame, struct _CONTEXT* pContextRecord, void* pDispatcherContext)
{
	fflush(stderr);
	fflush(stdout);
	static int running=0;

	if(running)
		exit(1);

	running=1;

	saveCrashProject();

	GUI_Error_HIG(QT_TR_NOOP("Fatal Error"),QT_TR_NOOP("A fatal error has occurred.\n\nClick OK to generate debug information. This may take a few minutes to complete."));

	void* currentProcessId = GetCurrentProcess();

	SymInitialize(currentProcessId, NULL, TRUE);

	dumpExceptionInfo(currentProcessId, pExceptionRec, pContextRecord);
	fflush(stdout);
	dumpBackTrace(currentProcessId);

	SymCleanup(currentProcessId);

	exit(1);
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
  delete[] where;
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
    if(GUI_Confirmation_HIG(QT_TR_NOOP("Load it"),QT_TR_NOOP("Crash file"),
       QT_TR_NOOP("I have detected a crash file. \nDo you want to load it  ?\n(It will be deleted in all cases, you should save it if you want to keep it)")))
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
