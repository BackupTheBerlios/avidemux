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
#include "dmx_identify.h"

#define MIN_DELTA_PTS 150 // autofix in ms

static uint8_t Push(uint32_t ftype,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel);
static uint8_t gopDump(FILE *fd,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel,uint32_t nbTracks);

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
typedef struct IndFrame
{
	uint32_t type;
	uint32_t size;
	uint64_t abs;
	uint64_t rel;
	
}IndFrame;
static const char Type[5]={'X','I','P','B','P'};

#define MAX_PUSHED 100

static uint32_t nbPushed,nbGop,nbImage;


static IndFrame frames[MAX_PUSHED];


/*
        Index the incoming mpeg file

*/
uint8_t dmx_indexer(char *mpeg,char *file,uint32_t preferedAudio,uint8_t autosync,uint32_t nbTracks,MPEG_TRACK *tracks)
{
        DIA_working *work;
        dmx_demuxer *demuxer;
        uint64_t syncAbs,syncRel;
        uint64_t lastAbs,lastRel;
        uint64_t pts,dts;
        uint64_t olddts=ADM_NO_PTS;
        uint8_t streamid;   
        uint32_t lastRefPic=0;             
        char *realname=PathCanonize(mpeg);
        FILE *out;        
        DMX_TYPE mpegType;
        uint8_t  mpegTypeChar;
        uint32_t update=0;

        mpegType=dmxIdentify(realname);
        if(mpegType==DMX_MPG_UNKNOWN)
        {
                delete [] realname;
                return 0;
        }
        switch(mpegType)
        {
                case DMX_MPG_ES:
                                demuxer=new dmx_demuxerES;
                                mpegTypeChar='E';
                                break;
                case DMX_MPG_PS:
                		{
                		dmx_demuxerPS *dmx;
                                        dmx=new dmx_demuxerPS(nbTracks,tracks);
                                        demuxer=dmx;
                                        mpegTypeChar='P';
                            }
                                break;
                default : ADM_assert(0);

        }
        
        demuxer->open(realname);

        out=fopen(file,"wt");
        if(!out)
        {
                        printf("\n Error : cannot open index !");
                        delete demuxer;
                        delete [] realname;
                        return 0;
        }
        fprintf(out,"ADMX0003\n");
        fprintf(out,"Type     : %c\n",mpegTypeChar); // ES for now
        fprintf(out,"File     : %s\n",realname);
        fprintf(out,"Image    : %c\n",'P'); // Progressive
        fprintf(out,"Picture  : %04lu x %04lu %05lu fps\n",0,0,0); // width...
        fprintf(out,"Nb Gop   : %05lu \n",0); // width...
        fprintf(out,"Nb Images: %08lu \n",0); // width...
        fprintf(out,"Nb Audio : %02lu\n",nbTracks-1); 
        fprintf(out,"Main aud : %02lu\n",preferedAudio); 
        fprintf(out,"Streams  : ");
        for(int s=0;s<nbTracks;s++)
        {
                if(!s) fprintf(out,"V%04x:%04x ",tracks[0].pid,tracks[0].pes);
                        else fprintf(out,"A%04x:%04x ",tracks[s].pid,tracks[s].pes);                
        }
        fprintf(out,"\n");
        fprintf(out,"# NGop NImg nbImg type:abs:rel:size ...\n"); 

        uint8_t  firstGop=1;
        uint8_t  grabbing=0,seq_found=0;
        uint32_t imageW=0, imageH=0, imageFps=0;
        uint32_t total_frame=0,val;

        uint32_t temporal_ref,ftype;

        work=new DIA_working("Indexing mpeg");

        nbPushed=0;
        nbGop=0;
        nbImage=0;
        grabbing=0;
        printf("*********Indexing started***********\n");
        while(1)
        {
                                if(!demuxer->sync(&streamid,&syncAbs,&syncRel,&pts,&dts)) break;   
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
                                                if(grabbing) continue;
                                                grabbing=1;    
                                                
                                                gopDump(out,demuxer,syncAbs,syncRel,nbTracks);                        
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
                                                        continue;
                                                }
                                                gopDump(out,demuxer,syncAbs,syncRel,nbTracks);                        
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
                                                // skip illegal values
                                                if(ftype<1 || ftype>3)
                                                {
                                                         printf("[Indexer]Met illegal pic at %llu + %llu\n",syncAbs,syncRel);
                                                         continue;
                                                }
#define USING dts
                                                if(USING!=ADM_NO_PTS)
                                                {
                                                        if(olddts!=ADM_NO_PTS )
                                                        {                
                                                                if(USING>=olddts)
                                                                {
                                                                uint64_t delta,deltaRef;
                                                                        delta=USING-olddts;
                                                                        
                                                                        delta/=90;
                                                                      //  printf("Delta :%llu ms\n",delta);
                                                                        // compute what we should be using
                                                                        deltaRef=nbPushed+nbImage-lastRefPic;
                                                                        // in ms
                                                                        deltaRef=(deltaRef*1000*1000)/imageFps;
                                                                        if(abs(delta-deltaRef)>MIN_DELTA_PTS)
                                                                        {
                                                                                printf("Discontinuity %llu %llu!\n",delta,deltaRef);
                                                                        }
                                                                } 
                                                        }
                                                        olddts=USING;
                                                        lastRefPic=nbPushed+nbImage;
                                                }
                                                
                                                
                                                Push(ftype,demuxer,syncAbs,syncRel);
                                             
                                                break;
                                        default:
                                           break;
                                        }
                }
stop_found:
          demuxer->getPos(&lastAbs,&lastRel);
          if(nbPushed)  gopDump(out,demuxer,lastAbs,lastRel,nbTracks);

          fseeko(out,0,SEEK_SET);

        // Update header
        fprintf(out,"ADMX0003\n");
        fprintf(out,"Type     : %c\n",mpegTypeChar); // ES for now
        fprintf(out,"File     : %s\n",realname);
        fprintf(out,"Image    : %c\n",'P'); // Progressive
        fprintf(out,"Picture  : %04lu x %04lu %05lu fps\n",imageW,imageH,imageFps); // width...
        fprintf(out,"Nb Gop   : %05lu \n",nbGop); // width...
        fprintf(out,"Nb Images: %08lu \n",nbImage); // width...

        fclose(out);
        delete work;  

        printf("*********Indexing stopped***********\n");
        printf("Found :%lu gop\n",nbGop);
        printf("Found :%lu image\n",nbImage);

        
          delete demuxer;
          delete [] realname;
          return 1;
}
/**** Push a frame
There is a +- 2 correction when we switch gop
as in the frame header we read 2 bytes
Between frames, the error is self cancelling.

**/
uint8_t Push(uint32_t ftype,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel)
{
                                            
        frames[nbPushed].type=ftype;
        
        if(nbPushed)
        {                
                frames[nbPushed-1].size=demuxer->elapsed();
                if(nbPushed==1) frames[nbPushed-1].size-=2;
                frames[nbPushed].abs=abs;
                frames[nbPushed].rel=rel;        
                demuxer->stamp();
        
        }
        nbPushed++;
        
        ADM_assert(nbPushed<MAX_PUSHED);
        return 1;

}
/*** Pop the whold gop ***/
uint8_t gopDump(FILE *fd,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel,uint32_t nbTracks)
{
        if(!nbPushed && !nbImage) demuxer->stamp();
        if(!nbPushed) return 1;

uint64_t stats[nbTracks];

        frames[nbPushed-1].size=demuxer->elapsed()+2;
        fprintf(fd,"V %03lu %06lu %02lu ",nbGop,nbImage,nbPushed);

        // For each picture Type : abs position : relat position : size
        for(uint32_t i=0;i<nbPushed;i++) 
        {

                fprintf(fd,"%c:%08"LLX",%05lx",
                        Type[frames[i].type],
                        frames[i].abs,
                        frames[i].rel);
                fprintf(fd,",%05lx ",
                        frames[i].size);
        }
        
        fprintf(fd,"\n");

        // audio if any
        //*******************************************
        // Nb image abs_pos audio seen
        // The Nb image is used to compute a drift
        //*******************************************
        if(demuxer->hasAudio() & nbTracks>1)
        {
                demuxer->getStats(stats);
                
                fprintf(fd,"A %lu %"LLX" ",nbImage,abs);
                for(uint32_t i=1;i<nbTracks;i++)
                {
                        fprintf(fd,":%"LLX" ",stats[i]);
                }
                fprintf(fd,"\n");
                
        }

        nbGop++;
        nbImage+=nbPushed;
        nbPushed=0;
                
        frames[0].abs=abs;
        frames[0].rel=rel;
        demuxer->stamp();
        return 1;
        
}
