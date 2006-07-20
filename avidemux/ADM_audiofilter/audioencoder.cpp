 
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

#include "ADM_library/default.h"

#include "audioprocess.hxx"
#include "audioeng_process.h"
#include "audioencoder.h"

AUDMEncoder::AUDMEncoder(AUDMAudioFilter *in)  :AVDMGenericAudioStream  ()
{
  _wavheader = new WAVHeader;
  _incoming=in;
  memcpy(_wavheader, _incoming->getInfo(), sizeof(WAVHeader));
  _wavheader->encoding=WAV_AAC;
  _incoming->rewind();	// rewind
  _extraData=NULL;
  _extraSize=0;
  tmpbuffer=new float[_wavheader->frequency*_wavheader->channels];
  tmphead=tmptail=0;
  eof_met=0;
  
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
  if(eof_met) return 0;
  while(1)
  {
    ADM_assert(tmptail>=tmphead);
    if((tmptail-tmphead)>=minimum) return 1;
  
    if(tmphead && tmptail>filler/2)
    {
      memmove(&tmpbuffer[0],&tmpbuffer[tmphead],(tmptail-tmphead)*sizeof(float)); 
      tmptail-=tmphead;
      tmphead=0;
    }
    ADM_assert(filler>tmptail);
    nb=_incoming->fill( (filler-tmptail)/2,&tmpbuffer[tmptail],&status);
    if(!nb)
    {
      if(status!=AUD_END_OF_STREAM) ADM_assert(0);
      
      if((tmptail-tmphead)<minimum)
      {
        memset(&tmpbuffer[tmptail],0,sizeof(float)*(minimum-(tmptail-tmphead)));
        tmptail=tmphead+minimum;
        eof_met=1;  
        return minimum;
      }
      else continue;
    } else
      tmptail+=nb;
  }
}

static float rand_table[DITHER_CHANNELS][DITHER_SIZE];

void AUDMEncoder_initDither(void)
{
  printf("Initializing Dithering tables\n");
	float d, dp;
	for (int c = 0; c < DITHER_CHANNELS; c++) {
		dp = 0;
		for (int i = 0; i < DITHER_SIZE-1; i++) {
			d = rand() / (float)RAND_MAX * 2.0 - 1.0;
			rand_table[c][i] = d - dp;
			dp = d;
		}
  		rand_table[c][DITHER_SIZE-1] = 0 - dp;
	}
}

uint8_t AUDMEncoder::dither16(float *start, uint32_t nb)
{
	static uint16_t nr = 0;
	int16_t *data_int = (int16_t *)start;
	float *data = start;
	uint32_t len = nb / _wavheader->channels;

	for (int i = 0; i < len; i++) {
		for (int c = 0; c < _wavheader->channels; c++) {
			if (*data > 1) *data = 1;
			if (*data < -1) *data = -1;
			*data_int = (int16_t)(*data * 32765 + rand_table[c][nr]);
			data++;
			data_int++;
		}
		nr++;
		if (nr > DITHER_SIZE)
			nr = 0;
	}
	return 1;
}

uint32_t AUDMEncoder::read(uint32_t len,uint8_t *buffer)
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
