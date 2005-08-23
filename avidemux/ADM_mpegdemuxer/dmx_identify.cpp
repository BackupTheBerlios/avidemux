/***************************************************************************
                          PS demuxer
                             -------------------
                
    copyright            : (C) 2005 by mean
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include <ADM_assert.h>

#include "dmx_io.h"
#include "dmx_demuxerTS.h"
#include "dmx_demuxerPS.h"
#include "dmx_demuxerEs.h"
#include "dmx_identify.h"

#define MAX_PROBE (5*1024*1024)
#define MIN_DETECT 66

static uint8_t probeTs(fileParser *parser);

DMX_TYPE dmxIdentify(char *name)
{
DMX_TYPE ret=DMX_MPG_UNKNOWN;
uint64_t pos;
uint8_t stream;
FP_TYPE fp=FP_DONT_APPEND;
        uint64_t size;
        uint32_t typeES=0,typePS=0;
        fileParser *parser;
        parser=new fileParser();

        printf("Probing : %s\n",name);
        if(!parser->open(name,&fp))
        {
                goto _fnd;
        }
        // Try to see if it is a TS:
        if(probeTs(parser))
                {
                        delete parser;
                        return DMX_MPG_TS;
                }
        parser->setpos(0);
        while(1)
        {
                parser->getpos(&pos);
                if(pos>MAX_PROBE) goto _nalyze;
                if(!parser->sync(&stream)) goto _nalyze;

#define INSIDE(x,y) (stream>=x && stream<y)
                if(stream==00 || stream==0xb3 || stream==0xb8) 
                {                        
                        typeES++;
                        continue;
                }

                if( INSIDE(0xE0,0xE9)  ) 
                        typePS++;

        }
_nalyze:
                if(typeES>MIN_DETECT)
                {
                        if(typePS>MIN_DETECT)
                        {
                                printf("%s looks like Mpeg PS (%lu/%lu)\n",name,typeES,typePS);
                                ret=DMX_MPG_PS;
                        }
                        else
                        {
                                printf("%s looks like Mpeg ES (%lu/%lu)\n",name,typeES,typePS);
                                ret=DMX_MPG_ES;
                        }
                }
                else
                {
                        // Try TS here
                        printf("Cannot identify %s as mpeg (%lu/%lu)\n",name,typeES,typePS);

                }
     
                           
_fnd:
                delete parser;
                return ret;
}
uint8_t probeTs(fileParser *parser)
{
uint64_t size,pos,left;
uint32_t count,discarded;
        // Try to sync on a 0x47 and verify we have several of them in a raw
        size=parser->getSize();
        while(1)
        {
                parser->getpos(&pos);
                if(size-pos<TS_PACKET_SIZE || pos> 100*1024)
                {
                        return 0;
                }
                left=size-pos;
                count=0;
                discarded=0;
                while(parser->read8i()!=0x47 && left > TS_PACKET_SIZE)
                        {
                                left--;
                                discarded++;
                                if(discarded>TS_PACKET_SIZE*3) return 0;
                        }
                if(left<=TS_PACKET_SIZE) return 0;
                parser->getpos(&pos);

                parser->forward(TS_PACKET_SIZE-1);
                if(parser->read8i()!=0x47) goto loop;
                parser->forward(TS_PACKET_SIZE-1);
                if(parser->read8i()!=0x47) goto loop;
                parser->forward(TS_PACKET_SIZE-1);
                if(parser->read8i()!=0x47) goto loop;
                // It is a TS file:
                return 1;
loop:
                parser->setpos(pos);
        }

        return 1;
}
// EOF
