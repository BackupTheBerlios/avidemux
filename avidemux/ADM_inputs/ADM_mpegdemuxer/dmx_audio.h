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
#include "dmx_demuxer.h"
#include "dmx_demuxerEs.h"
#include "dmx_demuxerPS.h"
#include "dmx_demuxerTS.h"
#include "dmx_demuxerMSDVR.h"
#include "ADM_audio/aviaudio.hxx"

#define DMX_MAX_TRACK 16

 typedef struct 
 {
                uint32_t img;                      // Corresponding image
                uint64_t start;                    // Start of packet
                uint64_t count[DMX_MAX_TRACK];         // Size of audio seen
}dmxAudioIndex;
class dmxAudioTrack
{
public:
                      dmxAudioTrack(void) {};
                      ~dmxAudioTrack() {};
      uint32_t        myPes,myPid;
      WAVHeader       wavHeader;
      int32_t         avSync;
};
class dmxAudioStream : public AVDMGenericAudioStream
{
        protected:
                uint8_t         probeAudio (void);
       protected:
                dmx_demuxer            *demuxer;
                dmxAudioIndex           *_index;
                uint32_t                nbIndex;
                uint32_t                nbTrack;
                dmxAudioTrack           *_tracks;
                
                
                                
                public:
                uint32_t        currentTrack;
                                dmxAudioStream( void);
                uint8_t         open(const char *name);
        virtual                 ~dmxAudioStream() ;                    
        virtual uint8_t         goTo(uint32_t offset);
        virtual uint32_t        read(uint32_t size,uint8_t *ptr);
                  uint8_t        getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **infos);
                  uint8_t        changeAudioTrack(uint32_t newtrack);
}
;
#endif
