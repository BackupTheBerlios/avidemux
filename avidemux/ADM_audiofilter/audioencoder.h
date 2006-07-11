/***************************************************************************
    copyright            : (C) 2006 by mean
    email                : fixounet@free.fr
 ***************************************************************************/


#ifndef AUDIO_ENCODER_H
#define AUDIO_ENCODER_H
/*!
  This structure defines an audio encoder
  \param encoder Encoder attached to this descriptor
   \param name The name of the codec
  \param bitrate The bitrate in kb/s
  \param configure Function to call to configure the codec
  \param maxChannels The maximum # of channels this codec supports
  \param param : An opaque structure that contains the codec specific configuration datas
*/
#include "audioeng_buildfilters.h"
typedef struct ADM_audioEncoderDescriptor
{
  AUDIOENCODER encoder;
  int       (*configure)(ADM_audioEncoderDescriptor *descritor);
  const     char *name;
  uint32_t  bitrate;
  uint32_t  maxChannels;
  uint32_t  paramSize;
  void     *param;
};

/*!
  Base class for all audio encoder.It does the reverse of the bridge class and offers a proper GenericAudioStreamAPI

*/
 //_____________________________________________
class AUDMEncoder : public AVDMBufferedAudioStream
{
  protected:
    //
    uint32_t grab(uint8_t *outbuffer);
    uint32_t grab(float *outbuffer) {ADM_assert(0);return 1;}
    uint32_t  eof_met;
    //
    uint8_t         *_extraData;
    uint32_t        _extraSize;
    AUDMAudioFilter *_incoming;
    uint8_t         cleanup(void);
    
    float          *tmpbuffer;
    uint8_t        refillBuffer(int minimum); // Mininum is in float
    uint8_t        dither16(float *start, uint32_t nb); //
    uint32_t       tmphead,tmptail;
  public:
    //
    uint32_t read(uint32_t len,uint8_t *buffer);
    uint32_t read(uint32_t len,float *buffer) {ADM_assert(0);return 1;}
    //
    virtual ~AUDMEncoder();
    AUDMEncoder(AUDMAudioFilter *in);	
    virtual uint8_t init(ADM_audioEncoderDescriptor *config)=0;
    virtual uint8_t getPacket(uint8_t *dest, uint32_t *len, uint32_t *samples)=0;
    virtual uint8_t packetPerFrame( void) {return 1;}
    virtual uint8_t extraData(uint32_t *l,uint8_t **d) {*l=_extraSize;*d=_extraData;return 1;}
};

#endif
