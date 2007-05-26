#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "default.h" 
#include "ADM_misc.h"


#ifdef ADM_WIN32
#include "windows.h"
#include "winbase.h"
#include "io.h"
#include "winsock2.h"
#define WIN32_CLASH
#include "ADM_assert.h" 
uint8_t win32_netInit(void);

void ADM_usleep(unsigned long us)
{
	Sleep(us/1000);
}

uint8_t win32_netInit(void)
{
WSADATA wsaData;
	int iResult;
		printf("Initializing WinSock\n");
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
		if (iResult != NO_ERROR)
		{
			printf("Error at WSAStartup()\n");
			return 0;
		}	
		printf("WinSock ok\n");
		return 1;
}

#ifndef HAVE_GETTIMEOFDAY
extern "C"
{
void gettimeofday(struct timeval *p, void *tz);
}

void gettimeofday(struct timeval *p, void *tz)
{
unsigned long int sec;
    SYSTEMTIME  tme;
    GetSystemTime(&tme);
    sec=tme.wSecond;
    sec+=60*tme.wMinute;
    sec+=60*60*tme.wHour;
    
	p->tv_sec=sec;
	p->tv_usec=tme.wMilliseconds*1000;
	return;
  
	
}
#endif

uint64_t ftello_adm(FILE *f)
{
	fpos_t pos;
	fgetpos(f,&pos);
	return (uint64_t)pos;
}
int fseeko_adm(FILE *f,fpos_t off,int whence)
{
    int64_t pos;
	switch(whence)
	{
		case SEEK_SET:
			fsetpos(f,&off);
			return 0;
			break;
		case SEEK_END:
		    ADM_assert(!off);
		    pos=_filelengthi64(fileno(f)) ;
		    if(pos==-1) return -1;		   
			fsetpos(f,&pos);
			return 0;
			break;
		case SEEK_CUR:
			off+=ftello_adm(f);
			fsetpos(f,&off);
			return 0;

	}
	ADM_assert(0);
	return 0;
 }

int getpriority(int which, int who)
{
	unsigned int priorityClass;

	ADM_assert(which == PRIO_PROCESS);
	ADM_assert(who == 0);

	priorityClass = GetPriorityClass(GetCurrentProcess());

	switch (priorityClass)
	{
		case HIGH_PRIORITY_CLASS:
			return -18;
			break;
		case ABOVE_NORMAL_PRIORITY_CLASS:
			return -10;
			break;
		case NORMAL_PRIORITY_CLASS:
			return 0;
			break;
		case BELOW_NORMAL_PRIORITY_CLASS:
			return 10;
			break;
		case IDLE_PRIORITY_CLASS:
			return 18;
			break;
		default:
			ADM_assert(0);
	}
}

int setpriority(int which, int who, int value)
{
	unsigned int priorityClass;

	ADM_assert(which == PRIO_PROCESS);
	ADM_assert(who == 0);
	ADM_assert(value >= PRIO_MIN && value <= PRIO_MAX);

	if (value >= -20 && value <= -16)
	{
		priorityClass = HIGH_PRIORITY_CLASS;
	}
	else if (value >= -15 && value <= -6)
	{
		priorityClass = ABOVE_NORMAL_PRIORITY_CLASS;
	}
	else if (value >= -5 && value <= 4)
	{
		priorityClass = NORMAL_PRIORITY_CLASS;
	}
	else if (value >= 6 && value <= 15)
	{
		priorityClass = BELOW_NORMAL_PRIORITY_CLASS;
	}
	else if (value >= 16 && value <= 20)
	{
		priorityClass = IDLE_PRIORITY_CLASS;
	}

	if (!SetPriorityClass(GetCurrentProcess(), priorityClass))
	{
		return -1;
	}

	return 0;
}

int shutdown_win32(void)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;

	// Get a token for this process. 
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		return -1;
	}

	// Get the LUID for the shutdown privilege.
	LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

	tkp.PrivilegeCount = 1;  // one privilege to set
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	// Get the shutdown privilege for this process.
	AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

	if (GetLastError() != ERROR_SUCCESS)
	{
		return -1;
	}

	// Shut down the system and force all applications to close.
	if (!ExitWindowsEx(EWX_POWEROFF | EWX_FORCE, SHTDN_REASON_FLAG_PLANNED))
	{
		return -1;
	}

	return 0;
}
#endif

