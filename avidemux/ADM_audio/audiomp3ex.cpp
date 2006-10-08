/***************************************************************************
                          audiomp3ex.cpp  -  description
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
#include <ADM_assert.h>
#include <math.h>
#include "config.h"

#include "avifmt.h"
#include "aviaudio.hxx"
#include "ADM_audio/audioex.h"      



uint8_t AVDMMP3AudioStream::open(char *name)
{
    //int a, b, c;
	uint8_t buffer[2048],*ptr;
	ptr=buffer;
	
    printf("\n opening stream %s (MP3)",name);
    fd = fopen(name, "rb");
    ADM_assert(fd);
    fread(buffer,2048,1,fd);
     _wavheader = new WAVHeader();
    

    if(! mpegAudioIdentify(ptr, 2048,_wavheader,_mpegSync) )
    {
		delete _wavheader;
		_wavheader=NULL;
		fclose(fd);
                fd=NULL;
		return 0;		
	}
    fseek(fd, 0, SEEK_END);
    _length = ftell(fd);
    //
    //
      _codec=getAudioCodec(_wavheader->encoding,_wavheader);
     ADM_assert(_codec);
     _wavheader->blockalign=1;
    return 1;
}


//EOF
