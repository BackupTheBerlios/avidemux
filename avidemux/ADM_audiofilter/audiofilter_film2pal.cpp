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
#include <math.h>

#include "ADM_default.h"

#include "ADM_audioFilter.h"
#include "audiofilter_film2pal.h"

//__________

AUDMAudioFilterFilm2Pal::AUDMAudioFilterFilm2Pal(AUDMAudioFilter * instream):AUDMAudioFilter (instream)
{
    // nothing special here...
  _target=0;
  _removed=0;
  _previous->rewind();
  _modulo=0;
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
  
  len=_tail-_head;
  if(len>max) len=max;
  
  len=len/min;

  rendered=0;
  start=&_incomingBuffer[_head];

  while(len--) // in sample
  {
        //_______________________________________
        // FilmToPal : 24 fps to 25 fps -> shorten frame
        // 1001 samples -> 960 samples
        //______________________________________
       if(_removed) // Fixme , use dither or something
        { 
              _removed--;
        }
        else
        {
        
            memcpy(buffer,start,min*sizeof(float));
            buffer+=min;
            rendered+=min;    
        }
        _target++; // nb Processed
     // Consume it
        start+=min;
        _head+=min;
        _modulo++;
        if(_target>1001)
        {
            _removed+=41;
            _target-=1001;
        }
  }
  return rendered;
};
//****************************************************
AUDMAudioFilterPal2Film::AUDMAudioFilterPal2Film(AUDMAudioFilter * instream):AUDMAudioFilter (instream)
{
    // nothing special here...
  _target=0;
  _removed=0;
  _modulo=0;
  _previous->rewind();
  printf("[Pal2Film] Creating\n");
};

AUDMAudioFilterPal2Film::~AUDMAudioFilterPal2Film()
{
  printf("[Film2Pal] Destroying\n");
}
//
//___________________________________________
uint32_t AUDMAudioFilterPal2Film::fill( uint32_t max, float * buffer,AUD_Status *status)
//uint32_t AUDMAudioFilterFilm2Pal::fill( uint32_t max, float * buffer,AUD_Status *status)
{
  uint32_t len,i,rendered;
  uint32_t min=_wavHeader.channels;
  float *start;
  
  shrink();
  fillIncomingBuffer(status);
  
  ADM_assert(_tail>=_head);
  
  len=_tail-_head;  // How much float available ?
  if(len>max) len=max;
  
  len=len/min; // convert to sample
  
  if(len==0)
  {
        printf("**oops**\n");
  }
  rendered=0;
  start=&_incomingBuffer[_head];
  while(len--)
  {
        memcpy(buffer,start,min*sizeof(float));
       // printf(" Target :%d removed :%d\n",_target,_removed);
        buffer+=min;
        rendered+=min;

       
        _target++; // _target is the number of sample we processed

        _modulo++;

        if(_target>960) // Time to duplicate samples
        {
            _removed+=41;
            _target-=960;
        }
        if(rendered>=max-1)
        {
            start+=min;
            _head+=min;
            return rendered;
        }
        //_______________________________________
        // Pal2Film : 25 fps to 24 fps -> Duplicate samples
        // by 41 samples every 960 samples 960->1001
        //______________________________________
        if(_removed /*&& ((_modulo&15)==1)*/) // Spread the duplication
        {
          memcpy(buffer,start,min*sizeof(float));
          buffer+=min;
          rendered+=min;			
          _removed--;
        }  
        start+=min;
        _head+=min;
  }
  return rendered;
};
//EOF

