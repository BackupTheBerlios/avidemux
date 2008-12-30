/***************************************************************************
    copyright            : (C) 2007/2009 by mean
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
#include "fourcc.h"
#include "DIA_coreToolkit.h"

#include "ADM_ps.h"

#include <math.h>

uint32_t ADM_UsecFromFps1000(uint32_t fps1000);

/**
      \fn open
      \brief open the flv file, gather infos and build index(es).
*/

uint8_t psHeader::open(const char *name)
{
  
  
  return false;
}
/**
        \fn getVideoDuration
        \brief Returns duration of video in us
*/
uint64_t psHeader::getVideoDuration(void)
{
     return 0;
}


/**
    \fn getAudioInfo
    \brief returns wav header for stream i (=0)
*/
WAVHeader *psHeader::getAudioInfo(uint32_t i )
{
 
      return NULL;
}
/**
   \fn getAudioStream
*/

uint8_t   psHeader::getAudioStream(uint32_t i,ADM_audioStream  **audio)
{
  
  *audio=NULL;
  return 0; 
}
/**
    \fn getNbAudioStreams

*/
uint8_t   psHeader::getNbAudioStreams(void)
{
 
  return 0; 
}
/*
    __________________________________________________________
*/

void psHeader::Dump(void)
{
 
}
/**
    \fn close
    \brief cleanup
*/

uint8_t psHeader::close(void)
{
  
}
/**
    \fn psHeader
    \brief constructor
*/

 psHeader::psHeader( void ) : vidHeader()
{ 
    
    
    
}
/**
    \fn psHeader
    \brief destructor
*/

 psHeader::~psHeader(  )
{
  close();
}


/**
    \fn setFlag
    \brief Returns timestamp in us of frame "frame" (PTS)
*/

  uint8_t  psHeader::setFlag(uint32_t frame,uint32_t flags)
{
   
    return 1;
}
/**
    \fn getFlags
    \brief Returns timestamp in us of frame "frame" (PTS)
*/

uint32_t psHeader::getFlags(uint32_t frame,uint32_t *flags)
{
    
    return  1;
}

/**
    \fn getTime
    \brief Returns timestamp in us of frame "frame" (PTS)
*/
uint64_t psHeader::getTime(uint32_t frame)
{
    return 0;
}
/**
        \fn getFrame
*/

uint8_t  psHeader::getFrame(uint32_t frame,ADMCompressedImage *img)
{
     return false;
}
/**
        \fn getExtraHeaderData
*/
uint8_t  psHeader::getExtraHeaderData(uint32_t *len, uint8_t **data)
{
                *len=0; //_tracks[0].extraDataLen;
                *data=NULL; //_tracks[0].extraData;
                return 1;            
}

/**
      \fn getFrameSize
      \brief return the size of frame frame
*/
uint8_t psHeader::getFrameSize (uint32_t frame, uint32_t * size)
{
    *size=0;
    return true;
}


//EOF
