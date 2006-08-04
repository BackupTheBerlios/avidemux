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
  return 0; 
}
/*
    __________________________________________________________
*/

uint32_t asfHeader::getFlags(uint32_t frame,uint32_t *flags)
{
  *flags=AVI_KEY_FRAME;
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
  asfChunk chunk(_fd);
  // The first header is header chunk
  chunk.nextChunk();
  id=chunk.chunkId();
  
  if(id->id!=ADM_CHUNK_HEADER_CHUNK)
  {
    printf("[ASF] expected header chunk\n"); 
    return 0;
  }
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
            printf("Min size      : %04x\n",s->read32());
            printf("Max size      : %04x\n",s->read32());
            printf("Uncompres.size: %04x\n",s->read32());
          }
          break;
      case ADM_CHUNK_STREAM_HEADER_CHUNK:
      {
         // Client GID
        uint32_t audiovideo=0; // video=1, audio=2, 0=unknown
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
        printf("Stream nb   : %04x\n",s->read16());
        printf("Reserved    : %04x\n",s->read32());
        switch(audiovideo)
        {
          case 1: // Video
          {
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
            _videostream.dwLength=_mainaviheader.dwTotalFrames=5;
            printBih(&_video_bih);
            if(x>sizeof(BITMAPINFOHEADER))
            {
              _extraDataLen=x-sizeof(BITMAPINFOHEADER);
              _extraData=new uint8_t[_extraDataLen];
              s->read(_extraData,_extraDataLen);
            }
            return 1;
}

//EOF
