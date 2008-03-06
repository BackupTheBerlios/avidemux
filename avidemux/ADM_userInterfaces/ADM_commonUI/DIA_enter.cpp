/***************************************************************************
                          DIA_enter.cpp
  
  (C) Mean 2007 fixounet@free.fr

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "ADM_default.h"

#include "ADM_assert.h"

#include "DIA_enter.h"
#include "DIA_factory.h"

uint8_t  		GUI_getIntegerValue(int *valye, int min, int max, const char *title)
{
	return DIA_GetIntegerValue(valye,min,max,title,title);
}
/**
      \fn DIA_GetIntegerValue( 
      \brief Popup a window asking for a value between min & max (int)
      @return 1 on success, 0 on failure

*/

uint8_t  DIA_GetIntegerValue(int *value, int min, int max, const char *title, const char *legend)
{

    int32_t val=*value;;
     
    diaElemInteger     fval(&val,legend,min,max);
    
      diaElem *elems[1]={&fval};

   if(diaFactoryRun(title,1,elems))
  {
    *value=(int)val;
    return 1;
  }
  return 0;	
}
/**
      \fn DIA_GetFloatValue
      \brief Popup a window asking for a value between min & max
      @return 1 on success, 0 on failure

*/
uint8_t  DIA_GetFloatValue(float *value, float min, float max, const char *title, const char *legend)
{
  ELEM_TYPE_FLOAT val=*value;;
     
    diaElemFloat     fval(&val,legend,min,max);
    
      diaElem *elems[1]={&fval};

   if(diaFactoryRun(title,1,elems))
  {
    *value=(float)val;
    return 1;
  }
  return 0;	
}

