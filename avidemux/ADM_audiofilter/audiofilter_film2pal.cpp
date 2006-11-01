/***************************************************************************
                          audiofilterFilm2pal
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
#include "avifmt2.h"

#include "ADM_toolkit/toolkit.hxx"
//#include "ADM_dialog/DIA_working.h"
#include "audioeng_process.h"
#include "audiofilter_film2pal.h"


#define PAL_SAM  25025
#define FILM_SAM 24000

//__________

AUDMAudioFilterFilm2Pal::AUDMAudioFilterFilm2Pal(AUDMAudioFilter * instream):AUDMAudioFilter (instream)
{
    // nothing special here...
  _target=0.0;
  _previous->rewind();
  printf("[Film2Pal] Creating\n");
};

AUDMAudioFilterFilm2Pal::~AUDMAudioFilterFilm2Pal()
{
  printf("[Film2Pal] Destroying\n");
}
//
//___________________________________________
uint32_t AUDMAudioFilterFilm2Pal::fill( uint32_t max, float * buffer,AUD_Status *status)
{
  uint32_t len,i,rendered;
  uint32_t min=_wavHeader.channels;
  float *start;
  
  shrink();
  fillIncomingBuffer(status);
  float *head;
  
  
  shrink();
  fillIncomingBuffer(status);
  
  len=_tail-_head;
  if(len>max) len=max;
  
  len=len/min;
  len*=min;

  rendered=0;
  start=&_incomingBuffer[_head];
  while(len>=min)
  {
    memcpy(buffer,start,min*sizeof(float));
    buffer+=min;
    start+=min;
    rendered+=min;
    len-=min;
    _target+=(PAL_SAM-FILM_SAM);
    _head+=min;
    //_______________________________________
    // Remove sample
    // To compensate
    //_______________________________________
    // Fps higher=we have to accelerate sound = remove samples
    // The compression factor is 1-((25-24)/25)	
    while(_target>PAL_SAM && rendered>=min)
    {	
      rendered-=min;	
      buffer-=min;
      _target=_target-PAL_SAM;
    }
	
  }
  return rendered;
};
//****************************************************
AUDMAudioFilterPal2Film::AUDMAudioFilterPal2Film(AUDMAudioFilter * instream):AUDMAudioFilter (instream)
{
    // nothing special here...
  _target=0.0;
  _previous->rewind();
  printf("[Pal2Film] Creating\n");
};

AUDMAudioFilterPal2Film::~AUDMAudioFilterPal2Film()
{
  printf("[Film2Pal] Creating\n");
}
//
//___________________________________________
uint32_t AUDMAudioFilterPal2Film::fill( uint32_t max, float * buffer,AUD_Status *status)
{
  uint32_t len,i,rendered;
  uint32_t min=_wavHeader.channels;
  float *start;
  
  shrink();
  fillIncomingBuffer(status);
  float *head;
  
  
  shrink();
  fillIncomingBuffer(status);
  
  len=_tail-_head;
  if(len>max) len=max;
  
  len=len/min;
  len*=min;

  rendered=0;
  start=&_incomingBuffer[_head];
  while(len)
  {
        memcpy(buffer,start,min*sizeof(float));
        buffer+=min;
        start+=min;
        rendered+=min;
        len-=min;
        _target+=(PAL_SAM-FILM_SAM);
        //_______________________________________
        // Duplicate sample
        // To compensate
        //_______________________________________
        // In : One second is worth FILM_SAM samples
        // Out:FILM_SAM+(PAL_SAM-FILM_SAM)*FILM_SAM/FILM_SAM=PAL_SAM = 25 fps
      
        while(_target>FILM_SAM)
        {
          memcpy(buffer,start-min,min*sizeof(float));
          buffer+=min;
          rendered+=min;			
          _target=_target-FILM_SAM;
        }
  }
  return rendered;
};
//EOF

