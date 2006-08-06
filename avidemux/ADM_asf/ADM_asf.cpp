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
#include "ADM_dialog/DIA_working.h"
#include "ADM_asf.h"
#include "ADM_asfPacket.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_ASF
#include "ADM_toolkit/ADM_debug.h"

static const uint8_t asf_audio[16]={0x40,0x9e,0x69,0xf8,0x4d,0x5b,0xcf,0x11,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b};
static const uint8_t asf_video[16]={0xc0,0xef,0x19,0xbc,0x4d,0x5b,0xcf,0x11,0xa8,0xfd,0x00,0x80,0x5f,0x5c,0x44,0x2b};


WAVHeader *asfHeader::getAudioInfo(void )
{
  
  return NULL;
}
/*
    __________________________________________________________
*/

uint8_t asfHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
 
  *audio=NULL;
  return 0; 
}
/*
    __________________________________________________________
*/

void asfHeader::Dump(void)
{
 
  printf("*********** ASF INFO***********\n");
}
/*
    __________________________________________________________
*/

uint32_t asfHeader::getNbStream(void) 
{
  return _nbAudioTrack; 
}
/*
    __________________________________________________________
*/

uint8_t asfHeader::close(void)
{
  if(_videoExtraData)
  {
    delete [] _videoExtraData;
    _videoExtraData=NULL; 
  }
  if(myName)
  {
    delete [] myName;
    myName=NULL; 
  }
  if(_extraData)
  {
    delete [] _extraData;
    _extraData=NULL; 
  }
  if(_index)
  {
    delete [] _index; 
  }
  _index=NULL;
  if(_packet)
    delete _packet;
  _packet=NULL;
}
uint8_t       asfHeader::getExtraHeaderData(uint32_t *len, uint8_t **data)
{
  *len=_extraDataLen;
  *data=_extraData;
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t asfHeader::needDecompress(void)
{
  return 1; 
}
/*
    __________________________________________________________
*/

 asfHeader::asfHeader( void ) : vidHeader()
{
  _fd=NULL;
  _audioIndex=_videoIndex=-1;
  _nbAudioTrack=0;
  _audioTracks=NULL;
  myName=NULL;
  _extraDataLen=0;
  _extraData=NULL;
  _packetSize=0;
  _videoStreamId=0;
  nbImage=0;
  _index=NULL;
  _packet=NULL;
}
/*
    __________________________________________________________
*/

 asfHeader::~asfHeader(  )
{
  close();
}
/*
    __________________________________________________________
*/

uint8_t asfHeader::open(char *name)
{
  _fd=fopen(name,"rb");
  if(!_fd)
  {
    GUI_Error_HIG("File Error.","Cannot open file\n");
    return 0; 
  }
  myName=ADM_strdup(name);
  if(!getHeaders())
  {
    return 0; 
  }
  buildIndex();
  fseeko(_fd,_dataStartOffset,SEEK_SET);
  _packet=new asfPacket(_fd,_packetSize,&readQueue);
  curSeq=1;
  return 1;
}
/*
    __________________________________________________________
*/

 
/*
    __________________________________________________________
*/

  uint8_t  asfHeader::setFlag(uint32_t frame,uint32_t flags)
{
  ADM_assert(frame<nbImage);
  _index[frame].flags=flags;
  return 1; 
}
/*
    __________________________________________________________
*/

uint32_t asfHeader::getFlags(uint32_t frame,uint32_t *flags)
{
  ADM_assert(frame<nbImage);
  if(!frame) *flags=AVI_KEY_FRAME;
  else 
      *flags=_index[frame].flags;
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t  asfHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                                     uint32_t *flags)
{
  *framelen=0;
  if(flags)
    *flags=AVI_KEY_FRAME;
  if(framenum>=nbImage)
  {
    printf("[ASF] Going out of bound %u %u\n",framenum, nbImage);
  }
  if(!_index[framenum].frameLen)
  {
    return 1; // Empty frame 
  }
  // In case curSeq is stored as one byte..
  curSeq&=0xff;
  //
  uint32_t len=0;
  aprintf("Framenum %u curSeq %u frameSeq=%u packetnb=%u \n",
          framenum,curSeq,_index[framenum].segNb,_index[framenum].packetNb);
  // Seeking ?
  if(_index[framenum].segNb!=curSeq)
  {
    uint32_t go;
    asfIndex *idx=&_index[framenum];
    go=idx->packetNb;
    if(go>=1) go--;
    go=_dataStartOffset+go*_packetSize;
    printf("Seeking packetnb=%u start=%x end=%u (0x%x)\n",idx->packetNb,_dataStartOffset,go,go);
    
    fseeko(_fd,go,SEEK_SET);
    // Flush queue
    while(!readQueue.isEmpty())
    {
      asfBit *bit;
      ADM_assert(readQueue.pop((void**)&bit));
      delete bit;
    }
    curSeq=_index[framenum].segNb;
    printf("Seeking starting at seq=%u\n",curSeq);
  }
  
  
  len=0;
  uint32_t delta;
  while(1)
  {
   
    while(!readQueue.isEmpty())
    {
      asfBit *bit;
      ADM_assert(readQueue.pop((void**)&bit));
      aprintf(">found packet of size %d seq %d, while curseq =%d\n",bit->len,bit->sequence,curSeq);
      // Backward ?
      delta=256+bit->sequence-_index[framenum].segNb;
      delta &=0xff;
      if(_index[framenum].segNb > bit->sequence  && delta!=1)
      {
        printf("Dropping seq=%u too old for %u delta %d\n",bit->sequence,_index[framenum].segNb,delta);
        delete bit;
        continue; 
      }
      if(bit->sequence!=curSeq )
      {
        aprintf("New sequence %u->%u while loading %u frame\n",curSeq,bit->sequence,framenum);
        *framelen=len;
        readQueue.pushBack(bit);
        curSeq=bit->sequence;
        goto gotcha;
      }
      // still same sequence ...add
      memcpy(ptr+len,bit->data,bit->len);
      delete bit;
      len+=bit->len;
    }
    if(!_packet->nextPacket(_videoStreamId))
    {
      printf("[ASF] Packet Error\n");
      return 0; 
    }
    _packet->skipPacket();
  }
gotcha:
  
  *framelen=len;
  if(len!=_index[framenum].frameLen)
  {
    printf("[ASF] Frame=%u :-> Mismatch found len : %u expected %u\n",framenum,len, _index[framenum].frameLen);
  }
  aprintf(">>Len %d seq %d\n",len,curSeq);
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t  asfHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
  uint32_t flags;
  return getFrameNoAlloc(framenum,ptr,framelen,&flags); 
}
/*******************************************
  Read Headers to collect information 
********************************************/
uint8_t asfHeader::getHeaders(void)
{
  uint32_t i=0,nbSubChunk,hi,lo;
  const chunky *id;
  uint8_t gid[16];
  uint32_t mn=0,mx=0;
  asfChunk chunk(_fd);
  // The first header is header chunk
  chunk.nextChunk();
  id=chunk.chunkId();
  if(id->id!=ADM_CHUNK_HEADER_CHUNK)
  {
    printf("[ASF] expected header chunk\n"); 
    return 0;
  }
  printf("[ASF] getting headers\n");
  chunk.dump();
  nbSubChunk=chunk.read32();
  printf("NB subchunk :%u\n",nbSubChunk);
  chunk.read8();
  chunk.read8();
  for(i=0;i<nbSubChunk;i++)
  {
    asfChunk *s=new asfChunk(_fd);
    
    s->nextChunk();
    printf("***************\n");  
    id=s->chunkId();
    s->dump();
    switch(id->id)
    {
      case ADM_CHUNK_FILE_HEADER_CHUNK:
        {
            // Client GID
            printf("Client        :");
            for(int z=0;z<16;z++) printf(":%02x",s->read8());
            printf("\n");
            printf("File size     : %08lx\n",s->read64());
            printf("Creation time : %08lx\n",s->read64());
            printf("Number of pack: %08lx\n",s->read64());
            printf("Timestamp 1   : %08lx\n",s->read64());
            printf("Timestamp 2   : %08lx\n",s->read64());
            printf("Timestamp 3   : %04x\n",s->read32());
            printf("Preload       : %04x\n",s->read32());
            printf("Flags         : %04x\n",s->read32());
            mx=s->read32();
            mn=s->read32();
            if(mx!=mn)
            {
              printf("Variable packet size!!\n");
              delete s;
              return 0; 
            }
            _packetSize=mx;
            printf("Min size      : %04x\n",mx);
            printf("Max size      : %04x\n",mn);
            printf("Uncompres.size: %04x\n",s->read32());
          }
          break;
      case ADM_CHUNK_STREAM_HEADER_CHUNK:
      {
         // Client GID
        uint32_t audiovideo=0; // video=1, audio=2, 0=unknown
        uint32_t sid;
        s->read(gid,16);
        printf("Type            :");
        for(int z=0;z<16;z++) printf("0x%02x,",gid[z]);
        if(!memcmp(gid,asf_video,16))
        {
          printf("(video)");
          audiovideo=1;
        } else
        {
          if(!memcmp(gid,asf_audio,16))
          {
            printf("(audio)"); 
            audiovideo=2;
          } else printf("(? ? ? ?)"); 
        }
        printf("\nConceal       :");
        for(int z=0;z<16;z++) printf(":%02x",s->read8());
        printf("\n");
        printf("Reserved    : %08x\n",s->read64());
        printf("Total Size  : %04x\n",s->read32());
        printf("Size        : %04x\n",s->read32());
        sid=s->read16();
        printf("Stream nb   : %04x\n",sid);
        printf("Reserved    : %04x\n",s->read32());
        switch(audiovideo)
        {
          case 1: // Video
          {
                    _videoStreamId=sid;
                    loadVideo(s);
                    break;
          }
              break;
          case 2: // audio
              break;
          default:break; 
          
        }
        
      }
      break;
       default:
         break;
    }
    s->skipChunk();
    delete s;
  }
  printf("End of headers\n");
  return 1;
}
uint8_t asfHeader::loadVideo(asfChunk *s)
{
  uint32_t w,h,x;
            w=s->read32();
            h=s->read32();
            s->read8();
            x=s->read16();
            _isvideopresent=1;

            memset(&_mainaviheader,0,sizeof(_mainaviheader));
            _mainaviheader.dwWidth=w;
            _mainaviheader.dwHeight=h;
            _video_bih.biWidth=w;
            _video_bih.biHeight=h;
            printf("Pic Width  %04d\n",w);
            printf("Pic Height %04d\n",h);
            printf(" BMP size  %04d (%04d)\n",x,sizeof(BITMAPINFOHEADER));
            s->read((uint8_t *)&_video_bih,sizeof(BITMAPINFOHEADER));
            _videostream.dwScale=1000;
            _videostream.dwRate=30000;

            _videostream.fccHandler=_video_bih.biCompression;
            printf("Codec : <%s> (%04x)\n",
                    fourCC::tostring(_video_bih.biCompression),_video_bih.biCompression);
            
            printBih(&_video_bih);
            if(x>sizeof(BITMAPINFOHEADER))
            {
              _extraDataLen=x-sizeof(BITMAPINFOHEADER);
              _extraData=new uint8_t[_extraDataLen];
              s->read(_extraData,_extraDataLen);
            }
            return 1;
}
/*
    Scan the file to build an index
    
    Header Chunk
            Chunk
            Chunk
            Chunk
            
    Data chunk
            Chunk
            Chunk
            
    We skip the 1st one, and just read the header of the 2nd one
    
*/
uint8_t asfHeader::buildIndex(void)
{
  uint32_t fSize;
  const chunky *id;
  uint32_t chunkFound;
  uint32_t r=5;
  uint32_t nbPacket=0,len;
  
  fseeko(_fd,0,SEEK_END);
  fSize=ftello(_fd);
  fseeko(_fd,0,SEEK_SET);
  
  asfChunk h(_fd);
  printf("[ASF] Building index\n");
  printf("[ASF] Searching data\n");
  while(r--)
  {
    h.nextChunk();    // Skip headers
    id=h.chunkId();
    h.dump();
    if(id->id==ADM_CHUNK_DATA_CHUNK) break;
    h.skipChunk();
  }
  if(id->id!=ADM_CHUNK_DATA_CHUNK) return 0;
  // Remove leftover from DATA_chunk
 // Unknown	GUID	16
//       Number of packets	UINT64	8
//       Unknown	UINT8	1
//       Unknown	UINT8	1
//   
  h.read32();
  h.read32();
  h.read32();
  h.read32();
  nbPacket=(uint32_t) h.read64();
  h.read16();
  
  len=h.chunkLen-16-8-2-24;
  
  printf("[ASF] nbPacket  : %u\n",nbPacket);
  printf("[ASF] len to go : %u\n",len);
  printf("[ASF] scanning data\n");
  _dataStartOffset=ftello(_fd);
  
  // Here we go
  DIA_working *working=new DIA_working("indexing asf");
  asfPacket *aPacket=new asfPacket(_fd,_packetSize,&readQueue);
  uint32_t packet=0;
  uint32_t sequence=1;
  
  nbImage=0;
  asfIndex *tmpIndex=new asfIndex[nbPacket*3];
  memset(tmpIndex,0,sizeof(asfIndex)*nbPacket);
  len=0;
  
  while(packet<nbPacket)
  {
    while(!readQueue.isEmpty())
    {
      asfBit *bit=NULL;
      ADM_assert(readQueue.pop((void**)&bit));
      aprintf(">found packet of size %d seq %d, while curseq =%d\n",bit->len,bit->sequence,curSeq);
      if(bit->sequence!=sequence)
      {
        tmpIndex[nbImage].frameLen=len;
        aprintf("New sequence\n");
        if( ((sequence+1)&0xff)!=(bit->sequence&0xff))
        {
            printf("!!!!!!!!!!!! non continuous sequence %u %u\n",sequence,bit->sequence); 
#if 1            
            // Let's insert a couple of null frame
            int32_t delta,start,end;
            
            start=256+bit->sequence-sequence-1;
            start&=0xff;
            printf("!!!!!!!!!!!! Delta %d\n",start);
            
            for(int filler=0;filler<start;filler++)
            {
              tmpIndex[++nbImage].frameLen=0;
            }
#endif            
        }
        nbImage++;
        ADM_assert(nbImage<nbPacket*3);
        tmpIndex[nbImage].frameLen=0;
        tmpIndex[nbImage].segNb=bit->sequence;
        tmpIndex[nbImage].packetNb=packet;
        tmpIndex[nbImage].flags=bit->flags;
        readQueue.pushBack(bit);

        sequence=bit->sequence;
        len=0;
        continue;
      }
      len+=bit->len;
      delete bit;
    }
    working->update(packet,nbPacket);

    packet++;
    aPacket->nextPacket(_videoStreamId);
    aPacket->skipPacket();
  }
  delete aPacket;
  delete working;
  /* Compact index */
  _index=new asfIndex[nbImage];
  memcpy(_index,tmpIndex,sizeof(asfIndex)*nbImage);
  delete [] tmpIndex;
  
  fseeko(_fd,_dataStartOffset,SEEK_SET);
  printf("[ASF]%u images found\n",nbImage);
  _videostream.dwLength=_mainaviheader.dwTotalFrames=nbImage;
  return 1;
  
}
//EOF
