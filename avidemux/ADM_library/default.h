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
/*
   	The maximum width/height is MAXIMUM_SIZE 768*768 for now
*/
#define MAXIMUM_SIZE 2048


typedef unsigned char ADM_filename ;

#ifdef HAVE_STDINT_H
#include <stdint.h>
#define GOT_TYPES
#endif

#ifdef HAVE_INTTYPES_H
	#define __STDC_FORMAT_MACROS
	#include <inttypes.h>
	#define GOT_TYPES
#ifndef SCNu8
#define SCNu8 "u"
#endif
#endif

#ifndef GOT_TYPES
#ifdef ARCH_X86_64
#define int32_t 	signed int
#define int64_t 	signed long int 
#define uint64_t 	unsigned long  int
#else
#define int32_t 	signed long  int
#define int64_t 	signed long  long
#define uint64_t 	unsigned long  long

#endif
#define uint8_t  	unsigned char
#define int8_t   	signed char

#define int16_t 	signed short int
#define uint16_t 	unsigned short int
#define uint32_t 	unsigned long  int

#endif
#define UNUSED_ARG(a) do {/* null */} while (&a == 0)

#define MKFCC(a,b,c,d)   ((d<<24)+(c<<16)+(b<<8)+a)
#define MKFCCR(a,b,c,d)   ((a<<24)+(b<<16)+(c<<8)+d)

// 0 means error, 1 means ok, 2 means ignore
// 2 is seldom used

#define ADM_ERR 0
#define ADM_OK 	1
#define ADM_IGN 2

// For win32, useless else
#ifdef __cplusplus
uint64_t ftello_adm(FILE *f);
int fseeko_adm(FILE *f,fpos_t off,int whence);
#endif
/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define N_(String) (String)
#endif

#endif
