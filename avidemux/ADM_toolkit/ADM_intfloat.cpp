/***************************************************************************
                 
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

#include "ADM_default.h"


#include "ADM_assert.h"

#include "ADM_intfloat.h"

static uint8_t ADM_int2float_C(int16_t *in, float *out,int nb);
static uint8_t ADM_float2int_C(float *in,int16_t *out,int nb);

 ptr_Int2Float *pADM_int2float;
 ptr_Float2Int *pADM_float2int;

uint8_t ADM_intFloatInit(void)
{
  pADM_int2float=&ADM_int2float_C;
  pADM_float2int=&ADM_float2int_C;
  return 1;
}
uint8_t ADM_int2float_C(int16_t *in, float *out,int nb)
{
  return 0;
  
}
uint8_t ADM_float2int_C(float *in,int16_t *out,int nb)
{
  return 0;
  
}

