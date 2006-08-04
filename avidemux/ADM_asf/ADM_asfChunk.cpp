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


static const chunky mychunks[]=
{
  {"Header Chunk",    {0x30,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c},ADM_CHUNK_HEADER_CHUNK},
  {"File Header",     {0xa1,0xdc,0xab,0x8c,0x47,0xa9,0xcf,0x11,0x8e,0xe4,0x00,0xc0,0x0c,0x20,0x53,0x65},ADM_CHUNK_FILE_HEADER_CHUNK},
  {"No audio conceal",{0x40,0x52,0xd1,0x86,0x1d,0x31,0xd0,0x11,0xa3,0xa4,0x00,0xa0,0xc9,0x03,0x48,0xf6},ADM_CHUNK_NO_AUDIO_CONCEAL},
  {"Stream Header",   {0x91,0x07,0xdc,0xb7,0xb7,0xa9,0xcf,0x11,0x8e,0xe6,0x00,0xc0,0x0c,0x20,0x53,0x65},ADM_CHUNK_STREAM_HEADER_CHUNK},
  {"Stream Group Id", {0xce,0x75,0xf8,0x7b,0x8d,0x46,0xd1,0x11,0x8d,0x82,0x00,0x60,0x97,0xc9,0xa2,0xb2},ADM_CHUNK_STREAM_GROUP_ID},
  {"Data Chunk",      {0x36,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c},ADM_CHUNK_DATA_CHUNK},
  {"zz",{0x30,0x26,0xb2,0x75,0x8e,0x66,0xcf,0x11,0xa6,0xd9,0x00,0xaa,0x00,0x62,0xce,0x6c},ADM_CHUNK_HEADER_CHUNK}
  
};
static const chunky nochunk=
{"Unknown",{0x10,0x20,0x30,0x40,0xde,0xad,0xde,0xad,0xbe,0xef,0xbe,0xef,0x00,0x62,0xce,0x6c},ADM_CHUNK_UNKNOWN_CHUNK};


 asfChunk::asfChunk(FILE *f)
{
  _fd=f;
  _chunkStart=ftello(f);;
  printf("Chunk created at %x\n",_chunkStart);
  ADM_assert(_fd);
  chunkLen=0;
}
 asfChunk::~asfChunk()
{
}
uint8_t   asfChunk::readChunkPayload(uint8_t *data, uint32_t *dataLen)
{
  uint32_t remaining;
  
  remaining=ftello(_fd);
  remaining-=_chunkStart;
  remaining=chunkLen-remaining;
  fread(data,remaining,1,_fd);
  *dataLen=remaining;
  return 1;
  
}
uint8_t   asfChunk::nextChunk(void)
{
  uint32_t low,high;
  
  if(_chunkStart)
  {
    
    fseeko(_fd,_chunkStart+ chunkLen,SEEK_SET);
  }
  
  _chunkStart=ftello(_fd);
  fread(guId,16,1,_fd);
  low=read32();
  high=read32();
  chunkLen=high;
  chunkLen<<=32;
  chunkLen+=low;
  
  printf("Next chunk from %x +%u to %x\n",_chunkStart,chunkLen,chunkLen+_chunkStart);
  
  return 1;
  
}
uint8_t   asfChunk::skipChunk(void)
{
  uint32_t go;
  go=_chunkStart+ chunkLen;
  printf("Pos %x\n",ftello(_fd));
  fseeko(_fd,go,SEEK_SET);
  printf("Skipping to %x\n",go);
  
  return 1; 
}
uint64_t  asfChunk::read64(void)
{
  uint64_t lo,hi;
  lo=read32();
  hi=read32();
  return lo+(hi<<32); 
  
}
uint32_t   asfChunk::read32(void)
{
  uint8_t c[4];
  
  fread(c,4,1,_fd);
  
  return c[0]+(c[1]<<8)+(c[2]<<16)+(c[3]<<24);
  
}
uint32_t   asfChunk::read16(void)
{
  uint8_t c[2];
  
  fread(c,2,1,_fd);
  
  return c[0]+(c[1]<<8);
  
}

uint8_t   asfChunk::read8(void)
{
  uint8_t c[1];
  
  fread(c,1,1,_fd);
  
  return c[0];
  
}
uint8_t   asfChunk::read(uint8_t *where, uint32_t how)
{
 
  if(1!=fread(where,how,1,_fd))
  {
    printf("[AsfChunk] Read error\n");
    return 0; 
  }
  return 1;

  
}
uint8_t   asfChunk::dump(void)
{
  const chunky *id;
  id=chunkId();
  printf("Chunk type  : %s\n",id->name);
  printf("Chunk Start : %x\n",_chunkStart);
  printf("Chunk Len   : %lu\n",(uint32_t)chunkLen);
  for(int i=0;i<16;i++) printf("%02x ",guId[i]);
  printf("\n");
  return 1;
  
}
const chunky *asfChunk::chunkId(void)
{
  int mx=sizeof(mychunks)/sizeof(chunky);
  for(int i=0;i<sizeof(mychunks)/sizeof(chunky);i++)
  {
    if(!memcmp(mychunks[i].val,guId,16)) return &mychunks[i];
  }
  return &nochunk;
  
}

