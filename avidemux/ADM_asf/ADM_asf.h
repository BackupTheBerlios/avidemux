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
    void                    *_context;
    uint32_t                _trackIndex;
    WAVHeader               _wavHeader;
    uint32_t                _extraDataLen;
    uint8_t                 *_extraData;
    char                    *myName;
  public:
                                asfAudio(char *name,asfAudioTrak *track);
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
                                
    FILE                    *_fd;
    int32_t                 _audioIndex;
    int32_t                 _videoIndex;
    uint8_t                 readVideoInfo( void);
    uint8_t                 readAudioInfo( uint32_t track);
    uint8_t                 prebuildIndex(void);
    uint8_t                 rewind(void);
    void                    *_context;
    uint32_t                _nbAudioTrack;
    asfAudioTrak            *_audioTracks;
    asfAudio                *_curAudio;
    char                    *myName;
  public:


    virtual   void          Dump(void);
    virtual   uint32_t      getNbStream(void) ;
    virtual   uint8_t       needDecompress(void);

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


