/***************************************************************************
                          default.h  -  description
                             -------------------
    begin                : Thu Nov 1 2001
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
#ifndef __ZDEFAULT
#define __ZDEFAULT
#ifdef HAVE_STDINT_H
#include <stdint.h>
#else
	#ifdef HAVE_INTTYPES_H
		#include <inttypes.h>
	#else

#define int16_t 	signed short int
#define int32_t 	signed long  int

#define uint16_t 	unsigned short int
#define uint32_t 	unsigned long  int

#define int64_t 	signed long  long
#define uint64_t 	unsigned long  long

#define uint8_t  	unsigned char
#define int8_t   	signed char

#endif
#endif
#define UNUSED_ARG(a) do {/* null */} while (&a == 0)

#define MKFCC(a,b,c,d)   ((d<<24)+(c<<16)+(b<<8)+a)
#define MKFCCR(a,b,c,d)   ((a<<24)+(b<<16)+(c<<8)+d)

// 0 means error, 1 means ok, 2 means ignore
// 2 is seldom used

#define ADM_ERR 0
#define ADM_OK 	1
#define ADM_IGN 2
#endif
