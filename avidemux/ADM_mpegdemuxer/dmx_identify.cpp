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
#include "dmx_demuxerPS.h"
#include "dmx_demuxerEs.h"
#include "dmx_identify.h"

#define MAX_PROBE (5*1024*1024)
#define MIN_DETECT 100
DMX_TYPE dmxIdentify(char *name)
{
DMX_TYPE ret=DMX_MPG_UNKNOWN;
uint64_t pos;
uint8_t stream;

        uint64_t size;
        uint32_t typeES=0,typePS=0;
        fileParser *parser;
        parser=new fileParser();

        printf("Probing : %s\n",name);
        if(!parser->open(name))
        {
                goto _fnd;
        }
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
