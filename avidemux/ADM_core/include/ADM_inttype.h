/** *************************************************************************
    \fn ADM_inttype.h
    \brief Defint plaform agnostic var type
                      
    copyright            : (C) 2008 by mean
    
 ***************************************************************************/

#ifndef __ZDEFAULT
#define __ZDEFAULT

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

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
#ifdef ADM_CPU_64BIT
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

/*
 * Standard gettext macros.
 */
#ifdef HAVE_GETTEXT
#  include <libintl.h>
#  undef _
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#endif

extern const char* translate(const char *__domainname, const char *__msgid);

#ifdef QT_TR_NOOP
#undef QT_TR_NOOP
#endif

#define QT_TR_NOOP(String) translate (PACKAGE, String)

#if (defined( HAVE_LIBESD) && defined(HAVE_ESD_H)) || \
 defined(OSS_SUPPORT) || defined (USE_ARTS) || \
  defined(USE_SDL) || defined(__APPLE__) || \
  defined(__WIN32) || defined(ALSA_SUPPORT)
  
#define HAVE_AUDIO
#endif
               
#endif
