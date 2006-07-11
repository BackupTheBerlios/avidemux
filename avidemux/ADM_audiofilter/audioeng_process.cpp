/***************************************************************************
                          audioeng_process
                             -------------------
    copyright            : (C) 2002/2006 by mean
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
#include <math.h>

#include <ADM_assert.h>

#include "avifmt.h"
#include "audioprocess.hxx"

#include "ADM_toolkit/toolkit.hxx"
#include "ADM_dialog/DIA_working.h"
#include "audioeng_process.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"



AUDMAudioFilter::AUDMAudioFilter(AUDMAudioFilter *previous)
{
  _previous=previous;
  if(_previous)
      memcpy(&_wavHeader,_previous->getInfo(),sizeof(_wavHeader));
  _head=_tail=0; 
}
AUDMAudioFilter::~AUDMAudioFilter()
{
}

uint8_t  AUDMAudioFilter::rewind(void)
{
  _head=_tail=0;
  return _previous->rewind();
}

uint8_t AUDMAudioFilter::shrink(void)
{
  if(_tail>AUD_PROCESS_BUFFER_SIZE/2)
  {
    memmove(&_incomingBuffer[0],&_incomingBuffer[_head],sizeof(float)*(_tail-_head));
    _tail-=_head;
    _head=0;
  }
  return 1;
}
/*
    If the incoming data is getting low (less than 1/4) fill it up 
*/
uint8_t AUDMAudioFilter::fillIncomingBuffer(AUD_Status *status)
{
  uint32_t asked;
  *status=AUD_OK;
  // Hysteresis
  if((_tail-_head)<(AUD_PROCESS_BUFFER_SIZE>>2)) // Less than 1/4 full
 {

  while ((  _tail < (3*AUD_PROCESS_BUFFER_SIZE)/5)) // Fill up to 3/5--3/4
  {
      // don't ask too much front.
    asked = (3*AUD_PROCESS_BUFFER_SIZE)/4-_tail;
    //asked = _incoming->readDecompress(asked, &(_incomingBuffer[_tail]));
    asked=_previous->fill(asked,&(_incomingBuffer[_tail]),status);

    if (!asked )
    {
      *status=AUD_END_OF_STREAM;
      break;
    }
    _tail+=asked;
  }
 }
 return 1;
}


WAVHeader  *AUDMAudioFilter::getInfo(void)
{
  return &(_wavHeader);
}
//EOF