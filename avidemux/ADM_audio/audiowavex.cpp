/***************************************************************************
                          audiowavex.cpp  -  description
                             -------------------
    begin                : Fri May 31 2002
    copyright            : (C) 2002 by mean
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
#include <assert.h>
#include <math.h>


#include "config.h"
#include "fourcc.h"
#include "avifmt.h"
#include "avifmt2.h"

#include "aviaudio.hxx"
#include "ADM_audio/audioex.h"
/*---------------------------------------

-------------------------------------------*/

/*_______________________________________
  	Try to open the Wav file
		Gather informations
_________________________________________*/
uint8_t AVDMWavAudioStream::open(char *name)
{
    uint32_t t32;
    fd = fopen(name, "rb");
    if (!fd)
	return 0;
    // it is a riff, but we wont use riff parser
    t32 = read32();
    if (!fourCC::check( t32, (uint8_t *) "RIFF"))
      {
	  printf("Not riff.\n");
	  fourCC::print(t32);	
	  goto drop;
      }
    _totallen = read32();
    printf("\n %lu bytes total \n", _totallen);

    t32 = read32();
    if (!fourCC::check( t32, (uint8_t *) "WAVE"))
      {
	  printf("\n no wave chunk..aborting..\n");
	  goto drop;
      }
    t32 = read32();
    if (!fourCC::check( t32, (uint8_t *) "fmt "))
      {
	  printf("\n no fmt chunk..aborting..\n");
	  goto drop;
      }
    t32 = read32();
    if (t32 < sizeof(WAVHeader))
      {
	  printf("\n incorrect fmt chunk..(%ld/%d)\n",t32,sizeof(WAVHeader));
	  goto drop;
      }
    // read wavheader
    _wavheader = new WAVHeader;
    assert(_wavheader);
    if (fread(_wavheader, sizeof(WAVHeader), 1, fd) != 1)
      {
	  printf("\n Error reading FMT chunk...");
	  goto drop;
      }
      
      // For our big endian friends
      Endian_WavHeader(_wavheader);
      //
    fseek(fd,t32-sizeof(WAVHeader),SEEK_CUR);  
    t32 = read32();
    if (!fourCC::check( t32, (uint8_t *) "data"))
      {
	  printf("\n no data chunk..aborting..\n");
	  goto drop;
      }

    _length = read32();
    printf("\n %lu bytes data \n", _totallen);
    _offset = ftell(fd);
    printf("\n %lu offset \n", _offset);
    _wavheader->blockalign=1;
     _codec=getAudioCodec(WAV_PCM);
     assert(_codec);
    return 1;
  drop:
    abort();
    return 0;
}


