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
#if 0
class mkvAudio : public AVDMGenericAudioStream
{
  protected:
    mkvTrak                     *_track;
    
  public:
                                mkvAudio(mkvTrak *track);
    virtual                     ~mkvAudio();
    virtual uint32_t            read(uint32_t len,uint8_t *buffer);
    virtual uint8_t             goTo(uint32_t newoffset);
    virtual uint8_t             getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
    virtual uint8_t             goToTime(uint32_t mstime);
    virtual uint8_t             extraData(uint32_t *l,uint8_t **d);
};
#endif

/**
    \fn ~mkvAudio
    
*/
 mkvAudio ::~mkvAudio()
{
  if(_parser) delete _parser;
  _parser=NULL; 
  _offset=0;
  _index=0;
}

uint32_t    mkvAudio::read(uint32_t len,uint8_t *buffer)
{
  uint32_t red=0;
  while(1)
  {
    //********************************************
    if(_index>=_track->_nbIndex)
    {
      return 0; 
    }
    //********************************************
    if(len+_offset <=_track->_index[_index].size)
    {
      _parser->readBin(buffer,len);
      _offset+=len;
      return len; 
    }
    // empty...
    //********************************************
    int32_t remaining=_track->_index[_index].size-_offset;
    ADM_assert(remaining>=0);
    
    _parser->readBin(buffer,remaining);
    red+=remaining;
    len-=remaining;
    buffer+=remaining;
    _index++;
    if(_index>=_track->_nbIndex)
    {
      return red; 
    }
    _parser->seek(_track->_index[_index].pos);
    _offset=0;
  }
}
/**
      \fn goTo
*/
uint8_t             mkvAudio::goTo(uint32_t newoffset)
{
  _parser->seek(_track->_index[0].pos);
    _offset=0;
    _index=0;
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
uint8_t             mkvAudio::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
    *samples=1152;
    *len=read(384,dest);
    if(*len) return 1;
    return 0;
}
/**
      \fn mkvAudio
*/
 mkvAudio::mkvAudio(const char *name,mkvTrak *track) : AVDMGenericAudioStream()
{
   _parser=new ADM_ebml_file();
   ADM_assert(_parser->open(name));
  _track=track; 
  
  // Compute total length in byte
  _length=0;
  for(int i=0;i<track->_nbIndex;i++)
  {
    _length+=track->_index[i].size; 
  }
  _wavheader=new WAVHeader;
  memcpy(_wavheader,&(_track->wavHeader),sizeof(WAVHeader));
  printf("[MKVAUDIO] found %lu bytes\n",_length);
}
//EOF
