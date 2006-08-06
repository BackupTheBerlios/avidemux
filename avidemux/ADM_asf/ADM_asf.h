/***************************************************************************
                          ADM_pics.h  -  description
                             -------------------
    begin                : Mon Jun 3 2002
    copyright            : (C) 2002 by mean
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
 


#ifndef ADM_ASF_H
#define ADM_ASF_H

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"

#include "ADM_toolkit/ADM_queue.h"
#include "ADM_asfPacket.h"

typedef struct asfIndex
{
  uint32_t packetNb;
  uint32_t frameLen;
  uint32_t segNb;
  uint32_t flags;
};

typedef enum ADM_KNOWN_CHUNK
{
  ADM_CHUNK_HEADER_CHUNK ,
  ADM_CHUNK_FILE_HEADER_CHUNK,
  ADM_CHUNK_NO_AUDIO_CONCEAL,
  ADM_CHUNK_STREAM_HEADER_CHUNK,
  ADM_CHUNK_STREAM_GROUP_ID,
  ADM_CHUNK_DATA_CHUNK,
  ADM_CHUNK_UNKNOWN_CHUNK
};
typedef struct chunky
{
  const char *name;
  uint8_t val[16];
  ADM_KNOWN_CHUNK id; 
};
class asfChunk
{
  protected:
    FILE        *_fd;
    
    
  public:
  uint32_t  _chunkStart;
            asfChunk(FILE *f);
            ~asfChunk();
  uint8_t   dump(void);
  uint8_t   guId[16];
  uint64_t  chunkLen;
  
  uint8_t   readChunkPayload(uint8_t *data, uint32_t *dataLen);
  uint8_t   nextChunk(void);
  uint8_t   skipChunk(void);
  uint64_t  read64(void);
  uint32_t  read32(void);
  uint32_t  read16(void);
  uint8_t   read8(void);
  uint8_t   read(uint8_t *where, uint32_t how);
  const chunky    *chunkId(void);
};

typedef struct asfAudioTrak
{
  uint32_t  streamIndex;
  WAVHeader wavHeader;
  uint32_t  extraDataLen;
  uint8_t   *extraData;
  uint32_t  nbPackets;
  uint32_t  length;
  
};

class asfAudio : public AVDMGenericAudioStream
{
  protected:
    uint32_t                _extraDataLen;
    uint8_t                 *_extraData;
    char                    *myName;
    uint32_t                _streamId;
    uint32_t                _dataStart;
    asfPacket               *_packet;
    FILE                    *_fd;
    ADM_queue               readQueue;
    uint32_t                _packetSize;
    class asfHeader         *_father;
  public:
                                asfAudio(asfHeader *father);
    virtual                     ~asfAudio();
    virtual uint32_t            read(uint32_t len,uint8_t *buffer);
    virtual uint8_t             goTo(uint32_t newoffset);
    virtual uint8_t             getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
    virtual uint8_t             goToTime(uint32_t mstime);
    virtual uint8_t             extraData(uint32_t *l,uint8_t **d);
};



class asfHeader         :public vidHeader
{
  protected:
    uint8_t                 getHeaders( void);
    uint8_t                 buildIndex(void);
    uint8_t                 loadVideo(asfChunk *s);
    
    ADM_queue               readQueue;
    uint32_t                curSeq;
    asfPacket               *_packet;
    
    asfAudio                *_audioTrack;
  protected:
                                
    FILE                    *_fd;
    int32_t                 _audioIndex;
    int32_t                 _videoIndex;
    uint32_t                _nbAudioTrack;
    asfAudioTrak            *_audioTracks;
    asfAudio                *_curAudio;
    
    
    uint32_t                _extraDataLen;
    uint8_t                 *_extraData;
    
    
    uint32_t                _videoStreamId;
    
  public: // Shared with audio track
    uint32_t                _audioStreamId;
    char                    *myName;
    WAVHeader               *_wavHeader;
    uint32_t                nbImage;
    asfIndex                *_index;
    uint32_t                _packetSize;
    uint32_t                _dataStartOffset;
    uint32_t                _audioExtraDataLen;
    uint8_t                 *_audioExtraData;
    
  public:


    virtual   void          Dump(void);
    virtual   uint32_t      getNbStream(void) ;
    virtual   uint8_t       needDecompress(void);
    virtual   uint8_t       getExtraHeaderData(uint32_t *len, uint8_t **data);
             asfHeader( void );
    virtual  ~asfHeader(  ) ;
// AVI io
    virtual uint8_t  open(char *name);
    virtual uint8_t  close(void) ;
  //__________________________
  //  Info
  //__________________________

  //__________________________
  //  Audio
  //__________________________

    virtual   WAVHeader *getAudioInfo(void ) ;
    virtual uint8_t getAudioStream(AVDMGenericAudioStream **audio);


// Frames
  //__________________________
  //  video
  //__________________________

    virtual uint8_t  setFlag(uint32_t frame,uint32_t flags);
    virtual uint32_t getFlags(uint32_t frame,uint32_t *flags);
    virtual uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen,
                                        uint32_t *flags);
    virtual uint8_t  getFrameNoAlloc(uint32_t framenum,uint8_t *ptr,uint32_t* framelen)	;

};
#endif


