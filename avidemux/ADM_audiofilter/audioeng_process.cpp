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
    memmove(&_incomingBuffer[_head],&_incomingBuffer[_tail],sizeof(float)*(_tail-_head));
    _tail-=_head;
    _head=0;
  }
  return 1;
}
