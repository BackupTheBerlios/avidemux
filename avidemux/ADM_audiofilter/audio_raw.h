/***************************************************************************
    copyright            : (C) 2006 by mean
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
#ifndef AUDIO_RSHIFT_H
#define AUDIO_RSHIFT_H
/**
  This class implements a timeshift directly on the bistream bits
  If the shift is <0, it amounts to duplicate the head of the bistream
  Shift >0 means seeking forward in the stream

*/
class AVDMProcessAudio_RawShift : public AVDMBufferedAudioStream
{
  protected:
    uint32_t _starttime; /*< Starting time in ms*/
    int32_t  _hold;      /*< Nb of audioSample to duplicate*/
  public :
    virtual uint32_t read(uint32_t len,uint8_t *buffer);
    virtual uint8_t  goTo(uint32_t newoffset);
    virtual uint8_t  goToTime(uint32_t newoffset);
    virtual uint32_t readDecompress(uint32_t len,uint8_t *buffer) ;
            uint8_t  getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples);
    
    AVDMProcessAudio_RawShift(AVDMGenericAudioStream * instream, uint32_t starttime, int32_t msoff);
    virtual ~AVDMProcessAudio_RawShift();
    uint8_t isVBR(void);
    uint8_t packetPerFrame(void);

    //
            uint32_t read(uint32_t len,float *buffer) {ADM_assert(0);return 0;}
            uint32_t grab(uint8_t *outbuffer) {ADM_assert(0);return 0;}
    virtual uint8_t  extraData(uint32_t *l,uint8_t **d)
    {
      ADM_assert(_instream);
      return _instream->extraData(l,d);
    }
};
#endif
//EOF
