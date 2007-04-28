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

#define vprintf(...) {}

/**
    \fn ~mkvAudio
    
*/
 mkvAudio ::~mkvAudio()
{
  if(_parser) delete _parser;
  _parser=NULL; 
  if(_clusterParser)  delete _clusterParser;
  _clusterParser=NULL;
}

uint32_t    mkvAudio::read(uint32_t len,uint8_t *buffer)
{
  uint32_t l,sam;
  if(!getPacket(buffer, &l, &sam)) return 0;
  return l;
}
/**
      \fn goTo
*/
uint8_t             mkvAudio::goTo(uint32_t newoffset)
{
  goToCluster(0);
  _currentCluster=0;
  return 1;
}
/**
      \fn extraData
*/
uint8_t             mkvAudio::extraData(uint32_t *l,uint8_t **d)
{
  *l=_track->extraDataLen;
  *d=_track->extraData; 
}
/**
    \fn getPacket
*/
uint8_t             mkvAudio::getPacket(uint8_t *dest, uint32_t *packlen, uint32_t *samples)
{
  uint32_t t;
  return  getPacket(dest, packlen,samples,&t);
}
uint8_t             mkvAudio::getPacket(uint8_t *dest, uint32_t *packlen, uint32_t *samples,uint32_t *timecode)
{
  uint64_t fileSize,len,bsize,pos;
  uint32_t alen,vlen;
  uint64_t id;
  ADM_MKV_TYPE type;
  const char *ss;
  vprintf("Enter: Currently at :%llx\n",_clusterParser->tell());
    // Have we still lace to go ?
    if(_currentLace<_maxLace)
    {
      _clusterParser->readBin(dest,_Laces[_currentLace]);
      *packlen= _Laces[_currentLace];
      vprintf("Continuing lacing : %u bytes, lacing %u/%u\n",*packlen,_currentLace,_maxLace);
      _currentLace++;
      *samples=1024; // FIXME
      *timecode=_curTimeCode;
      
      return 1;
    }
    while(1)
    {
        vprintf("While: Currently at :%llx\n",_clusterParser->tell());
        // need to switch cluster ?
        if(_clusterParser->finished())
        {
          if(!goToCluster(_currentCluster+1))
          {
            printf("[MKVAUDIO] cannot go to next cluster\n");
            return 0; 
          }
          _currentCluster++;
        }
        // Ok read a new block
        while(!_clusterParser->finished())
        {
          
            _clusterParser->readElemId(&id,&len);
            pos=_clusterParser->tell();
            if(!ADM_searchMkvTag( (MKV_ELEM_ID)id,&ss,&type))
            {
              vprintf("[MKVAUDIO] unknown tag %x\n",id);
              _clusterParser->skip(len);
              continue;
            }
            
            vprintf("[MKVAudio]Found tag %x (%s) at 0x%llx len %u\n",id,ss,_clusterParser->tell(),len);
            
            switch(id)
            {
              default:
              case MKV_TIMECODE:     _clusterParser->skip(len);  break;
              case MKV_BLOCK_GROUP:    break;
              case MKV_BLOCK: 
              {
                  uint32_t tid=_clusterParser->readEBMCode();
                  uint64_t tail=pos+len;
                  uint64_t head=pos;
                  
                  // FIXME WARNING ASSUME TRACK FITS ON 1 BYTE!
                  len--; 
                  vprintf("Tid = %u, my tid=%u\n",tid,_track->streamIndex);
                  if(tid!=_track->streamIndex)
                    {
                      _clusterParser->skip(len); // skip this block
                      break; // not our track
                    }
                    // Skip timecode
                    int tc=_clusterParser->readUnsignedInt(2); // FIXME Should be signed
                    if(tc<0) tc=0;
                    _curTimeCode=(uint32_t )tc;
                    len-=2;
                    *timecode=_curTimeCode;
                    uint8_t flags=_clusterParser->readu8();
                    len--;
                    uint32_t remaining=len;
                    uint32_t lacing=((flags>>1)&3);
                    switch(lacing)
                    {
                      case 0 : // no lacing 
                          
                              vprintf("No lacing :%d bytes\n",remaining);
                              _clusterParser->readBin(dest,remaining);
                              *packlen=remaining; 
                              *samples=1024; // FIXME
                              _currentLace=_maxLace=0;
                              
                              return 1;
                      case 2 : // constant size lacing
                              {
                                int nbLaces=_clusterParser->readu8()+1;
                                
                                remaining--;
                                int bsize=remaining/nbLaces;
                                vprintf("NbLaces :%u lacesize:%u\n",nbLaces,bsize);
                                ADM_assert(nbLaces<MKV_MAX_LACES);
                                for(int i=0;i<nbLaces;i++)
                                {
                                  _Laces[i]=bsize; 
                                }
                                _currentLace=0;
                                _maxLace=nbLaces;
                                return getPacket(dest, packlen, samples);
                              }
                      case 3: // Ebml lacing
                        {
                                
                                int nbLaces=_clusterParser->readu8()+1;
                                int32_t curSize=_clusterParser->readEBMCode();
                                int32_t delta;
                                
                                vprintf("Ebml nbLaces :%u lacesize(0):%u\n",nbLaces,curSize);
                                
                                _Laces[0]=curSize;
                                ADM_assert(nbLaces<MKV_MAX_LACES);
                                for(int i=1;i<nbLaces-1;i++)
                                {
                                  delta=_clusterParser->readEBMCode_Signed();
                                  vprintf("Ebml delta :%d lacesize[%d]->:%d\n",delta,i,curSize+delta);
                                  curSize+=delta;
                                  ADM_assert(curSize>0);
                                  _Laces[i]=curSize; 
                                 
                                }
                                uint64_t d=_clusterParser->tell();
                                
                                d=tail-d;
                                /* We have the remaining size after laces, substract the already known lace size */
                                for(int i=0;i<nbLaces-1;i++)
                                {
                                  d-=_Laces[i]; 
                                }
                                // What is left is the sift of the last lace
                                _Laces[nbLaces-1]=(uint32_t)d; 
                                _currentLace=0;
                                _maxLace=nbLaces;
                                return getPacket(dest, packlen, samples);
                              }
                      default:
                            printf("Unsupported lacing %u\n",lacing);
                            _clusterParser->seek(pos+len);
                    }
                
              }
              break;
            }
        }
  }
  return 0;
}
/**
      \fn mkvAudio 
*/
 mkvAudio::mkvAudio(const char *name,mkvTrak *track,mkvIndex *clust,uint32_t nbClusters) 
  : AVDMGenericAudioStream()
{
  _nbClusters=nbClusters;
  _clusters=clust;
  ADM_assert(_clusters);
  
   _parser=new ADM_ebml_file();
   ADM_assert(_parser->open(name));
  _track=track; 
  ADM_assert(_track);
  
  
  // Compute total length in byte
  _length=_track->_sizeInBytes; // FIXME
  
  
  _wavheader=new WAVHeader;
  memcpy(_wavheader,&(_track->wavHeader),sizeof(WAVHeader));
  printf("[MKVAUDIO] found %lu bytes\n",_length);
  _currentLace=_maxLace=0;
  _clusterParser=NULL;
  goToCluster(0);
}
/**
    \fn goToCluster
    \brief Change the cluster parser...
*/
uint8_t mkvAudio::goToCluster(uint32_t x)
{
  ADM_assert(_nbClusters);
  
  if(x>=_nbClusters)
  {
    printf("Exceeding max cluster : asked: %u max :%u\n",x,_nbClusters); 
    return 0;  // FIXME
  }
  
  if( _clusterParser) delete _clusterParser;
  _clusterParser=NULL;
  
  //************/
  _parser->seek(_clusters[x].pos);
  _clusterParser=new ADM_ebml_file(_parser,_clusters[x].size);
  _currentLace=_maxLace=0;
  printf("switching to cluster :%u/%u\n",x,_nbClusters);
  return 1;
}
/**
      \fn goToTime
      \brief seek in the current track until we meet the time give (in ms)
*/
 uint8_t	mkvAudio::goToTime(uint32_t mstime)
{
uint64_t target=mstime;

      // First identify the cluster...
      int clus=-1;
            for(int i=0;i<_nbClusters-1;i++)
            {
              if(target>=_clusters[i].timeCode && target<_clusters[i+1].timeCode)
              {
                clus=i;
                i=_nbClusters; 
              }
            }
            if(clus==-1) clus=_nbClusters-1; // Hopefully in the last one
            
            target-=_clusters[clus].timeCode; // now the time is relative
            goToCluster(clus);
            _currentCluster=clus;
            _curTimeCode=_clusters[clus].timeCode;
            
            printf("[MKVAUDIO] Asked for %u ms, go to cluster %u which starts at %u\n",mstime,clus,_curTimeCode);
            if(clus<_nbClusters-1)
              printf("[MKVAUDIO] next cluster starts at %u\n",_clusters[clus+1].timeCode);
#if 1
            // Now seek more finely
            // will be off by one frame
#define MAX_SEEK_BUFFER 20000
            uint8_t buffer[MAX_SEEK_BUFFER];
            uint32_t len,samples,timecode;
            while(getPacket(buffer, &len, &samples,&timecode))
            {
              uint32_t curTime=_clusters[_currentCluster].timeCode;
              vprintf("Wanted: %u clus : %u Timecode:%u=> %u\n",mstime,curTime,timecode,timecode+curTime);
              ADM_assert(len<MAX_SEEK_BUFFER); 
              
              if(timecode+curTime>=mstime) 
              {
                printf("[MKV audio] fine seek to %u (clu)+%u=%u\n",curTime,timecode,timecode+curTime);
                return 1;
              }
            }
            printf("Failed to seek to %u mstime\n");
            return 0;
#else
            return 1;
#endif
}
//EOF
