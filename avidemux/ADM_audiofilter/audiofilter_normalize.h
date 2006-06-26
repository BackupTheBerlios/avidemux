#ifndef AUDIO_F_NORMALIZE_H
#define AUDIO_F_NORMALIZE_H

#include "audioeng_process.h"
class AUDMAudioFilterNormalize : public AUDMAudioFilter
{
  protected:
              float       _ratio;
              uint32_t    _scanned;
              uint8_t     preprocess(void);
  public:
                          AUDMAudioFilterNormalize(AUDMAudioFilter *previous);
    virtual                ~AUDMAudioFilterNormalize();
    virtual    uint32_t   fill(uint32_t max,float *output,AUD_Status *status);
};
#endif
