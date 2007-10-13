/***************************************************************************
                       
    copyright            : (C) 2007 by mean
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
 #ifndef ADM_COLORSPACE_H
 #define ADM_COLORSPACE_H
#include "ADM_rgb.h" // To have colors
class ADMColorspace
{
  protected:
    void  *context;
    uint32_t width,height;
    ADM_colorspace fromColor,toColor;
    uint8_t getStrideAndPointers(uint8_t  *from,ADM_colorspace fromColor,uint8_t **srcData,int *srcStride);
  public :
    
    ADMColorspace(uint32_t w, uint32_t h, ADM_colorspace from,ADM_colorspace to);
    uint8_t convert(uint8_t  *from, uint8_t *to);
    ~ADMColorspace();
};
 
#endif
//EOF

