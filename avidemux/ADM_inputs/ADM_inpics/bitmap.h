/***************************************************************************
                          bitmap.h  -  description
                             -------------------
    begin                : Sat Dec 22 2001
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

typedef struct BITMAPHEADER
{
    uint32_t size;
    int32_t  width;
    int32_t  height;
    uint16_t numBitPlanes;
    uint16_t numBitsPerPlane;
    uint32_t compressionScheme;
    uint32_t sizeOfImageData;
    uint32_t xResolution;
    uint32_t yResolution;
    uint32_t numColorsUsed;
    uint32_t numImportantColors;
/*
    uint16_t resolutionUnits;
    uint16_t padding;
    uint16_t origin;
    uint16_t halftoning;
    uint32_t halftoningParam1;
    uint32_t halftoningParam2;
    uint32_t colorEncoding;
    uint32_t identifier;
*/
} BITMAPHEADER;

#ifndef _WINGDI_
typedef struct BITMAPFILEHEADER
{
   uint16_t    bfType;
   uint32_t    bfSize;
   int16_t     bfReserved1;
   int16_t     bfReserved2;
   uint32_t    bfOffBits;
}  BITMAPFILEHEADER;
#endif
