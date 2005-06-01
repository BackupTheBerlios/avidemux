/***************************************************************************
                          dmx_audio.h  
                        Demuxer for audio stream

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
 #ifndef MPX_AUDIO
 #define MPX_AUDIO
#include "ADM_mpegdemuxer/dmx_demuxer.h"
#include "ADM_mpegdemuxer/dmx_demuxerEs.h"
#include "ADM_mpegdemuxer/dmx_demuxerPS.h"
 #include "ADM_audio/aviaudio.hxx"
 typedef struct dmxAudioIndex
 {
                uint32_t img;
                uint64_t start;       
                uint64_t count;               
};
 
class dmxAudioStream : public AVDMGenericAudioStream
{
        protected:
                uint8_t         probeAudio (void);
       protected:
                dmx_demuxer            *demuxer;       
                uint32_t                nbIndex;
                dmxAudioIndex           *index;
                uint16_t                myPes;
                                
                public:
                                dmxAudioStream( void);
                uint8_t         open(char *name);
        virtual                 ~dmxAudioStream() ;                    
        virtual uint8_t         goTo(uint32_t offset);
        virtual uint32_t        read(uint32_t size,uint8_t *ptr);

}
;
#endif
