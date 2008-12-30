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

#include "dmxPSPacket.h"

ADM_DEMUXER_BEGIN( psHeader,
                    1,0,0,
                    "ps",
                    "mpeg ps demuxer plugin (c) Mean 2007/2008"
                );

/**
    \fn Probe
*/

extern "C"  uint32_t         probe(uint32_t magic, const char *fileName)
{
    printf(" [PS Demuxer] \n");
    psPacket *pkt=new psPacket;
    pkt->open(fileName,false);
    uint8_t buffer[50*1024];
    uint8_t pid;
    uint32_t packetSize;
    uint64_t pts,dts;
    int count=0;
    while(true==pkt->getPacket(50*1024, &pid, &packetSize,&pts,&dts,buffer) && count < 100)
    {
        printf("Packet pid :%x size :%04d pts:%08"LLD" dts:%08"LLD"\n",pid,packetSize,pts,dts);
        count++;
    }
    delete pkt;
   return 50;
}
