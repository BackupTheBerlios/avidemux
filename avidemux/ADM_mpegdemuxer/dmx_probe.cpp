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
#include "dmx_demuxerTS.h"
#include "dmx_identify.h"
#include "dmx_probe.h"
#include "ADM_dialog/DIA_busy.h"
#include "ADM_audio/ADM_mp3info.h"
#include "ADM_audio/ADM_a52info.h"
#include "ADM_audio/ADM_dcainfo.h"



#define MAX_PROBE (10*1024*1024LL) // Scans the 4 first meg
#define MIN_DETECT (10*1024) // Need this to say the stream is present

static uint8_t dmx_probePS(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks);
static uint8_t dmx_probeTS(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks);
static uint8_t dmx_probeTSBruteForce(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks);

uint8_t dmx_probe(char *file, DMX_TYPE  *type, uint32_t *nbTracks,MPEG_TRACK **tracks)
{

        printf("Probing %s for streams...\n",file);
        *type=dmxIdentify(file);
        switch(*type)
        {
        case DMX_MPG_ES:
                {
                *nbTracks=1;
                *tracks=new MPEG_TRACK;
                (*tracks)->pes=0xE0;
                (*tracks)->pid=0;
                printf("It is ES, no need to look for audio\n");
                return 1;
                }
        case DMX_MPG_TS:
                return dmx_probeTS(file,nbTracks,tracks);

        case DMX_MPG_PS:
                return dmx_probePS(file,nbTracks,tracks);
        }
        return 0;
}
uint8_t dmx_probePS(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks)
{
uint8_t dummy[10];
uint64_t seen[256],abs,rel;
int     audio,video;

        // It is mpeg PS
        // Create a fake demuxer, set a probe limite and collect info for all streams found
        dmx_demuxerPS *demuxer;
        MPEG_TRACK    pseudo;

               pseudo.pes=0xea; // Hopefully not used
               pseudo.pid=0;
               demuxer=new dmx_demuxerPS(256,&pseudo,0);
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
                //abs>>=20;
                printf("Stopped at %"LLU", %"LLU" MB\n",abs,abs>>20);

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
                delete demuxer;
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
        for(int i=0x40;i<0x49;i++) if(seen[i]>MIN_DETECT) audio++;

        *nbTracks=audio+1;      
        *tracks=new MPEG_TRACK[*nbTracks];
        
        memset(*tracks,0,(audio+1)*sizeof(MPEG_TRACK));
        (*tracks)[0].pes=video;
        audio=1;
#define DOME {(*tracks)[audio].pes=i;audio++;}
        for(int i=0;i<9;i++) if(seen[i]>MIN_DETECT) DOME;
        for(int i=0xc0;i<0xc9;i++) if(seen[i]>MIN_DETECT) DOME;
        for(int i=0xA0;i<0xA9;i++) if(seen[i]>MIN_DETECT) DOME;
        for(int i=0x40;i<0x49;i++) if(seen[i]>MIN_DETECT) DOME;

        // Now go a bit deeper and try to extract infos
        #define BUFFER_SIZE (10*1024)

        uint8_t buffer[BUFFER_SIZE];
        uint32_t read;
        uint32_t br,fq,offset,pes,chan;
        MpegAudioInfo mpegInfo;        

        for(int i=1;i<audio;i++)
        {

                pes=(*tracks)[i].pes;
                // Anything but PCM
                if((pes<0xC9 && pes>=0xc0) || ((pes<9)) || ((pes>=0x40 && pes<=0x49)))
                {
                        demuxer->changePid(0,pes);
                        demuxer->setPos(0,0);
                        read=demuxer->read(buffer,BUFFER_SIZE);
                        // We need about 5 Ko...
                        if(read>BUFFER_SIZE>>1)
                        {
                                if(pes<9)
                                {
                                        if(ADM_AC3GetInfo(buffer,read,&fq,&br,&chan,&offset))
                                        {
                                                (*tracks)[i].channels=chan;
                                                (*tracks)[i].bitrate=(8*br)/1000;
                                        }
                                }else
                                {

                                    if(pes>=0x40 && pes<=0x49)
                                    {
                                    uint32_t chan,samplerate,bitrate,framelength,syncoff,flags;
                                        if(ADM_DCAGetInfo(buffer,read,&samplerate,&bitrate,&chan, &syncoff,&flags))
                                        {
                                                (*tracks)[i].channels=chan;
                                                (*tracks)[i].bitrate=bitrate;
                                                if(syncoff) printf("[probe] There are some %u heading bytes\n",syncoff);
                                        }

                                    }
                                    else
                                        if(getMpegFrameInfo(buffer,read,&mpegInfo,NULL,&offset))
                                        {
                                                if(mpegInfo.mode!=3)  (*tracks)[i].channels=2;
                                                         else  (*tracks)[i].channels=1;
                                                
                                                (*tracks)[i].bitrate=mpegInfo.bitrate;
                                        }
                                }                

                        }
                }

        }

        delete demuxer;

        printf("Found video as %x, and %d audio tracks\n",video,audio-1);
        return 1;
}
uint8_t dmx_probeTS(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks)
{
        return dmx_probeTSBruteForce(file,nbTracks,tracks);
}
/**************************************
****************************************************************
    Brute force pid scanning in mpeg TS file
    We seek all PES packets and store their PID and PES id
*****************************************************************/
#define MAX_FOUND_PID 100
#define CHECK(x) val=parser->read8i(); left--;if(val!=x) goto _next;
typedef struct myPid
{
  uint32_t pid;
  uint32_t pes;

}myPid;
uint8_t dmx_probeTSBruteForce(char *file, uint32_t *nbTracks,MPEG_TRACK **tracks)
{

  // Brute force indexing
  //
  // Build a dummy track
MPEG_TRACK dummy[TS_ALL_PID];
fileParser *parser;
uint32_t   foundPid=0;
myPid      allPid[MAX_FOUND_PID];
uint8_t    buffer[BUFFER_SIZE];
MpegAudioInfo mpegInfo; 

    dummy[0].pid=0x1; // should no be in use
    dummy[0].pes=0xE0;

        dmx_demuxerTS demuxer(TS_ALL_PID,dummy,0);
        if(!demuxer.open(file))
        {
          return 0;
        }
    // Set probe to 10 Meg
      demuxer.setProbeSize(10*1024*1024L);
      parser=demuxer.getParser();
      // And run

      uint32_t pid,left,isPayloadStart,cc,val;
      uint64_t abs;
      while(demuxer.readPacket(&pid,&left, &isPayloadStart,&abs,&cc))
      {
        if(isPayloadStart)
        {
            // Is it a PES type packet
            // it should then start by 0x0 0x0 0x1 PID

            CHECK(0);
            CHECK(0);
            CHECK(1);
            val=parser->read8i();
            left--;
            // Check it does not exist already
            int present=0;
            for(int i=0;i<foundPid;i++) if(pid==allPid[i].pid) {present=1;break;}
            if(!present)
            {
              allPid[foundPid].pes=val;
              allPid[foundPid].pid=pid;
              foundPid++;
            }
            ADM_assert(foundPid<MAX_FOUND_PID);
        } 
_next:
        parser->forward(left);
      }
      if(!foundPid)
      {
         printf("ProbeTS: No PES packet header found\n");
         return 0;
      }
      //****************************************
      // Build information from the found Pid
      //****************************************
      for(int i=0;i<foundPid;i++) printf("Pid : %04x Pes :%02x \n",allPid[i].pid,allPid[i].pes);

      // Search for a pid for video track
      //
      *tracks=new MPEG_TRACK[foundPid];
      MPEG_TRACK *trk=*tracks;
      uint32_t vPid=0,vIdx;
      uint32_t offset,fq,br,chan;

      for(int i=0;i<foundPid;i++)
      {
        if(allPid[i].pes>=0xE0 && allPid[i].pes<=0xEA)
        {
            vPid=trk[0].pes=allPid[i].pes;
            trk[0].pid=allPid[i].pid;
            vIdx=i;
            break;
        }
      }
      if(!vPid)
      {
        delete [] trk;
        *tracks=0;
        printf("probeTs: No video track\n");
        return 0;
      }
      // Now build the other audio (?) tracks
      allPid[vIdx].pid=0;
      uint32_t start=1,code,id,read;
      for(int i=0;i<foundPid;i++)
      {
        code=allPid[i].pes;
        id=allPid[i].pid;

        if(!id) continue;

        if((code>=0xC0 && code <= 0xC9) || code==0xbd)
        {
            demuxer.changePid(id,code);
            demuxer.setPos(0,0);
            read=demuxer.read(buffer,BUFFER_SIZE);
            if(read!=BUFFER_SIZE) continue;
            if(code>=0xC0 && code <= 0xC9) // Mpeg audio
            {
              if(getMpegFrameInfo(buffer,read,&mpegInfo,NULL,&offset))
                   {
                      if(mpegInfo.mode!=3)  trk[start].channels=2;
                          else  trk[start].channels=1;
 
                      trk[start].bitrate=mpegInfo.bitrate;
                      trk[start].pid=id;
                      trk[start].pes=code;
                      start++;

                    }
            }
            else // AC3
            {
                  if(ADM_AC3GetInfo(buffer,read,&fq,&br,&chan,&offset))
                  {
                          trk[start].channels=chan;
                          trk[start].bitrate=(8*br)/1000;
                          trk[start].pid=id;
                          trk[start].pes=0;
                          start++;
                  }

            }

        }
      }
      *nbTracks=start;
      return 1;
}

