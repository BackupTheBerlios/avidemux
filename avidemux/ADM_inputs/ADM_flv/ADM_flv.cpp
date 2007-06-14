/***************************************************************************
    copyright            : (C) 2007 by mean
    email                : fixounet@free.fr
    
      See lavformat/flv[dec/env].c for detail
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

#include "default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "fourcc.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_flv.h"

enum {
    FLV_TAG_TYPE_AUDIO = 0x08,
    FLV_TAG_TYPE_VIDEO = 0x09,
    FLV_TAG_TYPE_META  = 0x12,
};
/**
    \fn Skip
    \brief Skip some bytes from the file
*/
uint8_t flvHeader::Skip(uint32_t len)
{
  fseeko(_fd,len,SEEK_CUR);
  return 1; 
}
/**
    \fn read
    \brief read the given size from file
*/

uint8_t flvHeader::read(uint32_t len, uint8_t *where)
{
  
    uint32_t got=fread(where,1,len,_fd);
    if(len!=got)
    {
      printf("[FLV] Read error : asked %u, got %u\n",len,got); 
      return 0;
    }
    return 1; 
}
//__________________________________________________________
uint8_t flvHeader::read8(void)
{
  uint8_t r;
    fread(&r,1,1,_fd);
    return r; 
}
uint32_t flvHeader::read16(void)
{
  uint8_t r[2];
    fread(r,2,1,_fd);
    return (r[0]<<8)+r[1]; 
}
uint32_t flvHeader::read24(void)
{
  uint8_t r[3];
    fread(r,3,1,_fd);
    return (r[0]<<16)+(r[1]<<8)+r[2]; 
}
uint32_t flvHeader::read32(void)
{
  uint8_t r[4];
    fread(r,4,1,_fd);
    return (r[0]<<24)+(r[1]<<16)+(r[2]<<8)+r[3]; 
}

/*
    __________________________________________________________
*/

uint8_t flvHeader::open(char *name)
{
  
  
  _isvideopresent=0;
 
  _filename=ADM_strdup(name);
  _fd=fopen(name,"rb");
  if(!_fd)
  {
    printf("[FLV] Cannot open %s\n",name);
    return 0; 
  }
  // It must begin by F L V 01
  uint8_t four[4];
  
  read(4,four);
  if(four[0]!='F' || four[1]!='L' || four[2]!='V')
  {
     printf("[FLV] Not a flv file %s\n",name);
    return 0; 
  }
  // Next one is flags
  uint32_t flags=read8();
  if(flags & 1) // VIDEO
  {
    _isvideopresent=1;
    printf("[FLV] Video flag\n");
  }
  if(flags & 4) // Audio
  {
    printf("[FLV] Audio flag\n");
  }
  // Skip header
  uint32_t skip=read32();
  fseeko(_fd,skip,SEEK_SET);
  
  
  // Loop
  while(!feof(_fd))
  {
    uint32_t prevLen, type, size, pts,pos;
    pos=ftello(_fd);
    prevLen=read32();
    type=read8();
    size=read24();
    pts=read24();
    read32(); // ???
    printf("[FLV] At %08x found type %x size %u pts%u\n",pos,type,size,pts);
    printf("Flags :%x\n",read8());
    Skip(size-1);
  }
  
  
  printf("[FLV]FLV successfully read\n");
  
  return 0;
}
/*
  __________________________________________________________
*/
WAVHeader *flvHeader::getAudioInfo(void )
{
  return NULL;
}
/*
    __________________________________________________________
*/

uint8_t flvHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
  *audio=NULL;
  return 0; 
}
/*
    __________________________________________________________
*/

void flvHeader::Dump(void)
{
 
}
/*
    __________________________________________________________
*/

uint32_t flvHeader::getNbStream(void) 
{
  return 0; 
}
/*
    __________________________________________________________
*/

uint8_t flvHeader::close(void)
{
  if(_filename) ADM_dealloc(_filename);
  if(videoTrack) delete videoTrack;
  if(audioTrack) delete audioTrack;
  if(_fd) fclose(_fd);
    _fd=NULL;
    _filename=NULL;
    videoTrack=NULL;
    audioTrack=NULL;

}
/*
    __________________________________________________________
*/

uint8_t flvHeader::needDecompress(void)
{
  return 1; 
}
/*
    __________________________________________________________
*/

 flvHeader::flvHeader( void ) : vidHeader()
{ 
    _fd=NULL;
    _filename=NULL;
    videoTrack=NULL;
    audioTrack=NULL;
    
}
/*
    __________________________________________________________
*/

 flvHeader::~flvHeader(  )
{
  close();
}

/*
    __________________________________________________________
*/

/*
    __________________________________________________________
*/

  uint8_t  flvHeader::setFlag(uint32_t frame,uint32_t flags)
{
}
/*
    __________________________________________________________
*/

uint32_t flvHeader::getFlags(uint32_t frame,uint32_t *flags)
{
}
/*
    __________________________________________________________
*/

uint8_t  flvHeader::getFrameNoAlloc(uint32_t framenum,ADMCompressedImage *img)
{
}
/*
    __________________________________________________________
*/

uint8_t  flvHeader::getExtraHeaderData(uint32_t *len, uint8_t **data)
{
                *len=0; //_tracks[0].extraDataLen;
                *data=NULL; //_tracks[0].extraData;
                return 1;            
}
/*
    __________________________________________________________
*/
uint8_t  flvHeader::changeAudioStream(uint32_t newstream)
{
    return 0;
}
/*
    __________________________________________________________
*/
uint32_t  flvHeader::getCurrentAudioStreamNumber(void)
{
  return 0;
}
/**
    \fn getAudioStreamsInfo
    \brief returns infos about audio streams (code,...)
    @param nbStreams (out) nb audio streams
    @param infos (out) pointer to streams info. It is up to the caller to free them.
*/
uint8_t  flvHeader::getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **infos)
{
    
    {
        *nbStreams=0;
        *infos=NULL;
        return 1;
    }
    
}
//EOF
