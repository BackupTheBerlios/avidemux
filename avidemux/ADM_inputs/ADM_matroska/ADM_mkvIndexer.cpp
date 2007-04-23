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
#define VIDEO _tracks[0]
/**
    \fn videoIndexer
    \brief index the video Track
*/
uint8_t mkvHeader::videoIndexer(ADM_ebml_file *parser)
{
  uint64_t fileSize,len,bsize;
  uint32_t alen,vlen;
  uint64_t id;
  ADM_MKV_TYPE type;
  const char *ss;
  
   parser->seek(0);
   
   // Start with a small index, it will grow automatically afterward
    VIDEO._indexMax=1000;
    VIDEO._index=new mkvIndex[VIDEO._indexMax];
    //************
   
   if(!parser->find(ADM_MKV_PRIMARY,MKV_SEGMENT,MKV_CLUSTER,&vlen))
   {
     printf("[MKV] Cannot find CLUSTER atom\n");
     return 0;
   }
  
  ADM_ebml_file segment(parser,vlen);
  printf("[MKV] Segment found at %llx (size %llx)\n",segment.tell(),vlen);
  
  //while FIXME LOOP ON ALL CLUSTER!
  while(segment.find(ADM_MKV_PRIMARY,MKV_CLUSTER,MKV_CLUSTER,&alen,0))
  {
  //  printf("[MKV] Beginning new cluster at 0x%llx\n",segment.tell());
   ADM_ebml_file cluster(&segment,alen);
   while(!cluster.finished())
   {
      cluster.readElemId(&id,&len);
      if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
      {
        printf("[MKV] Tag 0x%x not found (len %llu)\n",id,len);
        cluster.skip(len);
        continue;
      }
      //printf("\t\tFound %s\n",ss);
      switch(id)
      {
                default: 
                case MKV_TIMECODE: 
                  //printf("Skipping %s\n",ss);
                  cluster.skip(len);
                  break;
                
                case MKV_BLOCK_GROUP:
                {
                        //printf("Block Group\n");
                        ADM_ebml_file blockGroup(parser,len);
                        while(!blockGroup.finished())
                        {
                                blockGroup.readElemId(&id,&len);
                                if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
                                {
                                  printf("[MKV] Tag 0x%x not found (len %llu)\n",id,len);
                                  blockGroup.skip(len);
                                  continue;
                                }
                                //printf("\t\t\t\tFound %s\n",ss);
                                //printf(">%s\n",ss);
                                switch(id)
                                {
                                  default: blockGroup.skip(len);
                                  case MKV_BLOCK : 
                                  {
                                    // Read Track id 
                                      uint32_t tid=blockGroup.readEBMCode();
                                      //printf("Wanted %u got %u\n",_tracks[0].streamIndex,tid);
                                      if(_tracks[0].streamIndex!=tid)
                                      {
                                        blockGroup.skip(blockGroup.remaining());
                                        break;  // Not our track
                                      }
                                      // Skip timecode
                                      blockGroup.skip(3);
                                      addVideoEntry(blockGroup.tell(),blockGroup.remaining());
                                      blockGroup.skip(blockGroup.remaining());
                                      break;
                                  }
                                }
                          }
            }
            break; // Block Group
       }
     }
   // printf("[MKV] ending cluster at 0x%llx\n",segment.tell());
  }
     printf("Found %lu images in this cluster\n",VIDEO._nbIndex);
     return 1;
}
/**
    \fn addVideoEntry
    \brief add an entry to the video index
*/
uint8_t mkvHeader::addVideoEntry(uint64_t where, uint32_t size)
{
  // Need to grow index ?
  if(VIDEO._nbIndex==VIDEO._indexMax-1)
  {
    // Realloc 
    mkvIndex *dx=new mkvIndex[VIDEO._indexMax*2];
    memcpy(dx, _tracks[0]._index,sizeof(mkvIndex)*VIDEO._nbIndex);
    VIDEO._indexMax*=2;
    delete [] _tracks[0]._index;
    _tracks[0]._index=dx;
  }
  
  mkvIndex *index=_tracks[0]._index;
  int x=VIDEO._nbIndex;
  index[x].pos=where;
  index[x].size=size;
  index[x].flags=0;
  index[x].timeCode=0;
  VIDEO._nbIndex++;
 // printf("++\n");
  return 1;
}