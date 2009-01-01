/***************************************************************************
    copyright            : (C) 2007 by mean
    email                : fixounet@free.fr
    
      See lavformat/flv[dec/env].c for detail
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
#include "ADM_ps.h"
#include "ADM_demuxerInternal.h"
#include "fourcc.h"





ADM_DEMUXER_BEGIN( psHeader,
                    1,0,0,
                    "ps",
                    "mpeg ps demuxer plugin (c) Mean 2007/2008"
                );


/**
    \fn Probe
*/
uint8_t   psIndexer(const char *file);
extern "C"  uint32_t         probe(uint32_t magic, const char *fileName)
{
    printf(" [PS Demuxer] \n");
    psIndexer(fileName);

   return 0;
}
