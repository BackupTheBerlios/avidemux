/***************************************************************************
                          audioenf_lpcm.cpp  -  description
                             -------------------
                Lpcm codec (so simple)

    copyright            : (C) 2005 by mean
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stream.h>
#include <ADM_assert.h>
#include <math.h>

#include "config.h"


#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "audioprocess.hxx"
#include "audioeng_lpcm.h"
// Ctor: Duplicate
//__________

AVDMProcessAudio_Lpcm::AVDMProcessAudio_Lpcm(AVDMGenericAudioStream * instream):AVDMBufferedAudioStream (instream)
{
    _wavheader = new WAVHeader;

    memcpy(_wavheader, _instream->getInfo(), sizeof(WAVHeader));
    _wavheader->encoding = WAV_LPCM;
    strcpy(_name, "PROC:LPCM");
    _instream->goToTime(0);     // rewind
    _length = _instream->getLength();
  
    
};

AVDMProcessAudio_Lpcm::~AVDMProcessAudio_Lpcm()
{
    delete(_wavheader);
};


//_____________________________________________
uint32_t AVDMProcessAudio_Lpcm::grab(uint8_t * obuffer)
{

    uint32_t rd = 0, rdall = 0;
    uint8_t *in,*out;
    int32_t nbout;
    uint32_t asked, done;
#undef ONE_CHUNK
#define ONE_CHUNK (8192)
//      
// Read n samples
    // Go to the beginning of current block
    in = _bufferin;
    while (rdall < ONE_CHUNK)
      {
          // don't ask too much front.
          asked = ONE_CHUNK - rdall;
          rd = _instream->read(asked, in + rdall);
          rdall += rd;
          if (rd == 0)
              break;
      }
    // Block not filled
    if (rdall != ONE_CHUNK)
      {
          printf("\n not enough...%lu\n", rdall);
          if (rdall == 0)
              return MINUS_ONE; // we could not get a single byte ! End of stream
          // Else fillout with 0
          memset(in + rdall, 0, ONE_CHUNK - rdall);
      }
        in=_bufferin;
        out=obuffer;
        for(int i=0;i<rdall>>1;i++)
        {
                out[1]=in[0];
                out[0]=in[1];
                in+=2;
                out+=2;
        }
    return rdall;
}
