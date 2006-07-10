 
/***************************************************************************
    copyright            : (C) 2002-6 by mean
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
#include "avifmt2.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"

AUDMEncoder::AUDMEncoder(AUDMAudioFilter *in)  :AVDMBufferedAudioStream  (NULL)
{
  _wavheader = new WAVHeader;
  _incoming=in;
  memcpy(_wavheader, _incoming->getInfo(), sizeof(WAVHeader));
  _wavheader->encoding=WAV_AAC;
  _incoming->rewind();	// rewind
  _handle=NULL;
  _extraData=NULL;
  _extraSize=0;
  tmpbuffer=new float[_wavheader->frequency*_wavheader->channels];
  tmphead=tmptail=0;
};
/********************/
AUDMEncoder::~AUDMEncoder()
{
  cleanup();
};
/********************/
uint8_t AUDMEncoder::cleanup(void)
{
  if(_wavheader) delete(_wavheader);
  _wavheader=NULL;

  if(_extraData) delete [] _extraData;
  _extraData=NULL;
  
  if(tmpbuffer) delete [] tmpbuffer;
  tmpbuffer=NULL;
};
/********************/

uint8_t AUDMEncoder::refillBuffer(int minimum)
{
  uint32_t filler=_wavheader->frequency*_wavheader->channels;
  uint32_t nb;
  AUD_Status status;
  
  while(1)
  {
    if(tmptail-tmphead>minimum) return 1;
  
    if(tmphead && tmptail>filler/2)
    {
      memmove(&tmpbuffer[0],&tmpbuffer[tmphead],(tmptail-tmphead)*sizeof(float)); 
      tmptail-=tmphead;
      tmphead=0;
    }
    nb=_incoming->fill( (filler-tmptail)/2,&tmpbuffer[tmptail],&status);
    if(!nb)
    {
      if(status!=AUD_END_OF_STREAM) ADM_assert(0);
      if(tmphead==tmptail) return 0;
      memset(&tmpbuffer[tmptail],0,(minimum-(tmptail-tmphead))*sizeof(float));
      tmptail=tmphead+minimum;
    } else
      tmptail+=nb;
  }
}
// FIXME , incomplete dithering
#warning FIXME, incorrect dithering
uint8_t       AUDMEncoder::dither16(float *start, uint32_t nb)
{
    float dp;
    int16_t *data_int = (int16_t *)start;
    float *data = start;
    uint16_t len = nb;

    for (int i = 0; i < len; i++)
    {
      *data_int = (int16_t)(*data * 32765 );
      data++;
      data_int++;
      }
      return 1;
}
uint32_t AUDMEncoder::read(uint32_t len,uint8_t *buffer)
{
  ADM_assert(0);
  return 0; 
}
uint32_t AUDMEncoder::read(uint32_t len,float *buffer)
{
  ADM_assert(0);
  return 0; 
}
uint32_t AUDMEncoder::grab(float *outbuffer)
{
  ADM_assert(0);
  return 0; 

}
uint32_t AUDMEncoder::grab(uint8_t * obuffer)
{
  uint32_t len,sam;
  if(getPacket(obuffer,&len,&sam))
    return len;
  return MINUS_ONE;
}

//EOF
