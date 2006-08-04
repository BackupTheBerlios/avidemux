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

#include "ADM_asf.h"



 asfPacket::asfPacket(FILE *f)
 {
   _fd=f;
   packetStart=ftello(f);;
   printf("Packet created at %x\n",packetStart);
   ADM_assert(_fd);
   packetLen=0;
 }
 asfPacket::~asfPacket()
 {
 }
 uint8_t   asfPacket::readChunkPayload(uint8_t *data, uint32_t *dataLen)
 {
   uint32_t remaining;
  
   remaining=ftello(_fd);
   remaining-=packetStart;
   remaining=packetLen-remaining;
   fread(data,remaining,1,_fd);
   *dataLen=remaining;
   return 1;
  
 }
 uint8_t   asfPacket::nextPacket(void)
 {
   if(read8()!=0x82) 
   {
     printf("not a 82 packet\n");
     return 0;
   }
   read16();
   flags=read8();
   segmentId=read8();
  // packetLen=read16();
   paddingType=0;
   if(flags&0x10) paddingType=read16();
   if(flags&0x8) paddingType=read8();
   read32(); // Send time
   read16(); // Duration
   read8();
   
   return 1;
  
 }
 uint8_t   asfPacket::skipPacket(void)
 {
   uint32_t go;
   go=packetStart+ packetLen;
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
 uint8_t   asfPacket::dump(void)
 {
  
   return 1;
  
 }
 //EOF
