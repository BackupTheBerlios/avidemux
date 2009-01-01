/***************************************************************************
                        Mpeg2 in PS indexer                                            
                             
    
    copyright            : (C) 2005/2009 by mean
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

#include "ADM_default.h"
#include "ADM_demuxerInternal.h"
#include "fourcc.h"

#include "dmxPSPacket.h"

#include "avidemutils.h"
#include "ADM_quota.h"

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

typedef struct
{
    uint32_t w;
    uint32_t h;
    uint32_t fps;
    uint32_t interlaced;
    uint32_t ar;
}PSVideo;

static void writeVideo(FILE *file,PSVideo *video);
static void writeSystem(FILE *file,const char *filename,bool append);
/**
      \fn psIndexer 
      \brief main indexing loop for mpeg2 payload
*/
uint8_t   psIndexer(const char *file)
{
uint64_t packetStart,dts,pts;
uint32_t packetIndex;

uint8_t streamid;   
uint32_t temporal_ref,ftype,val;

uint8_t buffer[50*1024];
bool grabbing=false;
bool seq_found=false;
uint32_t nbPic=0;
FILE *index;

PSVideo video;
    
    char indexName[strlen(file)+5];
    sprintf(indexName,"%s.idx",file);
    index=qfopen(indexName,"wt");
    if(!index)
    {
        printf("[PsIndex] Cannot create %s\n",indexName);
        return false;
    }
    writeSystem(index,file,true);
    psPacketLinear *pkt=new psPacketLinear(0xE0);
    pkt->open(file,false);
   
      while(1)
      {
        uint32_t code=0xffff+0xffff0000;
        while((code&0x00ffffff)!=1 && pkt->stillOk())
        {
            code=(code<<8)+pkt->readi8();
        }
        if(!pkt->stillOk()) break;
        uint8_t startCode=pkt->readi8();

        pkt->getInfo(&packetStart, &packetIndex, &pts,&dts);

          switch(startCode)
                  {
                  case 0xB3: // sequence start
                          if(grabbing) continue;
                          grabbing=1;    
                          
                          if(seq_found)
                          {
                                  pkt->forward(8);  // Ignore
                                  continue;
                          }
                          //
                          seq_found=1;
                          val=pkt->readi32();
                          video.w=val>>20;
                          video.w=((video.w+15)&~15);
                          video.h= (((val>>8) & 0xfff)+15)& ~15;

                          video.ar = (val >> 4) & 0xf;

                          
                          video.fps= FPS[val & 0xf];
                          pkt->forward(4);
                          writeVideo(index,&video);
                          break;
                  case 0xb8: // GOP
                          
                          if(!seq_found) continue;
                          if(grabbing) 
                          {         
                                  continue;;
                          }
                          break;
                  case 0x00 : // picture
                        
                          if(!seq_found)
                          { 
                                  continue;
                                  printf("No sequence start yet, skipping..\n");
                          }
                          grabbing=0;
                          val=pkt->readi16();
                          temporal_ref=val>>6;
                          ftype=7 & (val>>3);
                          //aprintf("Temporal ref:%lu\n",temporal_ref);
                          // skip illegal values
                          if(ftype<1 || ftype>3)
                          {
                                  printf("[Indexer]Met illegal pic at %"LLX" + %"LX"\n",
                                                  packetStart,packetIndex);
                                  continue;
                          }
/*
                          printf("[PsIndexer] Found image %c at %"LLU":%"LU" pts:%"LLD" dts:%"LLD"\n",
                                                                Type[ftype],packetStart,packetIndex,pts,dts);
*/
                          if(ftype==1) // intra
                          {
                                if(!nbPic) 
                                {
                                    qfprintf(index,"[Data]\n");
                                }else qfprintf(index,"\n");
                                qfprintf(index,"Video %"LLX":%"LX" ts:%"LLD":%"LLD,packetStart,packetIndex,pts,dts);
                           }
                            qfprintf(index," %c",Type[ftype]);
                            nbPic++;
                          break;
                  default:
                    break;
                  }
      }
    
        printf("\n");
        
        qfprintf(index,"\n[End]\n");
        qfclose(index);
        delete pkt;
        return 1; 
}
/**
    \fn writeVideo
    \brief Write Video section of index file
*/
void writeVideo(FILE *file,PSVideo *video)
{
    qfprintf(file,"[Video]\n");
    qfprintf(file,"Width=%d\n",video->w);
    qfprintf(file,"Height=%d\n",video->h);
    qfprintf(file,"Fps=%d\n",video->fps);
    qfprintf(file,"Interlaced=%d\n",video->interlaced);
    qfprintf(file,"AR=%d\n",video->ar);

}
/**
    \fn writeSystem
    \brief Write system part of index file
*/
void writeSystem(FILE *file,const char *filename,bool append)
{
    qfprintf(file,"PSD1\n");
    qfprintf(file,"[System]\n");
    qfprintf(file,"Type=P\n");
    qfprintf(file,"File=%s\n",filename);
    qfprintf(file,"Append=%d\n",append);

}

/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

//

//EOF
