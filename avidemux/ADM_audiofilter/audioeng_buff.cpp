/***************************************************************************
                          audioeng_buff.cpp  -  description

  	That is a derivated class to handle generically buffered input/output
	
                             -------------------
    begin                : Thu Feb 7 2002
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
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
//#include "aviaudio.hxx"
#include "audioprocess.hxx"
//#include "../toolkit.hxx"
//
AVDMBufferedAudioStream::AVDMBufferedAudioStream(AVDMGenericAudioStream * instream):AVDMProcessAudioStream
    (instream)
{


    _headBuff=_tailBuff=0;
}

//
//
uint32_t AVDMBufferedAudioStream::read(uint32_t len, uint8_t * buffer)
{
    uint32_t avail = 0;
    uint32_t got=0;
    uint32_t grabbed;
    uint8_t  *myBuffer;
    myBuffer=(uint8_t *)internalBuffer;
    // this disable the filter
    // return _instream->read(len,buffer);

more:
    

    // have we already got what's needed ?
    avail=_tailBuff-_headBuff;
 //    printf("\n asked : %lu, available : %lu",len,avail);  
    // got everything ?
    if (avail >= len)
      {	
	  memcpy(buffer, myBuffer+_headBuff, len);
	  _headBuff+=len;	  
	  got+=len;
//	  printf("Got : %lu\n",got);
	  return got;
      }
    // first empty what was available
    memcpy(buffer, myBuffer+_headBuff, avail);
    
    got+=avail;
    len -= avail;
    buffer+=avail;
    _headBuff+=avail;
    
   
    // rewind to beginning
    _tailBuff=_headBuff=0;
		    
    //printf("\n grabing...");
    grabbed = grab(myBuffer);
  //  printf("grabbed :%lu\n",grabbed);
    // Minus one means we could not get a single byte
    if (grabbed == MINUS_ONE)
      {
	  printf("\n ** end stream **\n");
	  _tailBuff=_headBuff=0;	
	  return got;		// we got everything  !
      }
    _tailBuff+=grabbed;    
    goto more;
};

// EOF
