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

#include "ADM_amv.h"

#define vprintf(...) {}
#if 0
/**
    \fn ~flvAudio
    
*/
 flvAudio ::~flvAudio()
{
  
  if(_fd) fclose(_fd);
  _fd=NULL;
}

uint32_t    flvAudio::read(uint32_t len,uint8_t *buffer)
{
  uint32_t l,sam;
  if(!getPacket(buffer, &l, &sam)) return 0;
  return l;
}
/**
      \fn goTo
*/
uint8_t             flvAudio::goTo(uint32_t newoffset)
{
  goToBlock(0);
  return 1;
}
/**
      \fn extraData
*/
uint8_t             flvAudio::extraData(uint32_t *l,uint8_t **d)
{
  *l=0;
  *d=NULL;
}
/**
    \fn getPacket
*/
uint8_t             flvAudio::getPacket(uint8_t *dest, uint32_t *packlen, uint32_t *samples)
{
  uint32_t t;
  return  getPacket(dest, packlen,samples,&t);
}
uint8_t             flvAudio::getPacket(uint8_t *dest, uint32_t *packlen, uint32_t *samples,uint32_t *timecode)
{
  flvIndex *x;
    if(!goToBlock(_curBlock))
    {
      printf("[FLVAUDIO] Get packet out of bound\n");
      return 0;
    }
    x=&(_track->_index[_curBlock]);
    fread(dest,x->size,1,_fd);
    *packlen=x->size;
    *timecode=x->timeCode;
    int delta=0;
    if(_curBlock<_track->_nbIndex-1)
    {
      delta= _track->_index[_curBlock+1].timeCode-x->timeCode;
    }
    else
      delta=_track->_index[1].timeCode-_track->_index[0].timeCode;
    // convert delta in ms to delta in sample
    // 
    float f=delta;
    f=f*_wavheader->frequency;
    f/=1000;
    *samples=(uint32_t)floor(f+0.49);
    _curBlock++;
    //
    return 1;
}
/**
      \fn flvAudio 
*/
   flvAudio::flvAudio(const char *name,flvTrak *track,WAVHeader *hdr)
  : AVDMGenericAudioStream()
{
  _destroyable = 0; // Will be destroyed with master track
  _track=track;
    ADM_assert(_track);
  _fd=fopen(name,"rb");
  ADM_assert(_fd);

  _wavheader=new WAVHeader;
  memcpy(_wavheader,hdr,sizeof(WAVHeader));
  
  // Compute total length in byte
  _length=0;
  // 
  for(int i=0;i<_track->_nbIndex;i++)
    _length+=_track->_index[i].size;
  printf("[FLVAUDIO] found %lu bytes\n",_length);
  // Guess bitrate
  uint32_t duration=_track->_index[_track->_nbIndex-1].timeCode;
  printf("[FLVAUDIO] Duration %u ms\n",duration);
  
  if(duration)
  {
    float f=_length;
    f/=duration;
    uint32_t bitrate,byterate;
    

    
    bitrate=(uint32_t)f*8; // kbits/s
    byterate=(uint32_t)(f*1000);         // byte/s
    
    printf("[FLVAUDIO] Byterate %u / %u kbps\n",byterate,bitrate); 
    _wavheader->byterate=byterate;
  }
  
  //
  
  goToBlock(0);
}
/**
    \fn goToCluster
    \brief Change the cluster parser...
*/
uint8_t flvAudio::goToBlock(uint32_t x)
{
  if(x>=_track->_nbIndex)
  {
    printf("[FLVAUDIO]Exceeding max cluster : asked: %u max :%u\n",x,_track->_nbIndex); 
    return 0;  // FIXME
  }
  _curBlock=x;
  fseeko(_fd,_track->_index[_curBlock].pos,SEEK_SET);
  return 1;
}
/**
      \fn goToTime
      \brief seek in the current track until we meet the time give (in ms)
*/
 uint8_t	flvAudio::goToTime(uint32_t mstime)
{
uint32_t target=mstime;
uint32_t _nbClusters=_track->_nbIndex;

      // First identify the cluster...
      int clus=-1;
            for(int i=0;i<_nbClusters-1;i++)
            {
              if(target>=_track->_index[i].timeCode && target<_track->_index[i+1].timeCode)
              {
                clus=i;
                i=_nbClusters; 
              }
            }
            if(clus==-1) clus=_nbClusters-1; // Hopefully in the last one
            goToBlock(clus);
            return 1;
}

//EOF
#endif
