/***************************************************************************
                          Base class for Mpeg Demuxer
                             -------------------
                
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

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include <ADM_assert.h>



#include "dmx_demuxerEs.h"
 
dmx_demuxerES::dmx_demuxerES(void)
{
        consumed=0;
        parser=new fileParser();
        stampAbs=0;
}
dmx_demuxerES::~dmx_demuxerES()
{
        if(parser) delete parser;
        parser=NULL;
}
uint8_t dmx_demuxerES::open(char *name)
{
        if(! parser->open(name)) return 0;
        _size=parser->getSize();
        return 1;
}
uint8_t dmx_demuxerES::forward(uint32_t f)
{
        consumed+=f;
        return parser->forward(f);
}
uint8_t  dmx_demuxerES::stamp(void)
{
        consumed=0;
        parser->getpos(&stampAbs);
        stampAbs-=4;
}
uint8_t dmx_demuxerES::getStamp( uint64_t *a,uint64_t *r)
{
        *r=0;
        *a=stampAbs;
        return 1;
}
uint64_t dmx_demuxerES::elapsed(void)
{
        return consumed;        
}
uint8_t  dmx_demuxerES::getPos( uint64_t *abs,uint64_t *rel)
{
        *rel=0;
        parser->getpos(abs);
        abs-=4; //
        return 1;
}
uint8_t dmx_demuxerES::setPos( uint64_t abs,uint64_t  rel)
{
               return parser->setpos(abs);
}

          
