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
uint32_t imageW,imageH,imageAR ,imageFPS;

uint8_t buffer[50*1024];
bool grabbing=false;
bool seq_found=false;
uint32_t nbPic=0;


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
                          imageW=val>>20;
                          imageW=((imageW+15)&~15);
                          imageH= (((val>>8) & 0xfff)+15)& ~15;

                          imageAR = (val >> 4) & 0xf;

                          
                          imageFPS= FPS[val & 0xf];
                          printf("[PSIndexer] %dx%d %d fps AR:%d\n",imageW,imageH,imageFPS,imageAR);
                          pkt->forward(4);
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
                                  printf("[Indexer]Met illegal pic at %"LLX" + %"LLX"\n",
                                                  packetStart,packetIndex);
                                  continue;
                          }
/*
                          printf("[PsIndexer] Found image %c at %"LLU":%"LU" pts:%"LLD" dts:%"LLD"\n",
                                                                Type[ftype],packetStart,packetIndex,pts,dts);
*/
                          if(ftype==1) // intra
                          {
                                if(nbPic) printf("\n");
                                printf("Video %"LLX":%"LX" ts:%"LLD":%"LLD,packetStart,packetIndex,pts,dts);
                           }
                            printf(" %c",Type[ftype]);
                            nbPic++;
                          break;
                  default:
                    break;
                  }
      }
        printf("\n");
        delete pkt;
        return 1; 
}


/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/
/********************************************************************************************/

//

//EOF
