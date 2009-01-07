/***************************************************************************
                         ADM_PS
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
 


#ifndef ADM_PS_H
#define ADM_PS_H

#include "ADM_Video.h"
#include "ADM_audioStream.h"
#include "dmx_io.h"
#include "ADM_indexFile.h"
#include "dmxPSPacket.h"
#include <vector>
//std::vector <dmxToken *> ListOfTokens;

typedef struct 
{
    uint64_t  startAt;
    uint32_t  index;
    uint8_t   type; // 1=I 2=P 3=B
    uint64_t  pts;
    uint64_t  dts;
    uint32_t  len;
}dmxFrame;

/**
    \fn ADM_psAccess
*/
class ADM_psAccess : public ADM_audioAccess
{
protected:
                
             
                
public:
                                  ADM_psAccess(const char *name) {}; 
                virtual           ~ADM_psAccess() {};
                                    /// Hint, the stream is pure CBR (AC3,MP2,MP3)
                virtual bool      isCBR(void) { return true;}
                                    /// Return true if the demuxer can seek in time
                virtual bool      canSeekTime(void) {return true;};
                                    /// Return true if the demuxer can seek by offser
                virtual bool      canSeekOffset(void) {return false;};
                                    /// Return true if we can have the audio duration
                virtual bool      canGetDuration(void) {return true;};
                                    /// Returns audio duration in us
                virtual uint64_t  getDurationInUs(void) ;
                                    /// Go to a given time
                virtual bool      goToTime(uint64_t timeUs) {return false;}
                virtual bool      getPacket(uint8_t *buffer, uint32_t *size, uint32_t maxSize,uint64_t *dts) {return false;}
};


/**
    \Class psHeader
    \brief mpeg ps demuxer

*/
class psHeader         :public vidHeader
{
  protected:
    
    bool    interlaced;
    bool    readVideo(indexFile *index);
    bool    readIndex(indexFile *index);
    std::vector <dmxFrame *> ListOfFrames;      
    fileParser      parser;
    uint32_t       lastFrame;
    psPacketLinear *psPacket;
    uint64_t        timeConvert(uint64_t x);
    bool            updatePtsDts(void);
  public:


    virtual   void          Dump(void);

            psHeader( void ) ;
   virtual  ~psHeader(  ) ;
// AVI io
    virtual uint8_t  open(const char *name);
    virtual uint8_t  close(void) ;
  //__________________________
  //  Info
  //__________________________

  //__________________________
  //  Audio
  //__________________________
virtual 	WAVHeader              *getAudioInfo(uint32_t i )  ;
virtual 	uint8_t                 getAudioStream(uint32_t i,ADM_audioStream  **audio);
virtual     uint8_t                 getNbAudioStreams(void);
// Frames
  //__________________________
  //  video
  //__________________________

    virtual uint8_t  setFlag(uint32_t frame,uint32_t flags);
    virtual uint32_t getFlags(uint32_t frame,uint32_t *flags);
    virtual uint8_t  getFrame(uint32_t framenum,ADMCompressedImage *img);
    virtual uint64_t getTime(uint32_t frame);
            uint8_t  getExtraHeaderData(uint32_t *len, uint8_t **data);
    virtual uint64_t getVideoDuration(void);
    virtual uint8_t  getFrameSize(uint32_t frame,uint32_t *size) ;

};
#endif

