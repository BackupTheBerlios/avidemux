/***************************************************************************
                          ADM_misc.h  -  description
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

#ifndef __ADM_MISC__
#define __ADM_MISC__
#include "time.h"

typedef enum ADM_LOG_LEVEL
{
        ADM_LOG_NONE=0,
        ADM_LOG_IMPORTANT=1,
        ADM_LOG_INFO=2,
        ADM_LOG_DEBUG=3
  
};
typedef struct ADM_date
{
  uint32_t hours,minutes,seconds; 
};
void            TLK_getDate(ADM_date *date);
void            PathSplit(char *str, char **root, char **ext);
void	        PathStripName(char *str);
const char      *GetFileName(const char *str);
char            *PathCanonize(const char *tmpname);
void            LowerCase(char *string);
uint32_t        getTime( int called );;
void            frame2time(uint32_t frame, uint32_t fps, uint16_t * hh, uint16_t * mm,
                                uint16_t * ss, uint16_t * ms);
void            time2frame(uint32_t *frame, uint32_t fps, uint32_t hh, uint32_t mm,
                                uint32_t ss, uint32_t ms);
void            ms2time(uint32_t len2,uint16_t * hh, uint16_t * mm,
                                uint16_t * ss, uint16_t * ms);				
uint32_t 		getTimeOfTheDay(void);
uint64_t 	ADM_swap64(uint64_t in);
uint32_t 	ADM_swap32(uint32_t in);
uint16_t 	ADM_swap16(uint16_t in);
inline uint32_t 		dontswap(uint32_t in) {return in;};
char 		*ADM_rindex(const char *s, int c);
char 		*ADM_index(const char *s, int c);
void 		ADM_usleep(unsigned long us);
uint8_t         ADM_fileExist(char *name);

#ifdef HAVE_GETTIMEOFDAY
	#define TIMZ struct timezone
#else
	#ifndef HAVE_STRUCT_TIMESPEC
	#define HAVE_STRUCT_TIMESPEC
	extern "C"
	{
		typedef struct timespec
		{
			time_t tv_sec;
			long int tv_nsec;
		};

		void gettimeofday(struct timeval *p, void *tz);
		};
		#define timezone int
		#define TIMZ int
	#endif
#endif

#define FRAME_PAL 1
#define FRAME_FILM 2
#define FRAME_NTSC 3

uint8_t 	identMovieType(uint32_t fps1000); // identify the movie type (mainly for mpeg1/2) 
uint8_t 	ms2time(uint32_t ms, uint32_t *h,uint32_t *m, uint32_t *s);
#ifdef ADM_BIG_ENDIAN	
	#define R64 ADM_swap64
	#define R32 ADM_swap32
	#define R16 ADM_swap16
#else
	#define R64(x) (x)
	#define R32(x) (x) 
	#define R16(x) (x) 
#endif
#endif
