
/***************************************************************************
  \file DIA_plugins.cpp
  (C) 2008 Mean Fixounet@free.fr 
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
#include "ADM_default.h"
#include "DIA_factory.h"
/* Functions we need to get infos */
uint32_t ADM_ad_getNbFilters(void);
bool     ADM_ad_getFilterInfo(int filter, const char **name, uint32_t *major,uint32_t *minor,uint32_t *patch);
/* /Functions */
/**
        \fn DIA_pluginsInfo
        \brief Display loaded plugin infos        

*/
uint8_t DIA_pluginsInfo(void)
{
    uint32_t aNbPlugin=ADM_ad_getNbFilters();
    printf("[Audio Plugins] Found %u plugins\n",aNbPlugin);
    for(int i=0;i<aNbPlugin;i++)
    {
        const char *name;
        uint32_t major,minor,patch;
            ADM_ad_getFilterInfo(i, &name,&major,&minor,&patch);
            printf("[Audio Plugins] %s version :%2d.%2d.%2d\n",name,major,minor,patch);

    }
    return 1;
}
