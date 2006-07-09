/***************************************************************************
    copyright            : (C) 2006 by mean
    email                : fixounet@free.fr
 ***************************************************************************/


#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H

 //_____________________________________________
class AUDMEncoder : public AVDMBufferedAudioStream
{
  protected:
    uint32_t        grab(uint8_t *obuffer);
    void            *_handle;
    uint8_t         *_extraData;
    uint32_t        _extraSize;
    AUDMAudioFilter *_incoming;
    uint8_t         cleanup(void);
  public:
    
    virtual ~AUDMEncoder();
    AUDMEncoder(AUDMAudioFilter *in);	
    virtual uint8_t getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)=0;
    virtual uint8_t packetPerFrame( void) {return 1;}
    virtual uint8_t extraData(uint32_t *l,uint8_t **d) {*l=0;*d=NULL;return 1;}
};

#endif
