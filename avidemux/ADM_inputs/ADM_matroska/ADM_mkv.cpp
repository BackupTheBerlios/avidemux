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

#include "default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "fourcc.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_mkv.h"

#include "mkv_tags.h"
/*
    __________________________________________________________
*/

uint8_t mkvHeader::open(char *name)
{
 
  ADM_ebml_file ebml;
  uint64_t id,len;
  uint32_t alen;
  ADM_MKV_TYPE type;
  const char *ss;
  
  _isvideopresent=0;
  if(!ebml.open(name)) 
  {
    printf("[MKV]Failed to open file\n");
    return 0; 
  }
  if(!ebml.find(ADM_MKV_PRIMARY,EBML_HEADER,(MKV_ELEM_ID)0,&alen))
  {
    printf("[MKV] Cannot find header\n");
    return 0;
  }
  if(!checkHeader(&ebml,alen))
  {
     printf("[MKV] Incorrect Header\n");
     return 0;
  }

  /* Now find tracks */
  if(!ebml.find(ADM_MKV_SECONDARY,MKV_SEGMENT,MKV_TRACKS,&alen))
  {
     printf("[MKV] Cannot find tracks\n");
    return 0;
  }
  /* And analyze them */
  if(!analyzeTracks(&ebml,alen))
  {
      printf("[MKV] incorrect tracks\n");
  }
  printf("[MKV] Tracks analyzed\n");
  if(!_isvideopresent)
  {
    printf("[MKV] No video\n");
    return 0;
  }
  printf("[MKV] Indexing video\n");
    if(!videoIndexer(&ebml))
    {
      printf("[MKV] Video indexing failed\n");
      return 0; 
    }
  // update some infos
  _videostream.dwLength= _mainaviheader.dwTotalFrames=_tracks[0]._nbIndex; 
  printf("Matroska successfully read\n");
  _parser=new ADM_ebml_file();
  ADM_assert(_parser->open(name));
  return 1;
}
/**
    \fn checkHeader
    \brief Check that we are compatible with that version of matroska. At the moment, just dump some infos.
*/
uint8_t mkvHeader::checkHeader(void *head,uint32_t headlen)
{
  printf("[MKV] *** Header dump ***\n");
 ADM_ebml_file father( (ADM_ebml_file *)head,headlen);
 walk(&father);
 printf("[MKV] *** End of Header dump ***\n");
 return 1; 
  
}
/**
    \fn analyzeTracks
    \brief Read Tracks Info.
*/
uint8_t mkvHeader::analyzeTracks(void *head,uint32_t headlen)
{
  uint64_t id,len;
  ADM_MKV_TYPE type;
  const char *ss;
 ADM_ebml_file father( (ADM_ebml_file *)head,headlen);
 while(!father.finished())
 {
      father.readElemId(&id,&len);
      if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
      {
        printf("[MKV] Tag 0x%x not found (len %llu)\n",id,len);
        father.skip(len);
        continue;
      }
      ADM_assert(ss);
      if(id!=MKV_TRACK_ENTRY) 
      {
        printf("[MKV] skipping %s\n",ss);
        father.skip(len);
        continue; 
      }
      if(!analyzeOneTrack(&father,len)) return 0;
 }
 return 1; 
}

/**
    \fn walk
    \brief Walk a matroska atom and print out what is found.
*/
uint8_t mkvHeader::walk(void *seed)
{
  uint64_t id,len;
  ADM_MKV_TYPE type;
  const char *ss;
  
   ADM_ebml_file *father=(ADM_ebml_file *)seed;
    while(!father->finished())
   {
      father->readElemId(&id,&len);
      if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
      {
        printf("[MKV] Tag 0x%x not found (len %llu)\n",id,len);
        father->skip(len);
        continue;
      }
      ADM_assert(ss);
      switch(type)
      {
        case ADM_MKV_TYPE_CONTAINER:
                  father->skip(len);
                  printf("%s skipped\n",ss);
                  break;
        case ADM_MKV_TYPE_UINTEGER:
                  printf("%s:%llu\n",ss,father->readUnsignedInt(len));
                  break;
        case ADM_MKV_TYPE_INTEGER:
                  printf("%s:%lld\n",ss,father->readSignedInt(len));
                  break;
        case ADM_MKV_TYPE_STRING:
        {
                  char string[len+1];
                  string[0]=0;
                  father->readString(string,len);
                  printf("%s:<%s>\n",ss,string);
                  break;
        }
        default:
                printf("%s skipped\n",ss);
                father->skip(len);
                break;
      }
   }
  return 1;
}
/**

*/
/*
  __________________________________________________________
*/
WAVHeader *mkvHeader::getAudioInfo(void )
{

  return NULL;
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
  *audio=NULL;
  return 0; 
}
/*
    __________________________________________________________
*/

void mkvHeader::Dump(void)
{
 
}
/*
    __________________________________________________________
*/

uint32_t mkvHeader::getNbStream(void) 
{
  return 0; 
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::close(void)
{
  // CLEANUP!!
  if(_parser) delete _parser;
  _parser=NULL;
  // FIXME Cleanup index!
}
/*
    __________________________________________________________
*/

uint8_t mkvHeader::needDecompress(void)
{
  return 1; 
}
/*
    __________________________________________________________
*/

 mkvHeader::mkvHeader( void ) : vidHeader()
{ 
  _parser=NULL;
  _nbAudioTrack=0;
  memset(_tracks,0,sizeof(_tracks));
}
/*
    __________________________________________________________
*/

 mkvHeader::~mkvHeader(  )
{
  close();
}

/*
    __________________________________________________________
*/

/*
    __________________________________________________________
*/

  uint8_t  mkvHeader::setFlag(uint32_t frame,uint32_t flags)
{
  return 0; 
}
/*
    __________________________________________________________
*/

uint32_t mkvHeader::getFlags(uint32_t frame,uint32_t *flags)
{
  if(frame>=_tracks[0]._nbIndex) return 0;
  *flags=_tracks[0]._index[frame].flags;
  if(!frame) *flags=AVI_KEY_FRAME;
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t  mkvHeader::getFrameNoAlloc(uint32_t framenum,ADMCompressedImage *img)
{
  ADM_assert(_parser);
  if(framenum>=_tracks[0]._nbIndex) return 0;
  
  mkvIndex *dx=&(_tracks[0]._index[framenum]);
  
  _parser->seek(dx->pos);
  _parser->readBin(img->data,dx->size);
  img->dataLength=dx->size;
  
  img->flags=dx->flags;
  
  if(!framenum) img->flags=AVI_KEY_FRAME;
  
  
  return 1; 
}


//EOF
