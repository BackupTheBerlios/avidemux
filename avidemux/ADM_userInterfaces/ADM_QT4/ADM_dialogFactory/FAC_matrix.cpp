/***************************************************************************
  FAC_toggle.cpp
  Handle dialog factory element : Toggle
  (C) 2006 Mean Fixounet@free.fr 
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <QDialog>
#include <QSpinBox>
#include <QGridLayout>
#include <QLabel>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"

extern const char *shortkey(const char *);




diaElemMatrix::diaElemMatrix(uint8_t *trix,const char *toggleTitle, uint32_t trixSize,const char *tip)
  : diaElem(ELEM_MATRIX)
{
}

diaElemMatrix::~diaElemMatrix()
{
}
void diaElemMatrix::setMe(void *dialog, void *opaque,uint32_t line)
{

}
void diaElemMatrix::getMe(void)
{
  
}
void diaElemMatrix::enable(uint32_t onoff)
{
}

