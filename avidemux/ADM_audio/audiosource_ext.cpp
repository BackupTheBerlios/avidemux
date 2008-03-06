/***************************************************************************
                          audiosource_ext.cpp  -  description
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
#include "ADM_assert.h"
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "fourcc.h"
#include "aviaudio.hxx"
#include "ADM_audio/audiosource_ext.h"

AVDMFileStream::AVDMFileStream( void )
  {
    	_pos=0;
     	fd=NULL;
      _wavheader=NULL;
      	_offset=_totallen=_audiolen=0;
        _destroyable = 1;
    }
AVDMFileStream:: ~AVDMFileStream( )
{
		abort();
}
void AVDMFileStream::abort( void )
{
      if (fd)
    {
		fclose(fd);
  		fd=NULL;
  	}
    if (_wavheader)
      {
	  ADM_dealloc(_wavheader);
	  _wavheader = NULL;
      }
}
uint32_t AVDMFileStream::read32(void)
{
    uint32_t val;
    uint8_t buf[5];

    ADM_assert(fd);
    if(fread(buf, 4, 1, fd) != 1) return 0; // hopefully it will fail somehere

	val=buf[0]+(buf[1]<<8)+(buf[2]<<16)+(buf[3]<<24);

    return val;

}
/*_______________________________________
  	Read n bytes
_________________________________________*/
uint32_t AVDMFileStream::read(uint32_t len, uint8_t * buffer)
{
    uint32_t rd;
    ADM_assert(fd);

    rd = fread(buffer, 1, len, fd);
    _pos+=rd;
    return rd;
}

/*_______________________________________
  	seek to nth byte
_________________________________________*/
uint8_t AVDMFileStream::goTo(uint32_t newoffset)
{
    ADM_assert(fd);
    if(0==fseek(fd, newoffset + _offset, SEEK_SET))
    {
	   _pos=newoffset;
     	return 1;
     }
    return 0;
}




