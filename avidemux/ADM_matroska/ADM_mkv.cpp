/***************************************************************************
    copyright            : (C) 2006 by mean
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

#include "math.h"

#include "ADM_library/default.h"
#include "ADM_editor/ADM_Video.h"
#include <ADM_assert.h>

#include "ADM_library/fourcc.h"
#include "ADM_toolkit/toolkit.hxx"

#include "ADM_mkv.h"

WAVHeader *mkvHeader::getAudioInfo(void )
{
  return NULL;
}
uint8_t mkvHeader::getAudioStream(AVDMGenericAudioStream **audio)
{
  *audio=NULL;
  return 0; 
}
void mkvHeader::Dump(void)
{
  
}
uint32_t mkvHeader::getNbStream(void) 
{
  return 0; 
}
uint8_t mkvHeader::needDecompress(void)
{
  return 1; 
}
 mkvHeader::mkvHeader( void ) : vidHeader()
{
  _fd=NULL;
}
 mkvHeader::~mkvHeader(  )
{
  
}

uint8_t mkvHeader::open(char *name)
{
  printf("[Matroska] Open\n");
  return 0;
}

uint8_t mkvHeader:: close(void) 
{
  
}

  uint8_t  mkvHeader::setFlag(uint32_t frame,uint32_t flags)
{
  return 0; 
}
uint32_t mkvHeader::getFlags(uint32_t frame,uint32_t *flags)
{
  return 0; 
}

uint8_t  mkvHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                                     uint32_t *flags)
{
  return 0; 
}

uint8_t  mkvHeader::getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)
{
  return 0; 
}


//EOF
