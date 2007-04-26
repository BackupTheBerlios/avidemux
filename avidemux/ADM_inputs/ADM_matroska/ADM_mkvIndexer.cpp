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
   for(int i=0;i<_nbAudioTrack+1;i++)
   {
    _tracks[i]._indexMax=1000;
    _tracks[i]._index=new mkvIndex[_tracks[i]._indexMax];
   }
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
                        uint32_t lacing,nbLaces;
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
                                      int track=searchTrackFromTid(tid);
                                      //printf("Wanted %u got %u\n",_tracks[0].streamIndex,tid);
                                      if(track==-1)
                                      {
                                        blockGroup.skip(blockGroup.remaining());
                                        break;  // Not our track
                                      }
                                      // Skip timecode
                                      blockGroup.skip(2); // Timecode
                                      uint8_t flags=blockGroup.readu8();
                                      uint32_t remaining=blockGroup.remaining();
                                      lacing=((flags>>1)&3);
                                      switch(lacing)
                                      {
                                        case 0: // No lacing
                                              addVideoEntry(track,blockGroup.tell(),remaining);
                                              break;
                                        case 2 : // Constant size lacing
                                            {
                                                    nbLaces=blockGroup.readu8()+1;
                                                    remaining--;
                                                    // Only mp3/Ac3 supported, ignore lacing FIXME : Vorbis or AAC will not work
                                                    addVideoEntry(track,blockGroup.tell(),remaining);
                                                    int bsize=remaining/nbLaces;
                                                    if(bsize*nbLaces!=remaining)
                                                    {
                                                      printf("Warning not multiple bsize=%d total=%u  nbLaces=%u\n",bsize,remaining,nbLaces);
                                                    }
                                                    if(!track) printf("Warning lacing on video track\n");
                                                    //printf("tid:%u track %u Remaining : %llu laces %u blksize %d er%d\n",tid,track,remaining,nbLaces,remaining/nbLaces,remaining-(remaining/nbLaces)*nbLaces);
                                            } 
                                        break;
                                        default: 
                                            printf("unsupported lacing\n");
                                            break;
                                      }
                                      blockGroup.skip(remaining);
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
uint8_t mkvHeader::addVideoEntry(uint32_t track,uint64_t where, uint32_t size)
{
  //
  mkvTrak *Track=&(_tracks[track]);
  
  
  // Need to grow index ?
  if(Track->_nbIndex==Track->_indexMax-1)
  {
    // Realloc 
    mkvIndex *dx=new mkvIndex[Track->_indexMax*2];
    memcpy(dx, Track->_index,sizeof(mkvIndex)*Track->_nbIndex);
    Track->_indexMax*=2;
    delete [] Track->_index;
    Track->_index=dx;
  }
  
  mkvIndex *index=Track->_index;
  int x=Track->_nbIndex;
  index[x].pos=where;
  index[x].size=size;
  index[x].flags=0;
  index[x].timeCode=0;
  Track->_nbIndex++;
 // printf("++\n");
  return 1;
}

/**
  \fn searchTrackFromTid
  \brief Returns our track number for the stream track number. -1 if the stream is not handled.

*/
int mkvHeader::searchTrackFromTid(uint32_t tid)
{
  for(int i=0;i<1+_nbAudioTrack;i++)
  {
    if(tid==_tracks[i].streamIndex) return i;
  }
  return -1;
}

/**
  \fn readCue
  \brief Update index with cue content

*/
uint8_t                 mkvHeader::readCue(ADM_ebml_file *parser)
{
  uint64_t fileSize,len,bsize;
  uint32_t alen,vlen;
  uint64_t id;
  ADM_MKV_TYPE type;
  const char *ss;
  uint64_t time;
  uint64_t segmentPos;
  
   parser->seek(0);
   
   if(!parser->simplefind(MKV_SEGMENT,&vlen,1))
   {
     printf("[MKV] Cannot find CLUSTER atom\n");
     return 0;
   }
   ADM_ebml_file segment(parser,vlen);
   segmentPos=segment.tell();
   
   while(segment.simplefind(MKV_CUES,&alen,0))
  {
   ADM_ebml_file cues(&segment,alen);
   while(!cues.finished())
   {
      cues.readElemId(&id,&len);
      if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
      {
        printf("[MKV] Tag 0x%x not found (len %llu)\n",id,len);
        cues.skip(len);
        continue;
      }
      if(id!=MKV_CUE_POINT)
      {
        printf("Found %s in CUES, ignored \n",ss); 
        cues.skip(len);
        continue;
      }
      ADM_ebml_file cue(&cues,len);
      // Cue TIME normally
       cue.readElemId(&id,&len);
       if(id!=MKV_CUE_TIME)
       {
          ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type);
          printf("Found %s(0x%x), expected CUE_TIME  (0x%x)\n", ss,id,MKV_CUE_TIME);
          cue.skip(cue.remaining());
          continue;
       }
       time=cue.readUnsignedInt(len);
       
       
       cue.readElemId(&id,&len);
       if(id!=MKV_CUE_TRACK_POSITION)
       {
          ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type);
          printf("Found %s (0x%x), expected MKV_CUE_TRACK_POSITION (0x%x)\n", ss,id,MKV_CUE_TRACK_POSITION);
          cue.skip(cues.remaining());
          continue;
       }
       ADM_ebml_file trackPos(&cue,len);
       uint64_t tid=0;
       uint64_t cluster_position=0;
       while(!trackPos.finished())
       {
         trackPos.readElemId(&id,&len);
         switch(id)
         {
           case MKV_CUE_TRACK: tid=trackPos.readUnsignedInt(len);break;
           case MKV_CUE_CLUSTER_POSITION: cluster_position=trackPos.readUnsignedInt(len);break;
           default: 
                 ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type);
                 printf("[MKV] in cluster position found tag %s (0x%x)\n",ss,id);
                 trackPos.skip(len);
                 continue;
         }
       }
       printf("Track %u Position 0x%llx time %llu final pos:%llx \n",tid,cluster_position,time,
             cluster_position+segmentPos );
     }
   }
   printf("[MKV] Cues updated\n");
   return 1;  
}
//EOF