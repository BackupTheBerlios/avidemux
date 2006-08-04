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
  _context=NULL;
  _nbAudioTrack=0;
  _audioTracks=NULL;
  myName=NULL;
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
  int err;
  Dump();
  return 0;
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
 
  return 0; 
}
/*
    __________________________________________________________
*/

uint8_t  asfHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
  uint32_t flags;
  return getFrameNoAlloc(framenum,ptr,framelen,&flags); 
}
//EOF
