/***************************************************************************
                          toolkit.cpp  -  description
                             -------------------



    begin                : Fri Dec 14 2001
    copyright            : (C) 2001 by mean
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
#include <time.h>
#include <math.h>
#include <string.h>
#ifndef CYG_MANGLING
#include <unistd.h>
#endif
//#include <gtk/gtk.h>
#include <ctype.h>
#include <sys/time.h>

#include "default.h"
#include "ADM_assert.h"
#include "ADM_misc.h"


//
//    Sleep for n ms
//
void GUI_Sleep(uint32_t ms)
{
    if (ms < 10)
	return;
    ADM_usleep(ms*1000);
}


// Get tick (in ms)
// Call with a 0 to initialize
// Call with a 1 to read
//_____________________
uint32_t getTime(int called)
{

    static struct timeval timev_s;

    static struct timeval timev;
    static TIMZ timez;

    int32_t tt;

    if (!called)
      {
	  called = 1;
	  gettimeofday(&timev_s, &timez);
	  return 0;
      }
    gettimeofday(&timev, &timez);
    tt = timev.tv_usec - timev_s.tv_usec;
    tt /= 1000;
    tt += 1000 * (timev.tv_sec - timev_s.tv_sec);
    return (tt);
}
uint32_t getTimeOfTheDay(void)
{
  

     struct timeval timev;
     TIMZ timez;

    int32_t tt;

   
    gettimeofday(&timev, &timez);
    tt = timev.tv_usec;
    tt /= 1000;
    tt += 1000 * (timev.tv_sec);
    return (tt&0xffffff);

}
void ms2time(uint32_t len2,uint16_t * hh, uint16_t * mm,
	 			uint16_t * ss, uint16_t * ms)
{
    *hh = (uint32_t) floor(len2 / (3600.F * 1000.F));
    len2 -= (uint32_t) floor(*hh * 3600.F * 1000.F);
    *mm = (uint32_t) floor(len2 / (60.F * 1000.F));
    len2 -= (uint32_t) floor(*mm * 60.F * 1000.F);
    *ss = (uint32_t) floor(len2 / (1000.F));
    len2 -= (uint32_t) floor((*ss * 1000.F));
    *ms = (uint32_t) floor(len2);

}
void            time2frame(uint32_t *frame, uint32_t fps, uint32_t hh, uint32_t mm,
                                uint32_t ss, uint32_t ms)
{
// convert everything to ms : uint32_t = 1000 hours, should be plenty enough
uint32_t count=0;
                count+=ms;
                count+=ss*1000;
                count+=mm*60*1000;
                count+=hh*3600*1000;
double d;
                d=count;                
                // ms
                d=d*fps;
                d/=1000;
                d/=1000;
                *frame= (uint32_t)(floor(d+0.5));

}

uint64_t ADM_swap64(uint64_t in)
{
uint32_t low,high;
uint64_t out;
        high=in>>32;
        low=in&0xffffffff;
        high=ADM_swap32(high);
        low=ADM_swap32(low);
        out=low;
        out=(out<<32)+high;
        return out;
  
}
// swap BE/LE : Ugly
uint32_t ADM_swap32( uint32_t in)
{
        uint8_t r[4],u;
        memcpy(&r[0],&in,4);
        u=r[0];
        r[0]=r[3];
        r[3]=u;
        u=r[1];
        r[1]=r[2];
        r[2]=u;
        memcpy(&in,&r[0],4);
        return in;
}
// swap BE/LE : Ugly
uint16_t ADM_swap16( uint16_t in)
{
	return ( (in>>8) & 0xff) + ( (in&0xff)<<8);
}
uint8_t 	identMovieType(uint32_t fps1000)
{
#define INRANGE(value,type)  \
      {\
              if((fps1000 > value-300) &&( fps1000 < value+300))\
              {\
                r=type;\
                printf("Looks like "#type" \n");\
                }\
      }
      uint8_t r=0;
      INRANGE(25000,FRAME_PAL);
      INRANGE(23976,FRAME_FILM);
      INRANGE(29970,FRAME_NTSC);

      return r;
}
uint8_t ms2time(uint32_t ms, uint32_t *h,uint32_t *m, uint32_t *s)
{
      uint32_t sectogo;
      int  mm,ss,hh;


                              // d is in ms, divide by 1000 to get seconds
                              sectogo = (uint32_t) floor(ms / 1000.);
                              hh=sectogo/3600;
                              sectogo=sectogo-hh*3600;
                              mm=sectogo/60;
                              ss=sectogo%60;

                              *h=hh;
                              *m=mm;
                              *s=ss;
      return 1;
}
// Convert everything to lowercase
void  LowerCase(char *string)
{
  int l=strlen(string)-1;
        for(int i=l;i>=0;i--)
        {
                string[i]=tolower(string[i]);
        }

}
// Does not exist in cygwin
char *ADM_rindex(const char *s, int c)
{
      if(!s) return NULL;
      if(!*s) return NULL;
      uint32_t l=strlen(s)-1;
      while(l)
      {
              if(s[l]==c) return (char *)(s+l);
              l--;
      }
      return NULL;
}
// Does not exist in cygwin
char  *ADM_index(const char *s, int c)
{
      if(!s) return NULL;
      if(!*s) return NULL;
      uint32_t m=strlen(s);
      uint32_t l=0;
      while(l<m)
      {
              if(s[l]==c) return (char *)(s+l);
              l++;
      }
      return NULL;
}
uint8_t         ADM_fileExist(char *name)
{
FILE *file;
                file=fopen(name,"rb");
                if(!file) return 0;
                fclose(file);
                return 1;

}
/*
    In some case (e.g. javascript), the reader expects unixish path 
    c:/foo/bar/c.c
    and the "natural" path is c:\foo\bar
    
    This function convert the later to the former

*/
extern char *slashToBackSlash(char *in)
{
    char *out,*cout;
    int n;
    n=strlen(in);
    cout=out=(char *)ADM_alloc(n+1);   
    for(int i=0;i<n+1;i++)
    {
        if(   in[i]=='\\') out[i]='/';
        else    out[i]=in[i];
        
    }
    return cout;
    
}
/*
    
*/
void TLK_getDate(ADM_date *date)
{
  time_t timez;
  tm *t;
  time(&timez);
  t=localtime(&timez);
  if(t)
  {
    date->hours=t->tm_hour;
    date->minutes=t->tm_min;
    date->seconds=t->tm_sec;
  }
  
}

//EOF