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
 
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"

void *ADM_alloc(size_t size)
{
void *c;
	ADM_assert(!posix_memalign(&c,16,size));
	return c;

}
void ADM_dezalloc(void *ptr)
{
	free(ptr);
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
// EOF
