//
// C++ Interface: ADM_cpuCap
//
// Description: 
//	Wrapper for all memory alloc/dealloc
//	Ensure 16 bytes bits aligment allocation using
//		posix_memalign. Altivec on non darwin is very picky about this
//	special case Win32 : see later
//
// Author: mean <fixounet@free.fr>, (C) 2004
//
// Copyright: See COPYING file that comes with this distribution
//
//
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
#include <math.h>

#include "default.h"

#include <ADM_assert.h>

#undef memalign
#undef malloc
#undef free


static uint32_t ADM_consumed=0;

extern "C" {

void *av_malloc(unsigned int size);
void av_free(void *ptr);
void *av_realloc(void *ptr, unsigned int size);
}
void ADM_memStat( void );
void ADM_memStat( void )
{
	printf("Global mem stat\n");
	printf("\tMemory consumed :%lu (MB)\n",ADM_consumed>>20);

}
void *ADM_alloc(size_t size)
{
char *c;
uint64_t l,lorg;
uint32_t *backdoor;

	l=(uint64_t)malloc(size+32);
	// Get next boundary
	lorg=l;
	l=(l+15)& 0xfffffffffffffff0LL;
	l+=16;
	c=(char *)l;
	backdoor=(uint32_t *)(c-8);
	*backdoor=(0xdead<<16)+l-lorg;
	backdoor[1]=size;
	ADM_consumed+=size;
	return c;

}
void ADM_dezalloc(void *ptr)
{
	uint32_t *backdoor;
	uint32_t size,offset;
	char	 *c=(char *)ptr;

	if(!ptr) return;

	backdoor=(uint32_t *)ptr;
	backdoor-=2;
	
	ADM_assert(((*backdoor)>>16)==0xdead);
	
	offset=backdoor[0]&0xffff;
	size=backdoor[1];
	free(c-offset);
	ADM_consumed-=size;
}

void *operator new( size_t t)
{
	return ADM_alloc(t);
}
void *operator new[] ( size_t t)
{
	return ADM_alloc(t);
}
void operator delete (void *c)
{
	ADM_dezalloc(c);
}
void operator delete[] (void *c)
{
	ADM_dezalloc(c);
}
//********************************
// lavcodec wrapper
//********************************
void *av_malloc(unsigned int size)
{
 	return ADM_alloc(size);
}

/**
 * av_realloc semantics (same as glibc): if ptr is NULL and size > 0,
 * identical to malloc(size). If size is zero, it is identical to
 * free(ptr) and NULL is returned.  
 */

void *ADM_realloc(void *ptr, size_t newsize)
{
  void *nalloc;
    
    if(!ptr) return ADM_alloc(newsize);
    if(!newsize) 
    {
    	
    	ADM_dealloc(ptr);
	return NULL;
    }
    // now we either shrink them or expand them
    // in case of shrink, we do nothing
    // in case of expand we have to copy
    // Do copy everytime (slower)
	uint32_t *backdoor;
	uint32_t size,offset;
	char	 *c=(char *)ptr;
	
	backdoor=(uint32_t *)ptr;
	backdoor-=2;
	
	ADM_assert(((*backdoor)>>16)==0xdead);
	
	offset=backdoor[0]&0xffff;
	size=backdoor[1];
	if(size>=newsize) // do nothing
		return ptr;
	// Allocate a new one
	nalloc=ADM_alloc(newsize);
	
	memcpy(nalloc,ptr,size);
	ADM_dealloc(ptr);
	return nalloc;
}
void *av_realloc(void *ptr, unsigned int newsize)
{
	return ADM_realloc(ptr,newsize);
}
/* NOTE: ptr = NULL is explicetly allowed */
void av_free(void *ptr)
{
	if(ptr)
		ADM_dealloc(ptr);  
}

char *ADM_strdup(const char *in)
{
	uint32_t l=strlen(in);
	char	*out;
	//ADM_assert(l);
	out=(char *)ADM_alloc(l+1);
	memcpy(out,in,l+1);
	return out;
	

}
// EOF
