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


/*
    __________________________________________________________
*/

asfAudio::~asfAudio()
{
  printf("[asfAudio] Destroying track\n");
}
/*
    __________________________________________________________
*/

asfAudio::asfAudio(char *name,asfAudioTrak *track)
{
  printf("[asfAudio] Creating track\n");
   _trackIndex=track->streamIndex; 
   _wavheader=&(track->wavHeader);
  _extraDataLen=track->extraDataLen;
  _extraData=track->extraData;
  _length=track->length;
  
  
}
/*
    __________________________________________________________
*/

uint32_t            asfAudio::read(uint32_t len,uint8_t *buffer)
{
  uint32_t lan,samples;
  if(!getPacket(      buffer,    &lan, &samples)) return 0;
  return lan;
}
/*
    __________________________________________________________
*/

uint8_t   asfAudio::goTo(uint32_t newoffset)
{
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t   asfAudio::goToTime(uint32_t newoffset)
{
  return 1; 
}
/*
    __________________________________________________________
*/

uint8_t   asfAudio::extraData(uint32_t *l,uint8_t **d)
{
  if(_extraDataLen)
  {
    *l=_extraDataLen;
    *d=_extraData;  
  }
  else
  {
    *l=0;
    *d=NULL; 
  }
  return 1;
}
/*
    __________________________________________________________
*/

uint8_t  asfAudio::getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)
{
  
  
  return 0; 
}
//EOF
