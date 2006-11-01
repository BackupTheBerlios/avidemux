/***************************************************************************
                    
    copyright            : (C) 2006 by mean
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
#include <string.h>
#include "default.h"
#include <ADM_assert.h>


size_t ADM_fread (void *ptr, size_t size, size_t n, FILE *sstream)
{
  return fread(ptr,size,n,sstream); 
  
}

size_t ADM_fwrite (void *ptr, size_t size, size_t n, FILE *sstream)
{
  return fwrite(ptr,size,n,sstream); 
  
}
