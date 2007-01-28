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

#include "dmx_indexer_internal.h"

static const char Type[5]={'X','I','P','B','P'};

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





static uint8_t Push(runData *run,uint32_t ftype,uint64_t abs,uint64_t rel);
static uint8_t gopDump(runData *run,uint64_t abs,uint64_t rel,uint32_t nbTracks);
static uint8_t gopUpdate(runData *run);
uint8_t dumpPts(FILE *fd,dmx_demuxer *demuxer,uint64_t firstPts,uint32_t nbTracks);
/**** Push a frame
There is a +- 2 correction when we switch gop
as in the frame header we read 2 bytes
Between frames, the error is self cancelling.

**/
uint8_t Push(runData *run,uint32_t ftype,uint64_t abs,uint64_t rel)
{
                                            
        run->frames[run->nbPushed].type=ftype;
        
        if(run->nbPushed)
        {                
                run->frames[run->nbPushed-1].size=run->demuxer->elapsed();
                if(run->nbPushed==1) run->frames[run->nbPushed-1].size-=2;
                run->frames[run->nbPushed].abs=abs;
                run->frames[run->nbPushed].rel=rel;        
                run->demuxer->stamp();
        
        }
        //aprintf("\tpushed %d %"LLX"\n",nbPushed,abs);
        run->nbPushed++;
        
        ADM_assert(run->nbPushed<MAX_PUSHED);
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
uint8_t gopDump(runData *run,uint64_t abs,uint64_t rel,uint32_t nbTracks)
{
  dmx_demuxer *demuxer=run->demuxer;
        if(!run->nbPushed && !run->nbImage) demuxer->stamp();
        if(!run->nbPushed) return 1;

uint64_t stats[nbTracks];

        run->frames[run->nbPushed-1].size=demuxer->elapsed()+2;
        qfprintf(run->fd,"V %03u %06u %02u ",run->nbGop,run->nbImage,run->nbPushed);

        // For each picture Type : abs position : relat position : size
        for(uint32_t i=0;i<run->nbPushed;i++) 
        {

                qfprintf(run->fd,"%c:%08"LLX",%05x",
                        Type[run->frames[i].type],
                        run->frames[i].abs,
                        run->frames[i].rel);
                qfprintf(run->fd,",%05x ",
                        run->frames[i].size);
        }
        
        qfprintf(run->fd,"\n");

        // audio if any
        //*******************************************
        // Nb image abs_pos audio seen
        // The Nb image is used to compute a drift
        //*******************************************
        if(demuxer->hasAudio() & nbTracks>1)
        {
                demuxer->getStats(stats);
                
                qfprintf(run->fd,"A %u %"LLX" ",run->nbImage,abs);
                for(uint32_t i=1;i<nbTracks;i++)
                {
                        qfprintf(run->fd,":%"LLX" ",stats[i]);
                }
                qfprintf(run->fd,"\n");
                
        }
        // Print some gop stamp infos, does not hurt
        qfprintf(run->fd,"# Timestamp %02d:%02d:%02d,%03d\n",run->lastStamp.hh,run->lastStamp.mm,run->lastStamp.ss,run->lastStamp.ff);
        run->nbGop++;
        run->nbImage+=run->nbPushed;
        run->nbPushed=0;
                
        run->frames[0].abs=abs;
        run->frames[0].rel=rel;
        demuxer->stamp();
        return 1;
        
}
uint8_t gopUpdate(runData *run)
{
uint32_t a1,a2,a3,a4;
uint32_t hh,mm,ss,ff;
TimeStamp *ts;
dmx_demuxer *demuxer=run->demuxer;

        a1=demuxer->read8i();
        a2=demuxer->read8i();
        a3=demuxer->read8i();
        a4=demuxer->read8i();
        hh=(a1>>2)&0x1f;
        mm=((a1&3)<<4)+(a2>>4);
        ss=((a2&7)<<3)+(a3>>5);
        ff=((a3&0x1f)<<1)+(a4>>7);
        if(!run->nbGop)  ts=&(run->firstStamp);
                else ts=&(run->lastStamp);
        
        ts->hh=hh;
        ts->mm=mm;
        ts->ss=ss;
        ts->ff=ff;
        if(!run->nbGop) memcpy(&(run->lastStamp),&(run->firstStamp),sizeof(run->firstStamp));
        return 1;
}
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/**
        \fn mainLoopMpeg2
        \brief Main indexing function for mpeg1/mpeg2 payLoad
*/
uint8_t mainLoopMpeg2(runData *run)
{
dmx_demuxer *demuxer=run->demuxer;
uint64_t syncAbs,syncRel;
uint8_t streamid;   
uint32_t temporal_ref,ftype,val;
uint64_t pts,dts;

      run->frames=new IndFrame[MAX_PUSHED];

      while(1)
      {

              if(!demuxer->sync(&streamid,&syncAbs,&syncRel,&pts,&dts)) return 0;   
              if((run->totalFileSize>>16)>50)
              {
                    run->work->update(syncAbs>>16,run->totalFileSize>>16,
                               run->nbImage,run->lastStamp.hh,run->lastStamp.mm,run->lastStamp.ss);
              }else
              {
                    run->work->update(syncAbs,run->totalFileSize,run->nbImage,
                            run->lastStamp.hh,run->lastStamp.mm,run->lastStamp.ss);
              }
              if(run->work->isAborted()) return 0;
              switch(streamid)
                      {
                      case 0xB3: // sequence start
                              aprintf("Seq %d\n",run->nbGop);
                              if(run->grabbing) continue;
                              run->grabbing=1;    
                              
                              gopDump(run,syncAbs,syncRel,run->nbTrack);
                              if(run->seq_found)
                              {
                                      demuxer->forward(8);  // Ignore
                                      continue;
                              }
                              // Our firt frame is here
                              // Important to initialize the mpeg decoder !
                              run->frames[0].abs=syncAbs;
                              run->frames[0].rel=syncRel;
                              //
                              run->seq_found=1;
                              val=demuxer->read32i();
                              run->imageW=val>>20;
                              run->imageW=((run->imageW+15)&~15);
                              run->imageH= (((val>>8) & 0xfff)+15)& ~15;
                              run->imageAR=(val>>4)&0xf;
                              run->imageFPS= FPS[val & 0xf];
                              demuxer->forward(4);
                              break;
                      case 0xb8: // GOP
                              aprintf("GOP %d\n",run->nbGop);
#ifdef SHOW_PTS
                              if(pts!=ADM_NO_PTS)
                              {
                              qfprintf(run->fd,"# %lu\n",pts/90);
                              }
#endif
                              uint32_t gop;   
                              if(!run->seq_found) continue;
                              if(run->grabbing) 
                              {         
                                      gopUpdate(run);
                                      continue;;
                              }
                              gopDump(run,syncAbs,syncRel,run->nbTrack);
                              gopUpdate(run);
                              
                              break;
                      case 0x00 : // picture
                            
                              aprintf("pic \n");
                              if(!run->seq_found)
                              { 
                                      continue;
                                      printf("No sequence start yet, skipping..\n");
                              }
                              if(run->firstPicPTS==ADM_NO_PTS && pts!=ADM_NO_PTS)
                                      run->firstPicPTS=pts;
                              run->grabbing=0;
                              run->totalFrame++;
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
#if 0
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
                                                      deltaRef=nbPushed+nbImage-run->lastRefPic;
                                                      // in ms
                                                      deltaRef=(deltaRef*1000*1000)/run->imageFPS;
                                                      if(abs(delta-deltaRef)>MIN_DELTA_PTS)
                                                      {
                                                              printf("Discontinuity %llu %llu!\n",delta,deltaRef);
                                                      }
                                              } 
                                      }
                                      olddts=USING;
                                      run->lastRefPic=nbPushed+nbImage;
                              }
#endif
                              
                              Push(run,ftype,syncAbs,syncRel);
                          
                              break;
                      default:
                        break;
                      }
      }
                      return 1; 
}
/**
      \fn endLoopMpeg2
      \brief do cleanup and purge non processed data at the end of the mpeg2 stream
*/
void endLoopMpeg2(runData *run)
{
  uint64_t lastAbs,lastRel;
          run->demuxer->getPos(&lastAbs,&lastRel);
          if(run->nbPushed)  gopDump(run,lastAbs,lastRel,run->nbTrack);
          dumpPts(run->fd,run->demuxer,run->firstPicPTS,run->nbTrack); 
          
          if(run->frames) delete [] run->frames;
          run->frames=NULL;
}
//
