/***************************************************************************
                          audioac3ex.cpp  -  description
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
//#include <sstream>
#include <assert.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "aviaudio.hxx"
#include "ADM_audio/audioex.h"

#include "ADM_audio/ADM_a52info.h"

uint8_t AVDMAC3AudioStream::open(char *name)
{
uint8_t syncbuff[10*1024];
uint32_t fq,br,l,chan;
uint32_t syncoff;


    fd = fopen(name, "rb");
    if (!fd)
		return 0;
	l=fread(syncbuff,10*1024,1,fd);
 	if(!l)
  		{
        	abort();
         	return 0;
    	}

    // read wavheader
    _wavheader = new WAVHeader;
    assert(_wavheader);
    if(! 		ADM_AC3GetInfo(syncbuff, 10*1024,&fq, &br,&chan,&syncoff)   )
    		{
            	abort();
             	printf("\n could not sync ac3!\n");
             	return 0;
        	}
    // else fill up wav header
    _wavheader->encoding = WAV_AC3;
    _wavheader->channels = 2;
    _wavheader->frequency = fq;
    _wavheader->bitspersample = 16;	// yes i know
    _wavheader->byterate = br;
    _wavheader->blockalign = 4;
    // now update length field
    //_________________________
    fseek(fd, 0, SEEK_END);
    _length = ftell(fd);
    //
    //

       _codec=getAudioCodec(WAV_AC3);
     assert(_codec);
    return 1;

}


