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
#include <stdlib.h>



#include <string.h>
//#include <stream.h>
#include <math.h>

#include "config.h"
#include "avifmt.h"
#include "avifmt2.h"
#include "avio.hxx"
#include <ADM_assert.h>

#include "fourcc.h"
#include "riffparser.h"
#include "subchunk.h"
#include "avilist.h"
#include "ADM_toolkit/toolkit.hxx"

// initialize List

AviList::AviList(const char *name, ADMFile * f)
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
    
    _begin=_ff->tell();
    Write32((_fcc));
    Write32((uint32_t) 0);	// size
    Write32((fourCC::get((uint8_t *) subchunk)));
    return 1;
}

// Mark End
uint8_t AviList::End(void)
{
    uint32_t len, b, e;

    

    e=_ff->tell();
    _ff->seek(_begin);
    b=_ff->tell();

    len = (1 + e - b) & 0xfffffffe;
    len -= 8;



    Write32((_fcc));
    Write32((len));
    _ff->seek(e);
    return 1;

}

uint32_t AviList::TellBegin(void)
{
    return _begin;

}

uint32_t AviList::Tell(void)
{
        return _ff->tell();

}

//
//  Io stuff
//
uint8_t AviList::Write32(uint32_t val)
{
#ifdef ADM_BIG_ENDIAN
	val=R32(val);
#endif
        _ff->write((uint8_t *)&val,4);
        return 1;
}

uint8_t AviList::Write(uint8_t * p, uint32_t len)
{
        return _ff->write(p,len);
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

    fcc = fourCC::get(chunkid);
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
