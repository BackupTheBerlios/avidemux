/***************************************************************************
                          avilist.cpp  -  description
                             -------------------
    begin                : Thu Nov 15 2001
    copyright            : (C) 2001 by mean
    email                : fixounet@free.fr

	This class deals with LIST chunk

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
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#ifdef __FreeBSD__
          #include <sys/types.h>
#endif
#include <stdlib.h>
#include <ADM_assert.h>



#include <string.h>
//#include <stream.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include "fourcc.h"
#include "riffparser.h"
#include "subchunk.h"
#include "avilist.h"
#include "ADM_toolkit/toolkit.hxx"

// initialize List

AviList::AviList(const char *name, FILE * f)
{
    _fcc = fourCC::get((uint8_t *) name);
    ADM_assert(_fcc);
    _ff = f;
    ADM_assert(_ff);
    //_begin=_end=(fpos_t )0;
}

// Mark begin
uint8_t AviList::Begin(const char *subchunk)
{
    fgetpos(_ff, &_begin);
#ifdef NO_LARGE_FILE
    _begin_tell = (uint32_t) ftell(_ff);
#else
    _begin_tell = (uint32_t) ftello(_ff);
#endif
    Write32((_fcc));
    Write32((uint32_t) 0);	// size
    Write32((fourCC::get((uint8_t *) subchunk)));
    return 1;
}

// Mark End
uint8_t AviList::End(void)
{
    uint32_t len, b, e;


//      ADM_assert(_begin);
    fgetpos(_ff, &_end);
#ifdef NO_LARGE_FILE
    e = (uint32_t) ftell(_ff);
#else
    e = (uint32_t) ftello(_ff);
#endif

    fsetpos(_ff, &_begin);
#ifdef NO_LARGE_FILE
    b = (uint32_t) ftell(_ff);
#else
    b = (uint32_t) ftello(_ff);
#endif

    len = (1 + e - b) & 0xfffffffe;
    len -= 8;



    Write32((_fcc));
    Write32((len));
    fsetpos(_ff, &_end);
    return 1;

}

uint32_t AviList::TellBegin(void)
{
    return _begin_tell;

}

uint32_t AviList::Tell(void)
{
#ifdef NO_LARGE_FILE
    return (uint32_t)ftell(_ff);;
#else
    return (uint32_t)ftello(_ff);;
#endif

}

//
//  Io stuff
//
uint8_t AviList::Write32(uint32_t val)
{
#ifdef ADM_BIG_ENDIAN
	val=R32(val);
#endif
   if(! fwrite(&val, 4, 1, _ff))

   	{
       	//
        	//printf("\n Write error : %d",ferror(_ff));

      }

    return 1;
}

uint8_t AviList::Write(uint8_t * p, uint32_t len)
{
   if(! fwrite(p, len, 1, _ff))
   	{
       	//
        	//printf("\n Write error : %d",ferror(_ff));

      }
    return 1;
}

uint8_t AviList::Write32(uint8_t * c)
{
    uint32_t fcc;
    fcc = fourCC::get(c);
    ADM_assert(fcc);
    Write32(fcc);
    return 1;
}

uint8_t AviList::WriteChunk(uint8_t * chunkid, uint32_t len, uint8_t * p)
{
    uint32_t fcc;
//static uint8_t spacer[10]={0,0,0,0,0,0,0,0,0,0};
    fcc = fourCC::get(chunkid);
//
//              len=(len+1) & 0xfffffffe;
    ADM_assert(fcc);
    Write32(fcc);
    Write32(len);
    Write(p, len);
    if (len & 1)
      {				// pad to be a multiple of 4, nicer ...
	  Write(p, 1);
      }
    return 1;
}
