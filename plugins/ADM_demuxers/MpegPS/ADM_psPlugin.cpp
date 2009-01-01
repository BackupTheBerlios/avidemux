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
char index[strlen(fileName)+4];
int count=0;
    sprintf(index,"%s.idx",fileName);
again:    
    if(ADM_fileExist(index)) 
    {
        printf(" [PS Demuxer] There is an index for that file \n");
        FILE *f=fopen(index,"rt");
        char signature[10];
        fread(signature,4,1,f);
        signature[4]=0;
        fclose(f);
        if(!strcmp(signature,"PSD1")) return 50;
        printf("[PsDemuxer] Not a valid index\n");
        return false;
    }
    if(count) return false;
    printf("[PSDemuxer] Creating index..\n");
    count++;
    if(true==psIndexer(fileName)) goto again;
    printf("[PSDemuxer] Failed..\n");
   return 0;
}
