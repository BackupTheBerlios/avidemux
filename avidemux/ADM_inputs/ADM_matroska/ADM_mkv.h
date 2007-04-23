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
 


#ifndef ADM_MKV_H
#define ADM_MKV_H

#include "ADM_editor/ADM_Video.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_inputs/ADM_matroska/ADM_ebml.h"
typedef struct mkvTrak
{
  /* Index in mkv */
  uint32_t  streamIndex;
  
  /* Used for audio */
  WAVHeader wavHeader;
  uint32_t  nbPackets;
  uint32_t  length;
  /* Used for both */
  uint8_t    *extraData;
  uint32_t   extraDataLen;
};

#define ADM_MKV_MAX_TRACKS 20

class mkvHeader         :public vidHeader
{
  protected:
                                
    FILE                    *_fd;
    mkvTrak                 _tracks[ADM_MKV_MAX_TRACKS+1];

    
    uint32_t                _nbAudioTrack;
    
    
    uint8_t                 checkHeader(void *head,uint32_t headlen);
    uint8_t                 analyzeTracks(void *head,uint32_t headlen);
    uint8_t                 analyzeOneTrack(void *head,uint32_t headlen);
    uint8_t                 walk(void *seed);
    
  public:


    virtual   void          Dump(void);
    virtual   uint32_t      getNbStream(void) ;
    virtual   uint8_t       needDecompress(void);

            mkvHeader( void );
   virtual  ~mkvHeader(  ) ;
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

};
#endif


