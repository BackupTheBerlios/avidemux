/***************************************************************************
                        Debug #define, ugly actually
                             -------------------
    begin                : Fri Apr 12 2002
    copyright            : (C) 2002 by mean
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
#ifndef ADM_INTFLOAT_H
#define ADM_INTFLOAT_H

typedef uint8_t ptr_Int2Float(int16_t *in, float *out,int nb);
typedef uint8_t ptr_Float2Int(float *in,int16_t *out,int nb);


uint8_t ADM_intFloatInit(void);

extern ptr_Int2Float *pADM_int2float;
extern ptr_Float2Int *pADM_float2int;


#endif
