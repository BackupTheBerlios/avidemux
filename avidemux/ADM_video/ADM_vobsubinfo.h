//
// C++ Interface: Vobsubinfo struct & prototyping
//
// Description: Read the idx part of an index file
//
//
// Author: mean , fixounet@free.fr, GPL (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef _VOBSUB_INFO_
#define _VOBSUB_INFO_

typedef struct vobSubLine
{
        uint32_t        startTime;      /// Starting time in ms
        uint32_t        stopTime;       /// Stop time
        uint64_t        fileOffset;     /// Offset in the file where this sub starts

}vobSubLine;

typedef struct VobSubInfo
{
        uint32_t        Palette[16];
        uint32_t        nbLines;
        vobSubLine      *lines;
}VobSubInfo;

/// Read the sub and return a new filled out vobsub info struct
uint8_t vobSubRead(char *filename,VobSubInfo **info);
uint8_t destroySubInfo(VobSubInfo *sub);



#endif
// EOF
