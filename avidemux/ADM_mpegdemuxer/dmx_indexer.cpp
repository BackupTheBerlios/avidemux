/***************************************************************************
                        2nd gen indexer                                                 
                             
    
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
#include "dmx_demuxerEs.h"
#include "dmx_demuxerPS.h"


static uint8_t Push(uint32_t ftype,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel);
static uint8_t gopDump(FILE *fd,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel);

uint8_t dmx_indexer(char *mpeg,char *file);

static const uint32_t FPS[16]={
                0,                      // 0
                23976,          // 1 (23.976 fps) - FILM
                24000,          // 2 (24.000 fps)
                25000,          // 3 (25.000 fps) - PAL
                29970,          // 4 (29.970 fps) - NTSC
                30000,          // 5 (30.000 fps)
                50000,          // 6 (50.000 fps) - PAL noninterlaced
                59940,          // 7 (59.940 fps) - NTSC noninterlaced
                60000,          // 8 (60.000 fps)
                0,                      // 9
                0,                      // 10
                0,                      // 11
                0,                      // 12
                0,                      // 13
                0,                      // 14
                0                       // 15
        };

static const char Type[5]={'X','I','P','B','D'};

#define MAX_PUSHED 100

static uint32_t nbPushed,nbGop,nbImage;

static uint32_t frameType[MAX_PUSHED],frameSize[MAX_PUSHED];
static uint64_t frameAbs[MAX_PUSHED],frameRel[MAX_PUSHED];


/*
        Index the incoming mpeg file

*/
uint8_t dmx_indexer(char *mpeg,char *file)
{
        DIA_working *work;
        dmx_demuxer *demuxer;
        uint64_t syncAbs,syncRel;
        uint64_t lastAbs,lastRel;
        uint8_t streamid;                
        char *realname=PathCanonize(mpeg);
        FILE *out;        
        
        uint32_t update=0;

#if 0       
        demuxer=new dmx_demuxerES;
        demuxer->open(realname);
#else
        demuxer=new dmx_demuxerPS(0xE0);
        demuxer->open(realname);

#endif
        out=fopen(file,"wt");
        if(!out)
        {
                        printf("\n Error : cannot open index !");
                        delete demuxer;
                        delete [] realname;
                        return 0;
        }
        fprintf(out,"ADMX0003\n");
        fprintf(out,"Type     : %c\n",'E'); // ES for now
        fprintf(out,"File     : %s\n",realname);
        fprintf(out,"Image    : %c\n",'P'); // Progressive
        fprintf(out,"Picture  : %04lu x %04lu %05lu fps\n",0,0,0); // width...
        fprintf(out,"Nb Gop   : %05lu \n",0); // width...
        fprintf(out,"Nb Images: %05lu \n",0); // width...
        fprintf(out,"Nb Audio : %02lu\n",0); 
        fprintf(out,"Streams  : V0000:0000\n"); 
        
        fprintf(out,"# NGop NImg nbImg type:abs:rel:size ...\n"); 

        uint8_t  firstGop=1;
        uint8_t  grabbing=0,seq_found=0;
        uint32_t imageW, imageH, imageFps;
        uint32_t total_frame=0,val;

        uint32_t temporal_ref,ftype;

        work=new DIA_working("Indexing mpeg");

        nbPushed=0;
        nbGop=0;
        nbImage=0;
        
        while(1)
        {
                                if(!demuxer->sync(&streamid,&syncAbs,&syncRel)) break;                                
                                update++;
                                if(update>100)
                                        {
                                                if(work->update(syncAbs>>16,demuxer->getSize()>>16))
                                                {
                                                        // abort;
                                                        goto stop_found;
                                                }
                                                update=0;
                                        }
                                switch(streamid)
                                        {
                                        /* Useless apparently
                                                        case 0xB9: // sequence end
                                                        printf("\n End seq\n");
                                                        goto stop_found;
                                                        break;
                                        */
                                        case 0xB3: // sequence start
                                                
                                                grabbing=1;    
                                                if(firstGop)    
                                                {                                                   
                                                        firstGop=0;
                                                }
                                                gopDump(out,demuxer,syncAbs,syncRel);                        
                                                if(seq_found)
                                                {
                                                        demuxer->forward(8);
                                                        break;
                                                }
                                                seq_found=1;
                                                val=demuxer->read32i();
                                                imageW=val>>20;
                                                imageW=((imageW+15)&~15);
                                                imageH= (((val>>8) & 0xfff)+15)& ~15;
                                                imageFps= FPS[val & 0xf];
                                                demuxer->forward(4);
                                                break;
                                        case 0xb8: // GOP
                                                //      printf("GOP\n");
                                                uint32_t gop;   
                                                if(!seq_found) continue;
                                                if(grabbing) 
                                                {
                                                        grabbing=0;        
                                                        continue;
                                                }
                                                gopDump(out,demuxer,syncAbs,syncRel);                        
                                                firstGop=0;
                                                demuxer->forward(3);    
                                                gop=demuxer->read8i();
                                                
                                                break;
                                        case 0x00 : // picture
                                               
                                                if(!seq_found) continue;
                                                grabbing=0;
                                                total_frame++;
                                                val=demuxer->read16i();
                                                temporal_ref=val>>6;
                                                ftype=7 & (val>>3);
                                                Push(ftype,demuxer,syncAbs,syncRel);
                                             
                                                break;
                                        default:
                                           break;
                                        }
                }
stop_found:
          demuxer->getPos(&lastAbs,&lastRel);
          if(nbPushed)  gopDump(out,demuxer,lastAbs,lastRel);

          fseeko(out,0,SEEK_SET);

        // Update header
        fprintf(out,"ADMX0003\n");
        fprintf(out,"Type     : %c\n",'E'); // ES for now
        fprintf(out,"File     : %s\n",realname);
        fprintf(out,"Image    : %c\n",'P'); // Progressive
        fprintf(out,"Picture  : %04lu x %04lu %05lu fps\n",imageW,imageH,imageFps); // width...
        fprintf(out,"Nb Gop   : %05lu \n",nbGop); // width...
        fprintf(out,"Nb Images: %05lu \n",nbImage); // width...
        fprintf(out,"Nb Audio : %02lu\n",0); 
        fprintf(out,"Streams  : V0000:0000\n"); 

          fclose(out);
          delete work;
          delete demuxer;
          delete [] realname;
          return 1;
}
/**** Push a frame**/
uint8_t Push(uint32_t ftype,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel)
{
                                            
        frameType[nbPushed]=ftype;
        
        if(nbPushed)
        {
                
                frameSize[nbPushed-1]=demuxer->elapsed();
                frameAbs[nbPushed]=abs;
                frameRel[nbPushed]=rel;        
                demuxer->stamp();
        
        }
        nbPushed++;
        
        ADM_assert(nbPushed<MAX_PUSHED);
        return 1;

}
/*** Pop the whold gop ***/
uint8_t gopDump(FILE *fd,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel)
{

        if(!nbPushed) return 1;

        frameSize[nbPushed-1]=demuxer->elapsed();
        fprintf(fd,"V %03lu %06lu %02lu ",nbGop,nbImage,nbPushed);

        // For each picture Type : abs position : relat position : size
        for(uint32_t i=0;i<nbPushed;i++) 
        {
                fprintf(fd,"%c:%08llx,%05lx",
                        Type[frameType[i]],
                        frameAbs[i],
                        frameRel[i]);
                fprintf(fd,",%05lx ",
                        frameSize[i]);
        }
        
        fprintf(fd,"\n");

        nbGop++;
        nbImage+=nbPushed;
        nbPushed=0;
                
        frameAbs[0]=abs;
        frameRel[0]=rel;
        demuxer->stamp();
        return 1;
        
}
