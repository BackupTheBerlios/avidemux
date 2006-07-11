/***************************************************************************
              Convert output of decoder to filter api
              (c) 2006 Mean , fixounet@free.fr
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
#include "avifmt2.h"
// Needed for incoming ..
#include "audioprocess.hxx"

#include "audioeng_process.h"

#include "audiofilter_bridge.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME MODULE_AUDIO_FILTER
#include "ADM_toolkit/ADM_debug.h"


AUDMAudioFilter_Bridge::AUDMAudioFilter_Bridge(AUDMAudioFilter *previous,AVDMGenericAudioStream *incoming, uint32_t startInMs) : AUDMAudioFilter(NULL)
{
  _incoming=incoming;
  memcpy(&_wavHeader,_incoming->getInfo(),sizeof(_wavHeader));
  _startTime=startInMs;
  rewind();
  // FIXME : Compute duration in ms
}
AUDMAudioFilter_Bridge::~AUDMAudioFilter_Bridge()
{
}
uint8_t AUDMAudioFilter_Bridge::rewind(void)
{
  return _incoming->goToTime(_startTime);
}
uint32_t   AUDMAudioFilter_Bridge::fill(uint32_t max,float *output,AUD_Status *status)
{
#if 0
  *status=AUD_OK;
  return  _incoming->readDecompress(max, output);
   
#else
  uint32_t asked,asked2,total=0;
  //
  ADM_assert(_tail>=_head);
  shrink();
  ADM_assert(_tail>=_head);
  fillIncomingBuffer(status);
  // Now fill output
  // We could probably skip the buffering step
  // one extra memcpy gained
  uint32_t available;
  ADM_assert(_tail>=_head);
  available=_tail-_head;
  if(available>max) available=max;
  memcpy(output,&(_incomingBuffer[_head]),available*sizeof(float));
  _head+=available;
  if(!available)
  {
    printf("[bridge] No data in %u max %u out %u\n",_tail-_head,max,available);
  }

  return available;
#endif
}

uint8_t AUDMAudioFilter_Bridge::fillIncomingBuffer(AUD_Status *status)
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
      asked = _incoming->readDecompress(asked, &(_incomingBuffer[_tail]));

      if (!asked )
      {
        *status=AUD_END_OF_STREAM;
        printf("[Bridge] End of stream\n");
        break;
      }
      _tail+=asked;
    }
  }
  return 1;
}

//EOF
