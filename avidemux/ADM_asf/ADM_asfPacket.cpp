/***************************************************************************
    copyright            : (C) 2006 by mean
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

#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "ADM_library/fourcc.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_asfPacket.h"


asfPacket::asfPacket(FILE *f,uint32_t pSize)
 {
   _fd=f;
   pakSize=pSize;
   ADM_assert(pakSize);
   packetStart=ftello(f);;
   printf("Packet created at %x\n",packetStart);
   ADM_assert(_fd);

 }
 asfPacket::~asfPacket()
 {
 }
 uint8_t   asfPacket::readChunkPayload(uint8_t *data, uint32_t *dataLen)
 {
   uint32_t remaining;
   *dataLen=0;
   return 1;
  
 }
 /*
      Read ASF packet & segments 
 
    Flags are bitwise OR of:
   
 0x40 Explicit packet size specified word16  0X60 Means word32
 0x20 Explicit packet size specified byte
   
 0x10 16-bit padding size specified  0x18 means word32
 0x08 8-bit padding size specified
   
 0x04 sequence coded in word16
 0x02 sequence coded in byte
 0x01 More than one segment
 
 
 Docs from http://avifile.sourceforge.net/asf-1.0.htm
  completed by mplayer code
  
 
 */
 uint8_t   asfPacket::nextPacket(void)
 {
   uint32_t atime,aduration,nbSeg,segType=0x80;
   uint32_t sequenceLen,len,streamId;
   int32_t   packetLen=0;
   uint32_t  paddingLen;
   uint8_t   flags;
    
   packetStart=ftello(_fd);
   if(read8()!=0x82) 
   {
     printf("not a 82 packet\n");
     return 0;
   }
   printf("============== New packet ===============\n");
   read16();          // Always 0 ????
   flags=read8();
   segmentId=read8();
   packetLen=0;
   paddingLen=0;
   

   // Read packetLen
   packetLen=readVCL(flags>>5);
   // Sequence len
   sequenceLen=readVCL(flags>>1);
   // Read padding size (padding):
   paddingLen=readVCL(flags>>3);
   
   printf("paddingLen :         %d\n",paddingLen);
   
// Explicit (absolute) packet size	    
   if(((flags>>5)&3))
   {
     printf("## Explicit packet size %d\n",packetLen);
     if(packetLen>pakSize) printf("**************Len > packet size!! (%d /%d)\n",packetLen,pakSize);
   } 
   if(!packetLen)
   {
     // Padding (relative) size
     packetLen=ftello(_fd)-packetStart;
     packetLen=pakSize-packetLen-paddingLen;
   }

   
  
   atime=read32(); // Send time
   aduration=read16(); // Duration
   
   if(flags &1) // Multiseg
   {
     uint8_t r=read8();
     nbSeg=r&0x3f;
     segType=r>>6;
   }
   else
   {
     nbSeg=1; 
   }
   printf("-----------------------\n");
   printf("Flags     :           0X%x",flags);
   
   if(flags & 0x40) printf(" Packet Len Specified  ");
   if(flags & 0x10) printf(" Padding 16bits ");
   if(flags & 0x8) printf(" Padding 8bits ");
   if(flags & 0x1) printf(" Multiseg ");
   printf("\n");
   printf("SegmentId :           %d\n",segmentId);
   printf("sequenceLen :         %d\n",sequenceLen);
   
   
   printf("packetLen :           %d\n",packetLen);
   printf("Send      :           %d\n",atime);
   printf("Duration  :           %d\n",aduration);
   printf("# of seg  :           %d %x\n",nbSeg,segType);
   
   // Now read Segments....
   //
   uint32_t sequence, offset,replica,r;
   int32_t remaining;
   uint32_t payloadLen;
   for(int seg=0;seg<nbSeg;seg++)
   {
     r=read8(); // Read stream Id
     
     streamId=r&0x7f;
     printf(">>>>>Stream Id : %x<<<<<\n",streamId);
     if(r&0x80) printf("KeyFrame\n");
     sequence=readVCL(segmentId>>4);
     offset=readVCL(segmentId>>2);
     replica=readVCL(segmentId);
     printf("replica                %d\n",replica);
     // Skip replica data_len
     skip(replica);
     
     payloadLen=0;
     if(flags &1)  // multi seg
     {
       payloadLen=readVCL(segType);
       if(payloadLen)
        printf("##len                    %d\n",payloadLen);
       
     }
     remaining=ftello(_fd)-packetStart;
     remaining=pakSize-remaining-paddingLen;
     printf("Remaining %d asked %d\n",remaining,payloadLen);
     if(remaining<=0) 
     {
       printf("** Err: No data left (%d)\n",remaining); 
     }
     if(!payloadLen)
     {
       payloadLen=remaining;
     }
     if(remaining<payloadLen)
     {
       printf("** WARNING too big %d %d\n", remaining,packetLen);
       payloadLen=remaining;
     }
     printf("This segment %d bytes, %d /%d\n",packetLen,seg,nbSeg);
     printf("Offset                 %d\n",offset);
     printf("sequence               %d\n",sequence);
     printf("Grouping               %d\n",replica==1);
     printf("payloadLen             %d\n",payloadLen);
     // Frag
     if(replica==1) // Grouping
     {
       // Each tiny packet starts with 
       // 1 byte = packet Len
       // Data and we read them until "payloadLen" is comsumed
       while(payloadLen>0)
       {
         uint8_t l=read8();
         payloadLen--;
         if(l>payloadLen)
         {
           
           printf("oops exceeding %d/%d\n",l,payloadLen);
           skip(payloadLen);
           break;
         }
         skip(l);
         payloadLen-=l;
       }
       
     }else
     { // else we read "payloadLen" bytes and put them at offset "offset"
       skip(payloadLen);
       printf("Reading %d bytes\n",payloadLen);
     }
     
   }
   // Do some sanity check
   uint32_t pos=ftello(_fd);
   pos-=packetStart;
   pos=pakSize-pos;
   if(pos!=paddingLen) printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! %d %d\n",pos,paddingLen);
   return 1;
  
 }
 uint32_t asfPacket::readVCL(uint32_t bitwise)
 {
   uint32_t r;
   switch(bitwise&3)
   {
     case 3: r=read32();break;  // dword
     case 2: r=read16();break;  // word
     case 1: r=read8();break;   // byte
     default: r=0;
   }
   return r;
 }

 uint8_t   asfPacket::skipPacket(void)
 {
   uint32_t go;
   go=packetStart+ pakSize;
   printf("Pos %x\n",ftello(_fd));
   fseeko(_fd,go,SEEK_SET);
   printf("Skipping to %x\n",go);
  
   return 1; 
 }
 uint64_t  asfPacket::read64(void)
 {
   uint64_t lo,hi;
   lo=read32();
   hi=read32();
   return lo+(hi<<32); 
  
 }
 uint32_t   asfPacket::read32(void)
 {
   uint8_t c[4];
  
   fread(c,4,1,_fd);
  
   return c[0]+(c[1]<<8)+(c[2]<<16)+(c[3]<<24);
  
 }
 uint32_t   asfPacket::read16(void)
 {
   uint8_t c[2];
  
   fread(c,2,1,_fd);
  
   return c[0]+(c[1]<<8);
  
 }

 uint8_t   asfPacket::read8(void)
 {
   uint8_t c[1];
  
   fread(c,1,1,_fd);
  
   return c[0];
  
 }
 uint8_t   asfPacket::read(uint8_t *where, uint32_t how)
 {
 
   if(1!=fread(where,how,1,_fd))
   {
     printf("[AsfPacket] Read error\n");
     return 0; 
   }
   return 1;

  
 }
 uint8_t   asfPacket::skip( uint32_t how)
 {
   fseeko(_fd,how,SEEK_CUR);
   return 1;
 }
 uint8_t   asfPacket::dump(void)
 {
  
   return 1;
  
 }
 //EOF
