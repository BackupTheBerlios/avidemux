/***************************************************************************
                         ADM_FLV
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
 


#ifndef ADM_FLV_H
#define ADM_FLV_H

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"


typedef struct 
{
    uint64_t pos;
    uint32_t size;
    uint32_t flags;
    uint32_t timeCode;  // Time code in ms from start
}flvIndex;
//**********************************************
typedef struct 
{
  /* Index in mkv */
  uint32_t  streamIndex;
  uint32_t  length;
  uint8_t    *extraData;
  uint32_t   extraDataLen;
  flvIndex  *_index;
  uint32_t  _nbIndex;  // current size of the index
  uint32_t  _indexMax; // Max size of the index
  uint32_t  _sizeInBytes; // Approximate size in bytes of that stream
  uint32_t  _defaultFrameDuration; // in us!
}flvTrak;


class flvHeader         :public vidHeader
{
  protected:
                                
    FILE                    *_fd;
    char                    *_filename;
    flvTrak                 *videoTrack;
    flvTrak                 *audioTrack;
    
    uint8_t                 changeAudioStream(uint32_t newstream);
    uint32_t                getCurrentAudioStreamNumber(void);
    uint8_t                 getAudioStreamsInfo(uint32_t *nbStreams, audioInfo **infos);
    
    /* */
    
    uint8_t     read(uint32_t len, uint8_t *where);
    uint8_t     read8(void);
    uint32_t    read16(void);
    uint32_t    read24(void);
    uint32_t    read32(void);
    uint8_t     Skip(uint32_t len);
    uint8_t     insertVideo(uint32_t pos,uint32_t size,uint32_t frameType,uint32_t pts);
    uint8_t     getFrameSize (uint32_t frame, uint32_t * size);
  public:


    virtual   void          Dump(void);
    virtual   uint32_t      getNbStream(void) ;
    virtual   uint8_t       needDecompress(void);

            flvHeader( void );
   virtual  ~flvHeader(  ) ;
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
    virtual uint8_t  getFrameNoAlloc(uint32_t framenum,ADMCompressedImage *img);

            uint8_t  getExtraHeaderData(uint32_t *len, uint8_t **data);

};
#endif


