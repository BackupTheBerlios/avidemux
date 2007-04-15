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
#include <QMessageBox>
#include <QGridLayout>
#include <QCheckBox>

#include "default.h"
#include "ADM_commonUI/DIA_factory.h"
#include "ADM_assert.h"
diaElemFrame::diaElemFrame(const char *toggleTitle, const char *tip)
  : diaElem(ELEM_FRAME)
{
  
}
void diaElemFrame::swallow(diaElem *widget)
{
 
}
diaElemFrame::~diaElemFrame()
{
  
}
void diaElemFrame::setMe(void *dialog, void *opaque,uint32_t line)
{
 
}
void diaElemFrame::getMe(void)
{
  
}
void diaElemFrame::enable(uint32_t onoff)
{

}
void diaElemFrame::finalize(void)
{
}


//******************************************************

//EOF
