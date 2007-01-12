
/***************************************************************************
                         DIA_resizeWiz
                             -------------------

                           Ui for hue & sat
    copyright            : (C) 2004/5 by mean
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



#include <config.h>
#include <math.h>
#include <string.h>
#include <stdio.h>

#include "default.h"

#include "ADM_assert.h"

#include "DIA_resizeWiz.h"
#include "DIA_factory.h"


uint8_t DIA_resizeWiz(RESWIZ_FORMAT *format, RESWIZ_AR *source, RESWIZ_AR *destination)
{
uint8_t r=0;
#define ONELINE(x,y) {RESWIZ_##x,_(y),NULL}
            diaMenuEntry menuFTarget[6]={
                             ONELINE(VCD,"VCD"),
                             ONELINE(SVCD,"SVCD"),
                             ONELINE(DVD,"DVD"),
                             ONELINE(DVD_HD1,"DVD Half D1"),
                             ONELINE(PSP,"PSP"),
                             ONELINE(PSP_FULLRES,"PSP Full Res"),
                          };
             diaMenuEntry menuFAspect[3]={
               {RESWIZ_AR_1_1,"1:1",NULL},
               {RESWIZ_AR_4_3,"4:3",NULL},
               {RESWIZ_AR_16_9,"16:9",NULL}
                          };
  
  
                          
    uint32_t tformat=(uint32_t )*format;
    uint32_t tsource=(uint32_t )*source;
    uint32_t tdestination=(uint32_t )*destination;
                          
    diaElemMenu     menu1(&tformat,_("Target type"), 6,menuFTarget);
    diaElemMenu     menu2(&tsource,_("Source aspect ratio"), 3,menuFAspect);
    diaElemMenu     menu3(&tdestination,_("Dest. aspect ratio"), 3,menuFAspect);
    
    
    
      diaElem *elems[3]={&menu1,&menu2,&menu3};
    if(diaFactoryRun("Resize Type",3,elems))
    {
       *format=(RESWIZ_FORMAT) tformat;
       *source=(RESWIZ_AR) tsource;
       *destination=(RESWIZ_AR) tdestination;
      return 1; 
    }
    return 0;
}
