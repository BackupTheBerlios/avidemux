/***************************************************************************
        Sox
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

#include "ADM_assert.h"

#include "ADM_default.h"

#include "audioeng_process.h"
#include "audiofilter_sox.h"

AUDMAudioFilterSox::AUDMAudioFilterSox(AUDMAudioFilter *instream,uint32_t  tgt):AUDMAudioFilter (instream)
{
  _resamples=NULL;
  targetFrequency=tgt;
  _previous->rewind();     // rewind
  printf("[Sox] Creating from %d Hz to %d Hz\n",_wavHeader.frequency,targetFrequency);
  if(_wavHeader.frequency==targetFrequency)
  {
    engaged=0;
    return;
  }
    engaged=1;
    _wavHeader.frequency= targetFrequency;
    
    int org=_previous->getInfo()->frequency;
    _resamples=new ResampleStruct[_wavHeader.channels];
    for(int i=0;i<_wavHeader.channels;i++)
    {
      printf("[Sox Resample] from %d to %d for track %i\n",org,targetFrequency,i);
      ADM_assert( sox_init(org,targetFrequency,&(_resamples[i]))) ;
    }
};

AUDMAudioFilterSox::~AUDMAudioFilterSox()
{
  printf("[Sox] Destroying\n");
  if(_resamples)
  {
    for(int i=0;i<_wavHeader.channels;i++)
      sox_stop(&(_resamples[i]));
    delete [] _resamples;
    _resamples=NULL;
  }
};

#define BLK_SIZE 500 // Block size in sample
//_____________________________________________
uint32_t AUDMAudioFilterSox::fill(uint32_t max,float *output,AUD_Status *status)
{

  uint32_t nbout,snbout=0,nb_in;
  uint32_t nbBlockIn,nbi;
  double d;
  
  if(!engaged)
  {
    return _previous->fill(max, output,status); 
    
  }

    // Compute how much block we want
    d= _wavHeader.frequency;
    d=_previous->getInfo()->frequency/d;
    d*=max;
    d/=BLK_SIZE;
    nbBlockIn=(uint32_t)floor(d);
    if(!nbBlockIn)
    { 
      printf("[Resampler]Asked less than one block..(%u)\n"),max;
      return 0;
    }
    
    for(int j=0;j<nbBlockIn;j++)
    {
      // Fill incoming buffer
        shrink();
        fillIncomingBuffer(status);
        if(_head==_tail)
        {
          *status=AUD_END_OF_STREAM;
          return snbout;
        }
        ADM_assert(_tail>=_head);
        nb_in=(_tail-_head)/(_wavHeader.channels); // Nb Sample
        if(nb_in>BLK_SIZE) nb_in=BLK_SIZE;
        if(!nb_in)
        {
          printf("[Resampler]Not enough audio\n");
          break; 
        }
        // We have one BLK_SIZE incoming
        for(int i=0;i<_wavHeader.channels;i++)
        {
          nbout=BLK_SIZE*8;;    // 8Khz->48khz worst case ?
          nbi=nb_in;
          if(!sox_run(&(_resamples[i]), _incomingBuffer+i+_head,  output+i,    &nbi, &nbout,_wavHeader.channels-1))
          {
            printf("[Resampler] run error!!\n");
            return 0;
          }
        }
        if(nbi>nb_in)
        {
          printf("[Resampler]%u max %u\n",nbi,nb_in);
          ADM_assert(0); 
        }
      _head=_head+(nbi*_wavHeader.channels);
      snbout+=nbout*_wavHeader.channels;
      output+=nbout*_wavHeader.channels;
    }
    return snbout;
}
//EOF
