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

#include "default.h"
#include <ADM_assert.h>


#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/filesel.h"
#include "fourcc.h"
#include "DIA_working.h"



#include "ADM_osSupport/ADM_debugID.h"
#define MODULE_NAME MODULE_MPEG
#include "ADM_osSupport/ADM_debug.h"

#include "dmx_demuxerPS.h"
#include "dmx_demuxerTS.h"
#include "dmx_identify.h"
#include "dmx_probe.h"
#include "DIA_busy.h"
#include "ADM_audio/ADM_mp3info.h"
#include "ADM_audio/ADM_a52info.h"
#include "ADM_audio/ADM_dcainfo.h"



#define MAX_PROBE (10*1024*1024LL) // Scans the 4 first meg
#define MIN_DETECT (10*1024) // Need this to say the stream is present
#define MAX_NB_PMT 50
//****************************************************************************************
typedef struct MPEG_PMT
{
   uint32_t programNumber;
   uint32_t tid;
}MPEG_PMT;
//****************************************************************************************

static uint8_t dmx_probePS(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks);
static uint8_t dmx_probeTS(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks);
static uint8_t dmx_probeTSBruteForce(char *file,  uint32_t *nbTracks,MPEG_TRACK **tracks);
static uint8_t dmx_probeMSDVR(char *file, uint32_t *nbTracks,MPEG_TRACK **tracks);
/* For mpeg TS with PSI only */
static uint8_t dmx_probePat(dmx_demuxerTS *demuxer, uint32_t *nbPmt,MPEG_PMT *pmts,uint32_t maxPmt);
static uint8_t dmx_probePMT(dmx_demuxerTS *demuxer, uint32_t pmtId);

 uint8_t dmx_probeTSPat(const char *file, uint32_t *nbTracks,MPEG_TRACK **tracks);
 
extern uint32_t mpegTsCRC(uint8_t *data, uint32_t len);

//****************************************************************************************
uint8_t dmx_probe(char *file, DMX_TYPE  *type, uint32_t *nbTracks,MPEG_TRACK **tracks)
{

        printf("Probing %s for streams...\n",file);
        *type=dmxIdentify(file);
        switch(*type)
        {
        case DMX_MPG_MSDVR:
                {
                  return dmx_probeMSDVR(file,nbTracks,tracks);
                  printf("This is MSDVR file\n"); 
                  *nbTracks=1;
                  *tracks=new MPEG_TRACK;
                  (*tracks)->pes=0xE0;
                  (*tracks)->pid=1;
                  return 1;
                }
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
                                    uint32_t chan,samplerate,bitrate,framelength,syncoff,flags,nbs;
                                        if(ADM_DCAGetInfo(buffer,read,&samplerate,&bitrate,&chan, &syncoff,&flags,&nbs))
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
/* ****************************************************** */
#include "ADM_asf/ADM_asfPacket.h"
#include "ADM_asf/ADM_asf.h"

#define PROBE_BUF 32000
#define MAX_MSVDR_STREAMS 6
#define MAX_PACKET_PROBE 2000;   // assuming a packet is 8kB we will probe around 15 Meg
#define DETECT_MIN 7000

uint8_t dmx_probeMSDVR(char *file, uint32_t *nbTracks,MPEG_TRACK **ztracks)
{
  int r=5;
  const chunky *id=NULL;
  uint32_t nbPackets,dataStart;
  ADM_queue queue;
  MPEG_TRACK *tracks;
  uint8_t     buffer[MAX_MSVDR_STREAMS][PROBE_BUF*2];
  uint32_t    streamlen[MAX_MSVDR_STREAMS];
  
  memset(streamlen,0,MAX_MSVDR_STREAMS*sizeof(uint32_t));
  
      printf("**** Probing MSDVR file ****\n");
      // Assume first track is video 
      *nbTracks=1;
      tracks=new MPEG_TRACK[MAX_MSVDR_STREAMS];
      *ztracks=tracks;
      tracks[0].pes=0xE0;
      tracks[0].pid=1;
      // Now check for track up to 5
      
      FILE *fd=NULL;
      
      fd=fopen(file,"rb");
      if(!fd)
      {
        printf("Demuxer MSDVR open failed\n");
        return 0; 
      }
  // Get the data chunk and ignore others
      asfChunk h(fd);

      printf("[MSDVR] Searching data\n");
      while(r--)
      {
        h.nextChunk();    // Skip headers
        id=h.chunkId();
        h.dump();
        if(id->id==ADM_CHUNK_DATA_CHUNK) break;
        h.skipChunk();
      }
      if(id->id!=ADM_CHUNK_DATA_CHUNK)
      {
        printf("[MSDVR] Cannot find data chunk\n");
        return 0; 
      }
      h.read32();
      h.read32();
      h.read32();
      h.read32();
      nbPackets=(uint32_t) h.read64();
      h.read16();
      
  //********** Ready

      uint32_t probeceil=MAX_PACKET_PROBE;
      uint32_t count=0;
      
      if(probeceil>nbPackets) probeceil=nbPackets;
      
      dataStart=ftello(fd);
      asfPacket *packet;
      packet=new asfPacket(fd,nbPackets,0x2000,&queue,dataStart);
      
      printf("[MSDVR] Opened ok\n");
      // Parse and collect
      while(count++<probeceil)
      {
        if(!packet->nextPacket(0xff))
        {
          break; 
        }
        packet->skipPacket();
        // Now look into it
        while(!queue.isEmpty())
        {
          asfBit *bit;
          ADM_assert(queue.pop((void**)&bit));
          if(bit->stream>=MAX_MSVDR_STREAMS)
          {
            printf("Found stream %u, ignored\n",bit->stream); 
          }else
          {
            uint32_t len=streamlen[bit->stream];
            if(len<PROBE_BUF)
            {
              uint8_t *ptr=buffer[bit->stream];
              memcpy(ptr+len,bit->data,bit->len);
              len+=bit->len;
              streamlen[bit->stream]=len;
            }
          }
          delete bit;
      }
      } // /while
      // Now we have filled the buffers
      // identifies the content
      uint32_t sync;
      for(int i=2;i<MAX_MSVDR_STREAMS;i++)
      {
        printf("We have found %u bytes for streamId %u\n",streamlen[i],i);
        if(streamlen[i]>=PROBE_BUF)
        {  // We have a candidate
           // What is it ? AC3 or MP2 or subs ? 
          // Maybe mpegaudio ?
          MpegAudioInfo mpegInfo;
          if(getMpegFrameInfo(buffer[i],streamlen[i],&mpegInfo,NULL,&sync))
          {
            tracks[*nbTracks].channels=2;
            if(mpegInfo.mode==3) 
              tracks[*nbTracks].channels=1;
            tracks[*nbTracks].bitrate=mpegInfo.bitrate;
            tracks[*nbTracks].pid=i;
            tracks[*nbTracks].pes=0xC0;
            *nbTracks=*nbTracks+1;
            continue;
          }
          // AC3 ?
          uint32_t fq, br, chan, sync;
          
          if(ADM_AC3GetInfo(buffer[i], streamlen[i],&fq, &br,&chan,&sync))
          {
            tracks[*nbTracks].channels=chan;
            tracks[*nbTracks].bitrate=(8*br)/1000;
            tracks[*nbTracks].pid=i;
            tracks[*nbTracks].pes=0;
            *nbTracks=*nbTracks+1;
            continue;
          }
          
        }
        
      } // /for
      packet->purge();
      delete packet;
      fclose(fd);
      return 1;
}
/**
      \fn     dmx_probeTSPat(char *file, uint32_t *nbTracks,MPEG_TRACK **tracks)
      \brief  Try to extract info from a Mpeg TS file using PAT, PMT etc..
      @return 1 on success, 0 on failure
      @param file: File to scan
      @param *nbTrack : number of track found (out)
      @param **tracks : contains info about the tracks found (out)

*/
uint8_t dmx_probeTSPat(char *file, uint32_t *nbTracks,MPEG_TRACK **tracks)
{
MPEG_TRACK dummy[TS_ALL_PID];
fileParser *parser;
uint32_t   foundPid=0;
myPid      allPid[MAX_FOUND_PID];
uint8_t    buffer[BUFFER_SIZE];
MpegAudioInfo mpegInfo; 

    dummy[0].pid=0x00; // should no be in use
    dummy[0].pes=0xE0;

        dmx_demuxerTS demuxer(1,dummy,1);
        if(!demuxer.open(file))
        {
          return 0;
        }
    // Set probe to 10 Meg
      demuxer.setProbeSize(10*1024*1024L);
      uint32_t nbPmt;
      MPEG_PMT pmts[MAX_NB_PMT];

      
      if(!dmx_probePat(&demuxer,&nbPmt,pmts,MAX_NB_PMT))
      {
        printf("[PSI Probe]Cannot find Pat\n"); 
        parser=NULL;
        return 0;
      }
      printf("Found %d PMT..\n",nbPmt);
      demuxer.setProbeSize(40*1024*1024L); // We can can increase the probe size
      for(int i=0;i<nbPmt;i++)
      {
         dmx_probePMT(&demuxer, pmts[i].tid);
      }
}
uint8_t runProbe(char *file)
{
  uint32_t nb;
  return  dmx_probeTSPat(file, &nb,NULL);
  
}
/**
      \fn     dmx_searchAndSkipHeader
      \brief  Search for a given PSI and skip header
      @return 1 on success, 0 on failure
      @param myPid : Pid of the looked for psi
      @param demuxer: mpegTS demuxer *(input)
      @param *currentSec : current section (output)
      @param *maxSec : #of sections (output)
      @param *leftbyte : Total #of bytes left in the packet
      @param *payloadSize : #of bytes of usable payload

*/
uint8_t dmx_searchAndSkipHeader(uint32_t myPid,dmx_demuxerTS *demuxer,uint32_t *currentSec, uint32_t *maxSec,
                                    uint32_t *leftbyte,uint32_t *payloadSize)
{
  
  uint8_t packet[TS_PACKET_SIZE];
  uint32_t tableId;
  uint32_t misc;
  uint32_t sectionLength;
  uint32_t tId,pid,left,cc,nbPmt;
  uint32_t version,isPayloadStart;
  uint32_t sectionNumber;
  uint32_t lastSectionNumber;
  uint32_t programInfoLength;
  uint32_t crc,crccomputed;
  uint64_t startPos,endPos,abso;
  fileParser *parser;
      demuxer->changePid(myPid,myPid); // Search PAT
      parser=demuxer->getParser();
      
        while(demuxer->readPacket(&pid,&left, &isPayloadStart,&abso,&cc))
        {
          if(pid!=myPid)
          {
            printf("Wrong Pid %x/%x\n",pid,myPid);
            parser->forward(left);
            continue;
          }
          if(!isPayloadStart || left <= (9+4))
          {
            parser->forward(left);
            continue;
          }

          /* Found something that looks good...*/
            
            /* Decode PSI header */
            parser->read8i(); /* Pointer field, can be ignored (?) */
              
              parser->getpos(&startPos); /* Memorize beginning */
              tableId=parser->read8i();
              misc=parser->read16i(); // +3
              tId=parser->read16i();  
              version=parser->read8i(); // +6
              sectionNumber=parser->read8i();
              lastSectionNumber=parser->read8i(); // +8
              
              sectionLength=misc&0xFFF;
              
              if(sectionLength<=9 || left <=9)
              {
                printf("SectionLength too short :%d\n", sectionLength);
                 parser->setpos(startPos-1+left); // skip packet
                 continue;
              }
              
#ifdef PROBE_TS_VERBOSE
              printf("******************************************\n");
              printf("tableId        : %d\n",tableId);
              
              printf("sectionLength  : %d\n",sectionLength);
              printf("0              : %x\n",misc&0x40);
              printf("section syntax : %x\n",misc&0x80);
              printf("Transport ID   : 0x%x\n",tId);
              printf("Version Number : 0x%x\n",(version>>1)&0x1F);
              printf("CurrentNext    : 0x%x\n",version&1);
              
              printf("Section        : %d\n",sectionNumber);
              printf("LastSection    : %d\n",lastSectionNumber);
              
#endif
              // Check for error FIXME TODO
              
              // Check CRC
              parser->getpos(&endPos); // Here payload begins
              parser->setpos(startPos);
              parser->read32(sectionLength-1,packet); // Go back & Read Whole packet +3 for header -4 CRC
              crc=parser->read32i();
              crccomputed=mpegTsCRC(packet,sectionLength-1);
              if(crc!=crccomputed) // Bad CRC, skip packet
              {
                printf("Bad CRC\n");
                parser->setpos( startPos+left-1); // skip
                continue;
              }
              // CRC is ok, go back to interesting place
              printf("CRC OK\n");
              parser->setpos(endPos);
              *currentSec=sectionNumber;
              *maxSec=lastSectionNumber;
              
              *leftbyte=left-9;               // Total bytes left in packet
              *payloadSize=sectionLength-9; // No CRC, No header
              return 1;
          } // /while
      return 0;
}
/**
      \fn     dmx_probePat(dmx_demuxerTS *demuxer, uint32_t *nbPmt,MPEG_PMT *pmts,uint32_t maxPMT)
      \brief  Search for PAT and returns PMT info if found
      @return 1 on success, 0 on failure
      @param demuxer: mpegTS demuxer (input)
      @param *nbPmt : number of PMTS found (output)
      @param *pmts : contains info about the PMT found (out but must be allocated by caller)
      @param maxPMT : Maximum # of PMT we accept in pmts (in)

*/
uint8_t dmx_probePat(dmx_demuxerTS *demuxer, uint32_t *nbPmt,MPEG_PMT *pmts,uint32_t maxPmt)
{
  
  fileParser *parser;
  uint32_t curSection,maxSection;
  uint32_t left,toScan;
  
      parser=demuxer->getParser();
      *nbPmt=0;
      if(dmx_searchAndSkipHeader(0,demuxer,&curSection, &maxSection,&left,&toScan))
      {
        
              while(toScan >=4 && left>=8)
              {
                  printf("**\n");
                  pmts[*nbPmt].programNumber=parser->read16i()&0xFFFF;
                  pmts[*nbPmt].tid=parser->read16i()&0x1FFF;
                  printf(" [PAT]Program Number :%03x\n",pmts[*nbPmt].programNumber);
                  printf(" [PAT]PID for this   :%03x\n",pmts[*nbPmt].tid);
                  if((*nbPmt)<maxPmt)
                      (*nbPmt)++;
                  left-=4;
                  toScan-=4;
              }
              return 1;
        
      }
      return 0;
}
const char *toString(uint32_t type)
{
 switch(type)
 {
   case 1:case 2: return "Mpeg Video";
   case 3:case 4: return "Mpeg Audio";
   case 0x1B: return "H264";
   case 0x81: return "Private (AC3?)";
   
   default: return "???"; 
 }
  return "???";
}
/**
      \fn     dmx_probePat(dmx_demuxerTS *demuxer, uint32_t *nbPmt,MPEG_PMT *pmts,uint32_t maxPMT)
      \brief  Search for PAT and returns PMT info if found
      @return 1 on success, 0 on failure
      @param demuxer: mpegTS demuxer (input)
      @param *nbPmt : number of PMTS found (output)
      @param *pmts : contains info about the PMT found (out but must be allocated by caller)
      @param maxPMT : Maximum # of PMT we accept in pmts (in)

*/
uint8_t dmx_probePMT(dmx_demuxerTS *demuxer, uint32_t pmtId)
{
  
  fileParser *parser;
  uint32_t curSection,maxSection;
  uint32_t left,toScan,programInfo;
  
      printf("Searching for PMT, pid=0x%x\n",pmtId);
      demuxer->changePid(pmtId,pmtId); // change pid as setPos will seek for them
      demuxer->setPos(0,0);
      parser=demuxer->getParser();
      if(dmx_searchAndSkipHeader(pmtId,demuxer,&curSection, &maxSection,&left,&toScan))
      {
               printf("[PMT]PCR for it    :x%x\n",parser->read16i()&0x1FFF);
               programInfo=parser->read16i() & 0x0FFF;
               printf("[PMT]Program Info  :%d\n",programInfo);
               parser->forward(programInfo);
               toScan-=(2+programInfo);
               left-=(2+programInfo);
               while(toScan >=5 )
              {
                  uint8_t streamType;
                  uint32_t pid,esDescLen;
                  printf("**\n");
                  streamType=parser->read8i();
                  pid       =parser->read16i()&0x1FFF;
                  esDescLen =parser->read16i()&0x0FFF;
                  printf("[PMT]Stream Type :0x%x (%s)\n",streamType,toString(streamType));
                  printf("[PMT]Pid         :0x%x\n",pid);
                  printf("[PMT]ES Len      :%d\n",esDescLen);
                  
                  parser->forward(esDescLen);
                  left-=(5+esDescLen);
                  toScan-=(5+esDescLen);
                  printf("[PMT]left %u toscan %u\n",left,toScan);
              }
              
      }
      return 0;
}

/****EOF**/
