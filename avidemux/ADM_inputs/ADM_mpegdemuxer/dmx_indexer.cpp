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

#include "default.h"
#include <ADM_assert.h>


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "ADM_osSupport/ADM_quota.h"
#include "DIA_idx_pg.h"



#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_osSupport/ADM_debug.h"
#include "dmx_demuxerEs.h"
#include "dmx_demuxerPS.h"
#include "dmx_demuxerTS.h"
#include "dmx_demuxerMSDVR.h"
#include "dmx_identify.h"

#define MIN_DELTA_PTS 150 // autofix in ms
#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_osSupport/ADM_debug.h"

//#define SHOW_PTS

static uint8_t Push(uint32_t ftype,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel);
static uint8_t gopDump(FILE *fd,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel,uint32_t nbTracks);
static uint8_t gopUpdate(dmx_demuxer *demuxer);
uint8_t dumpPts(FILE *fd,dmx_demuxer *demuxer,uint64_t firstPts,uint32_t nbTracks);
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

typedef struct TimeStamp
{
        uint16_t hh,mm,ss,ff;
}TimeStamp;
static const char Type[5]={'X','I','P','B','P'};

#define MAX_PUSHED 200

static uint32_t nbPushed,nbGop,nbImage;
static TimeStamp firstStamp,lastStamp;

static IndFrame frames[MAX_PUSHED];
static uint32_t computeTimeDifference(TimeStamp *f,TimeStamp *l);

/*
        Index the incoming mpeg file

*/
uint8_t dmx_indexer(char *mpeg,char *file,uint32_t preferedAudio,uint8_t autosync,uint32_t nbTracks,MPEG_TRACK *tracks)
{
        DIA_progressIndexing *work;
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
        uint32_t multi=0;
        uint64_t firstPicPTS=ADM_NO_PTS;
        
        


        mpegType=dmxIdentify(realname);
        if(mpegType==DMX_MPG_UNKNOWN)
        {
                delete [] realname;
                return 0;
        }
        switch(mpegType)
        {
               case DMX_MPG_MSDVR:
                                {
                                  dmx_demuxerMSDVR *dmx;
                                  dmx=new dmx_demuxerMSDVR(nbTracks,tracks,0);
                                  demuxer=dmx;
                                  mpegTypeChar='M';
                                  break;
                                }
                case DMX_MPG_TS:
                                {
                                dmx_demuxerTS *dmx;
                                dmx=new dmx_demuxerTS(nbTracks,tracks,0);
                                demuxer=dmx;
                                mpegTypeChar='T';
                                break;
                                }
                case DMX_MPG_ES:
                                demuxer=new dmx_demuxerES;
                                mpegTypeChar='E';
                                break;
                case DMX_MPG_PS:
                		{
                		dmx_demuxerPS *dmx;
                                fileParser    *fp;
                                FP_TYPE       type=FP_PROBE; 
                                        fp=new fileParser;
                                        fp->open(realname,&type);
                                        delete fp;
                                        if(type==FP_APPEND)
                                        {
                                          if(GUI_Question(_("There is several mpeg file, append them ?")))
                                                        multi=1;
                                        }
                                        dmx=new dmx_demuxerPS(nbTracks,tracks,multi);
                                        demuxer=dmx;
                                        mpegTypeChar='P';
                            }
                                break;
                default : ADM_assert(0);

        }
        
        demuxer->open(realname);

        out=qfopen(file,"wt");
        if(!out)
        {
                        printf("\n Error : cannot open index !");
                        delete demuxer;
                        delete [] realname;
                        return 0;
        }
        qfprintf(out,"ADMX0003\n");
        qfprintf(out,"Type     : %c\n",mpegTypeChar); // ES for now
        qfprintf(out,"File     : %s\n",realname);
        qfprintf(out,"Append   : %d\n",multi);
        qfprintf(out,"Image    : %c\n",'P'); // Progressive
        qfprintf(out,"Picture  : %04lu x %04lu %05lu fps\n",0,0,0); // width...
        qfprintf(out,"Nb Gop   : %08lu \n",0); // width...
        qfprintf(out,"Nb Images: %010lu \n",0); // width...
        qfprintf(out,"Nb Audio : %02lu\n",nbTracks-1); 
        qfprintf(out,"Main aud : %02lu\n",preferedAudio); 
        qfprintf(out,"Streams  : ");
        for(int s=0;s<nbTracks;s++)
        {
                if(!s){
			qfprintf(out,"V%04x:%04x ",tracks[0].pid,tracks[0].pes);
		}else{
			qfprintf(out,"A%04x:%04x ",tracks[s].pid,tracks[s].pes);                
		}
        }
        qfprintf(out,"\n");
        qfprintf(out,"# NGop NImg nbImg type:abs:rel:size ...\n"); 

        uint8_t  firstGop=1;
        uint8_t  grabbing=0,seq_found=0;
        uint32_t imageW=0, imageH=0, imageAR=0, imageFps=0;
        uint32_t total_frame=0,val;

        uint32_t temporal_ref,ftype;

        work=new DIA_progressIndexing(mpeg);

        nbPushed=0;
        nbGop=0;
        nbImage=0;
        grabbing=0;
        uint64_t sSize=demuxer->getSize();
        printf("*********Indexing started (%d audio tracks)***********\n",nbTracks);
        while(1)
        {
                                if(!demuxer->sync(&streamid,&syncAbs,&syncRel,&pts,&dts)) break;   
                                
                                //aprintf("\t\tSync : %x at %"LLX"\n",streamid,syncAbs);
                                               /* if(work->update(syncAbs>>16,demuxer->getSize()>>16))
                                                {
                                                        // abort;
                                                        goto stop_found;
                                                }*/
                                if((sSize>>16)>50)
                                {
                                      work->update(syncAbs>>16,demuxer->getSize()>>16,nbImage,
                                              lastStamp.hh,lastStamp.mm,lastStamp.ss);
                                }else
                                {
                                      work->update(syncAbs,demuxer->getSize(),nbImage,
                                               lastStamp.hh,lastStamp.mm,lastStamp.ss);
                                }
//uint32_t done,uint32_t total, uint32_t nbImage, uint32_t hh, uint32_t mm, uint32_t ss);
                                if(work->isAborted()) break;
                                switch(streamid)
                                        {
                                        /* Useless apparently
                                                        case 0xB9: // sequence end
                                                        printf("\n End seq\n");
                                                        goto stop_found;
                                                        break;
                                        */
                                        case 0xB3: // sequence start
                                                aprintf("Seq %d\n",nbGop);
                                                if(grabbing) continue;
                                                grabbing=1;    
                                                
                                                gopDump(out,demuxer,syncAbs,syncRel,nbTracks);                        
                                                if(seq_found)
                                                {
                                                        demuxer->forward(8);
                                                        break;
                                                }
                                                // Our firt frame is here
                                                // Important to initialize the mpeg decoder !
                                                frames[0].abs=syncAbs;
                                                frames[0].rel=syncRel;
                                                //
                                                seq_found=1;
                                                val=demuxer->read32i();
                                                imageW=val>>20;
                                                imageW=((imageW+15)&~15);
                                                imageH= (((val>>8) & 0xfff)+15)& ~15;
						imageAR=(val>>4)&0xf;
                                                imageFps= FPS[val & 0xf];
                                                demuxer->forward(4);
                                                break;
                                        case 0xb8: // GOP
                                                aprintf("GOP %d\n",nbGop);
#ifdef SHOW_PTS
                                                if(pts!=ADM_NO_PTS)
                                                {
                                                qfprintf(out,"# %lu\n",pts/90);
                                                }
#endif
                                                uint32_t gop;   
                                                if(!seq_found) continue;
                                                if(grabbing) 
                                                {         
                                                        gopUpdate(demuxer);                                                 
                                                        continue;
                                                }
                                                gopDump(out,demuxer,syncAbs,syncRel,nbTracks);                        
                                                gopUpdate(demuxer);
                                                firstGop=0;                                                
                                                break;
                                        case 0x00 : // picture
                                               
                                                 aprintf("pic \n");
                                                if(!seq_found)
                                                { 
                                                        continue;
                                                        printf("No sequence start yet, skipping..\n");
                                                }
                                                if(firstPicPTS==ADM_NO_PTS && pts!=ADM_NO_PTS)
                                                        firstPicPTS=pts;
                                                grabbing=0;
                                                total_frame++;
                                                val=demuxer->read16i();
                                                temporal_ref=val>>6;
                                                ftype=7 & (val>>3);
                                                //aprintf("Temporal ref:%lu\n",temporal_ref);
                                                // skip illegal values
                                                if(ftype<1 || ftype>3)
                                                {
                                                         printf("[Indexer]Met illegal pic at %"LLX" + %"LLX"\n",
                                                                        syncAbs,syncRel);
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

         dumpPts(out,demuxer,firstPicPTS,nbTracks);

         switch(imageAR)
         {
           case 1: 	qfprintf(out,"# Video Aspect Ratio : %s\n", "1:1" );break;
           case 2: 	qfprintf(out,"# Video Aspect Ratio : %s\n", "4:3" );break;
           case 3: 	qfprintf(out,"# Video Aspect Ratio : %s\n", "16:9" );break;
           default:
              printf("imageAR=%u\n",imageAR);
              GUI_Error_HIG(_("Can't determine aspect ratio"),NULL);
	}

        /* Now update......... */
          fseeko(out,0,SEEK_SET);
        // Update if needed
        uint32_t compfps,delta=computeTimeDifference(&firstStamp,&lastStamp);

        delta=delta/1000; // in second
        if(delta)
        {
                 compfps= (1000*nbImage)/delta;    // 3 Million images should be enough, no overflow                
        }
        else
        {
                compfps=imageFps;
        }

        // Detect film (i.e. NTSC with computed fps close to 24)
        if(imageFps==29970 || imageFps==30000)
        {
                if(compfps>23800 && compfps < 24200) imageFps=23976;
        }
        // Detect interlaced vs progressive
        // If field encoded, the average fps is about twice as theoritical fps
        char type='P';
        float err;

        err=imageFps*2;
        err-=compfps;
        err*=100;
        err/=imageFps*2;
        if(err<0) err=-err;
        printf("%lu :%lu / %lu , %f\n",imageFps,imageFps*2,compfps,err);

        if(err<10) 
        {
                type='I';
                printf("Seems to be field encoded\n");
        }
        else
        {
                printf("Seems to be frame encoded\n");
        }

        // Now dump the delta PTS
        // *****************Update header*************
        qfprintf(out,"ADMX0003\n");
        qfprintf(out,"Type     : %c\n",mpegTypeChar); // ES for now
        qfprintf(out,"File     : %s\n",realname);
        qfprintf(out,"Append   : %d\n",multi);
        qfprintf(out,"Image    : %c\n",type); // Progressive
        qfprintf(out,"Picture  : %04lu x %04lu %05lu fps\n",imageW,imageH,imageFps); // width...
        qfprintf(out,"Nb Gop   : %08lu \n",nbGop); // width...
        qfprintf(out,"Nb Images: %010lu \n",nbImage); // width...

        qfclose(out);
        delete work;  

        printf("*********Indexing stopped***********\n");
        printf("Found       :%lu gop\n",nbGop);
        printf("Found       :%lu image\n",nbImage);                
        printf("Average fps :%lu /1000 fps\n",compfps);

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
        //aprintf("\tpushed %d %"LLX"\n",nbPushed,abs);
        nbPushed++;
        
        ADM_assert(nbPushed<MAX_PUSHED);
        return 1;

}
uint8_t dumpPts(FILE *fd,dmx_demuxer *demuxer,uint64_t firstPts,uint32_t nbTracks)
{
uint64_t stats[nbTracks],p;
double d;

        if(!demuxer->getAllPTS(stats)) return 0;
        qfprintf(fd,"# Video 1st PTS : %07u\n",firstPts);
        if(firstPts==ADM_NO_PTS) return 1;
        for(int i=1;i<nbTracks;i++)
        {
                p=stats[i];
                if(p==ADM_NO_PTS)
                {
                        qfprintf(fd,"# track %d no pts\n",i);
                }
                else
                {
                        
                        d=firstPts; // it is in 90 khz tick
                        d-=stats[i];
                        d/=90.;
                        qfprintf(fd,"# track %d PTS : %07u ",i,stats[i]);
                        qfprintf(fd," delta=%04d ms\n",(int)d);
                }

        }
        return 1;
}
/*** Pop the whold gop ***/
uint8_t gopDump(FILE *fd,dmx_demuxer *demuxer,uint64_t abs,uint64_t rel,uint32_t nbTracks)
{
        if(!nbPushed && !nbImage) demuxer->stamp();
        if(!nbPushed) return 1;

uint64_t stats[nbTracks];

        frames[nbPushed-1].size=demuxer->elapsed()+2;
        qfprintf(fd,"V %03u %06u %02u ",nbGop,nbImage,nbPushed);

        // For each picture Type : abs position : relat position : size
        for(uint32_t i=0;i<nbPushed;i++) 
        {

                qfprintf(fd,"%c:%08"LLX",%05x",
                        Type[frames[i].type],
                        frames[i].abs,
                        frames[i].rel);
                qfprintf(fd,",%05x ",
                        frames[i].size);
        }
        
        qfprintf(fd,"\n");

        // audio if any
        //*******************************************
        // Nb image abs_pos audio seen
        // The Nb image is used to compute a drift
        //*******************************************
        if(demuxer->hasAudio() & nbTracks>1)
        {
                demuxer->getStats(stats);
                
                qfprintf(fd,"A %u %"LLX" ",nbImage,abs);
                for(uint32_t i=1;i<nbTracks;i++)
                {
                        qfprintf(fd,":%"LLX" ",stats[i]);
                }
                qfprintf(fd,"\n");
                
        }
        // Print some gop stamp infos, does not hurt
        qfprintf(fd,"# Timestamp %02d:%02d:%02d,%03d\n",lastStamp.hh,lastStamp.mm,lastStamp.ss,lastStamp.ff);
        nbGop++;
        nbImage+=nbPushed;
        nbPushed=0;
                
        frames[0].abs=abs;
        frames[0].rel=rel;
        demuxer->stamp();
        return 1;
        
}
uint8_t gopUpdate(dmx_demuxer *demuxer)
{
uint32_t a1,a2,a3,a4;
uint32_t hh,mm,ss,ff;
TimeStamp *ts;

        a1=demuxer->read8i();
        a2=demuxer->read8i();
        a3=demuxer->read8i();
        a4=demuxer->read8i();
        hh=(a1>>2)&0x1f;
        mm=((a1&3)<<4)+(a2>>4);
        ss=((a2&7)<<3)+(a3>>5);
        ff=((a3&0x1f)<<1)+(a4>>7);
        if(!nbGop)  ts=&firstStamp;
                else ts=&lastStamp;
        
        ts->hh=hh;
        ts->mm=mm;
        ts->ss=ss;
        ts->ff=ff;
        if(!nbGop) memcpy(&lastStamp,&firstStamp,sizeof(firstStamp));
        return 1;
}
uint32_t computeTimeDifference(TimeStamp *f,TimeStamp *l)
{
        uint32_t h,m,s,ms,r=0,result=0;
#define DOIT(x,next) if(l->x < f->x) \
                        { \
                                r=(next-(f->x-l->x));\
                                result-=1; \
                        }\
                         else r=(l->x-f->x);\
                        result=result*next+r;

#define PRINT(z,x) printf(#z"%02d:%02d:%02d\n",x->hh,x->mm,x->ss,x->ff);
        DOIT(hh,0);                        
        DOIT(mm,60);        
        DOIT(ss,60);        
        DOIT(ff,1000);
#if 0
        PRINT(first,f);         
        PRINT(last,l);
        
#endif
        printf("Time difference:%lu s\n",result/1000);
        return result;

}
//
