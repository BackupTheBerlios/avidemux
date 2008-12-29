/**
    \file dmxPSPacket
    \brief Packet demuxer for mpeg PS
    copyright            : (C) 2005-2008 by mean
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
#include "ADM_default.h"
#include "dmxPSPacket.h"

/**
    \fn psPacket
    \brief ctor
*/
psPacket::psPacket(void)
{
    _file=NULL;
}
/**
    \fn psPacket
    \brief dtor
*/
psPacket::~psPacket()
{
    close();
}
/**
    \fn open
    \brief dtor
*/
bool psPacket::open(const char *filenames,bool dontappend)
{
FP_TYPE opentype=FP_APPEND;
    if(dontappend) opentype=FP_DONT_APPEND;
    _file=new fileParser();
    if(!_file->open(filenames,&opentype))
    {
        printf("[psPacket] cannot open %s\n",filenames);
        delete _file;
        _file=NULL;
        return false;
    }
    return true;
}
/**
    \fn close
    \brief dtor
*/
bool psPacket::close(void)
{
    if(_file)
    {
        delete _file;
        _file=NULL;
    }
    return true;
}
/**
    \fn getPos
*/
uint64_t    psPacket::getPos(void)
{
    return 0;
}
/**
    \fn setPos
*/

bool    psPacket::setPos(uint64_t pos)
{
    return true;
}

/**
    \fn getPacketOfType
    \brief Only returns packet of type pid
*/      

bool        psPacket::getPacketOfType(uint8_t pid,uint32_t maxSize, uint32_t *packetSize,uint64_t *pts,uint64_t *dts,uint8_t *buffer)
{

    bool xit=false;
    uint8_t tmppid;
    while(1)
    {
        if(true!=getPacket(maxSize,&tmppid,packetSize,pts,dts,buffer))
                return false;
        else
                if(tmppid==pid) return true;
    }
    return false;
}
/**
    \fn getPacket
*/      
bool        psPacket::getPacket(uint32_t maxSize, uint8_t *pid, uint32_t *packetSize,uint64_t *pts,uint64_t *dts,uint8_t *buffer)
{

}
//EOF
