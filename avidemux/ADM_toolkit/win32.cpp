#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef CYG_MANGLING
#include "windows.h"
#include "winbase.h"
void ADM_usleep(unsigned long us)
{
	Sleep(us/1000);
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

