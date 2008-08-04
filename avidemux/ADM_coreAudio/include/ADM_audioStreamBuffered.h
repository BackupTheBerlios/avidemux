/***************************************************************************
                          ADM_audioStreamBuffered.h  -  description
                             -------------------
    copyright            : (C) 2008 by mean
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
#ifndef ADM_audioStreamBuffered_H
#define ADM_audioStreamBuffered_H

#include "ADM_audioStream.h"

/**
        \fn ADM_audioStreamBuffered
        \brief Base class for byte oriented audio streams

*/
#define ADM_AUDIOSTREAM_BUFFER_SIZE (64*1024)
class ADM_audioStreamBuffered : public ADM_audioStream
{
        protected:
                       uint8_t  buffer[2*ADM_AUDIOSTREAM_BUFFER_SIZE];
                       uint32_t limit;
                       uint32_t start;
            
        public:

                                ADM_audioStreamBuffered(WAVHeader *header,ADM_audioAccess *access);  
                                /// Put datas...
                       bool      refill(void);
                       uint32_t  read8();
                       uint32_t  read16();
                       uint32_t  read32();
                       bool      read(uint32_t n,uint8_t *d);
                       bool      peek(uint32_t n,uint8_t *d);
virtual                bool      goToTime(uint64_t nbUs);
                       bool      needBytes(uint32_t nbBytes);
                       bool      skipBytes(uint32_t nbBytes);
};
#endif
// EOF
