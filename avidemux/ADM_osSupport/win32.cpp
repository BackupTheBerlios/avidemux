#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "default.h" 
#include "ADM_misc.h"


#ifdef CYG_MANGLING
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



#endif

