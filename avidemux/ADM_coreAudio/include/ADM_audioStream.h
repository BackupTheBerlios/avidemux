/***************************************************************************
                          ADM_audioStream.h  -  description
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
#ifndef ADM_audioStream.h
#define ADM_audioStream.h

#include "ADM_baseAudioStrem.h"
/**
        \fn      ADM_audioAccess
        \brief   Access layer to the file. That one is re-instancied by each demuxer 
*/
#define ADM_AUDIO_NO_DTS ((uint64_t)-1)
class ADM_audioAccess
{
protected:
public:
                virtual uint32_t  getLength(void)=0;
                virtual uint8_t   setPos(uint64_t pos)=0;
                virtual uint64_t  getPos()=0;
                virtual uint8_t   getPacket(uint8_t *buffer, uint32_t *size, uint32_t maxSize,uint64_t *dts)=0;
};
/**
        \fn ADM_audioStream
        \brief Base class for audio stream

*/
class ADM_audioStream
{
        protected:
                       WAVHeader                wavHeader;
                       ADM_audioAccess          *access; 
                       uint32_t                 lengthInBytes;
                       uint64_t                 position;
                       ADM_Audiocodec 	        *audioCodec;
                       uint32_t                 extraDataSize;
                       uint8_t                  *extraData;

        public:
                       ADM_audioStream(WAVHeader *header,ADM_audioAccess *access,uint8_t *extraData,uint32_t extraDataSize);  
                       WAVHeader                *getInfo(void) {return &wavHeader;}
///  Get  Channel mapping
virtual CHANNEL_TYPE   *getChannelMapping(void) {if(_codec) return _codec->channelMapping; else return NULL;}
///  Get a packet
virtual uint8_t         getPacket(uint8_t *buffer,uint32_t *size, uint32_t sizeMax,uint32_t *nbSample);
/// Go to a given time, in microseconds
virtual uint8_t         goToTime(uint64_t nbUs);
/// Returns current time in us. Not used.
//virtual uint8_t         getTime(uint64_t *nbUs);
/// Returns extra configuration data
        uint8_t         getExtraData(uint32_t *l, uint8_t **d);
};
/**
                Create the appropriate audio stream
*/
ADM_audioStream  *ADM_audioCreateStream(WAVHeader *wavheader, ADM_audioAccess *access);
#endif
// EOF

