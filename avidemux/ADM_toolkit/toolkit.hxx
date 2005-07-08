/***************************************************************************
                          toolkit.hxx  -  description
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

#ifndef __TOOLKIT__
#define __TOOLKIT__
#include "time.h"
void 		PathSplit(char *str, char **root, char **ext);
void			PathStripName(char *str);
char 		*PathCanonize(const char *tmpname);
void		LowerCase(char *string);
void 		GUI_Alert(const char *alertstring);
void            GUI_Info(const char *alertstring);
int 			GUI_Question(const char *alertstring);
void			GUI_Sleep(uint32_t ms);
int 			GUI_Alternate(char *title,char *choice1,char *choice2);
uint32_t 		getTime( int called );;
void 		runDialog(volatile int *lock);
// warning : they are in GUI_enter
uint8_t  		GUI_getDoubleValue(double *valye, float min, float max, const char *title);
uint8_t  		GUI_getIntegerValue(int *valye, int min, int max, const char *title);

void 		frame2time(uint32_t frame, uint32_t fps, uint16_t * hh, uint16_t * mm,
				uint16_t * ss, uint16_t * ms);
void            time2frame(uint32_t *frame, uint32_t fps, uint32_t hh, uint32_t mm,
                                uint32_t ss, uint32_t ms);
void 		ms2time(uint32_t len2,uint16_t * hh, uint16_t * mm,
	 			uint16_t * ss, uint16_t * ms);				
uint32_t 		getTimeOfTheDay(void);
/** return 1 if we are in silent mode */
uint8_t		isQuiet(void);
uint64_t 		swap64(uint64_t in);
uint32_t 		swap32(uint32_t in);
uint16_t 		swap16(uint16_t in);
inline uint32_t 		dontswap(uint32_t in) {return in;};
char 		*ADM_rindex(const char *s, int c);
char 		*ADM_index(const char *s, int c);
void 		ADM_usleep(unsigned long us);
uint8_t         ADM_fileExist(char *name);

#ifdef CYG_MANGLING
#ifndef HAVE_STRUCT_TIMESPEC
#define HAVE_STRUCT_TIMESPEC
	typedef struct timespec
	{
		time_t tv_sec;
		long int tv_nsec;
	};

	void gettimeofday(struct timeval *p, void *tz);
	#define timezone int
	#define usleep ADM_usleep
	#define TIMZ int
#endif
#else
	#define TIMZ struct timezone
#endif

#define FRAME_PAL 1
#define FRAME_FILM 2
#define FRAME_NTSC 3

uint8_t 	identMovieType(uint32_t fps1000); // identify the movie type (mainly for mpeg1/2) 
uint8_t 	ms2time(uint32_t ms, uint32_t *h,uint32_t *m, uint32_t *s);
#ifdef ADM_BIG_ENDIAN	
	#define R64 swap64
	#define R32 swap32
	#define R16 swap16
#else
	#define R64(x) (x)
	#define R32(x) (x) 
	#define R16(x) (x) 
#endif
#endif
