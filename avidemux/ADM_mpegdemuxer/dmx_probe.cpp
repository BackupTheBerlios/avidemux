/***************************************************************************
                        Probe for a stream                                              
                             
    
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


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_dialog/DIA_working.h"



#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_toolkit/ADM_debug.h"

#include "dmx_demuxerPS.h"
#include "dmx_identify.h"
#include "dmx_probe.h"
#include "ADM_dialog/DIA_busy.h"

#define MAX_PROBE (10*1024*1024LL) // Scans the 4 first meg
#define MIN_DETECT (10*1024) // Need this to say the stream is present

uint8_t dmx_probe(char *file, DMX_TYPE  *type, uint32_t *nbTracks,MPEG_TRACK **tracks)
{
uint8_t dummy[10];
uint64_t seen[256],abs,rel;
int     audio,video;

        printf("Probing %s for streams...\n",file);
        *type=dmxIdentify(file);
        if(*type==DMX_MPG_ES)
        {
                *nbTracks=1;
                *tracks=new MPEG_TRACK;
                (*tracks)->pes=0xE0;
                (*tracks)->pid=0;
                printf("It is ES, no need to look for audio\n");
                return 1;
        }
        if(*type!=DMX_MPG_PS) return 0;

        // It is mpeg PS
        // Create a fake demuxer, set a probe limite and collect info for all streams found
        dmx_demuxerPS *demuxer;
        MPEG_TRACK    pseudo;

               pseudo.pes=0xfe;
               pseudo.pid=0;
               demuxer=new dmx_demuxerPS(256,&pseudo);
               if(!demuxer->open(file))
                {
                        delete demuxer;
                        printf("Cannot open file file demuxer (%s)\n",file);
                        return 0;
                }
                DIA_StartBusy();
                demuxer->setProbeSize(MAX_PROBE);
                demuxer->read(dummy,1);
                demuxer->getStats(seen);
                demuxer->getPos( &abs,&rel);
                abs>>=20;
                printf("Stopped at %"LLU" MB\n",abs);

                delete demuxer;
                DIA_StopBusy();
        // Now analyze...
        video=0;
        // Take the first video track suitable
        for(int i=0xE0;i<0xE9;i++)
        {
                if(seen[i]>MIN_DETECT)
                {
                        video=i;
                        break;
                }
        }
        if(!video)
        {
                printf("Cannot find any video stream\n");
                return 0;
        }
        
        audio=0;
#if 1
        for(int i=0;i<256;i++)
        {
                if(seen[i]) printf("%x: there is something %lu kb\n",i,seen[i]>>10);
        }
#endif
        // 1 count how much audio we have
        for(int i=0;i<9;i++) if(seen[i]>MIN_DETECT) audio++;
        for(int i=0xc0;i<0xc9;i++) if(seen[i]>MIN_DETECT) audio++;
        for(int i=0xA0;i<0xA9;i++) if(seen[i]>MIN_DETECT) audio++;

        *nbTracks=audio+1;      
        *tracks=new MPEG_TRACK[*nbTracks];
        
        memset(*tracks,0,(audio+1)*sizeof(MPEG_TRACK));
        (*tracks)[0].pes=video;
        audio=1;
#define DOME {(*tracks)[audio].pes=i;audio++;}
        for(int i=0;i<9;i++) if(seen[i]>MIN_DETECT) DOME;
        for(int i=0xc0;i<0xc9;i++) if(seen[i]>MIN_DETECT) DOME;
        for(int i=0xA0;i<0xA9;i++) if(seen[i]>MIN_DETECT) DOME;

        printf("Found video as %x, and %d audio tracks\n",video,audio-1);
        return 1;
}
