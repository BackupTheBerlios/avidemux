/***************************************************************************
              
    copyright            : (C) 2006 Mean
    
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef ROTATE_PARAM_H
#define ROTATE_PARAM_H

typedef struct ROTATE_PARAM
{
  uint32_t width;
  uint32_t height;
  float    angle;
}ROTATE_PARAM;

void do_rotate(ADMImage *source,ADMImage *target,float angle);
#endif
