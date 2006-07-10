#ifndef AUDIO_F_LIMITER_H
#define AUDIO_F_LIMITER_H

#include "audioeng_process.h"
#include "audiofilter_limiter_param.h"
class AUDMAudioFilterLimiter : public AUDMAudioFilter
{
  protected:
    uint8_t            filled;
    DRCparam           _param;			
    float              mCircle[DRC_WINDOW];
    float              mLevelCircle[DRC_WINDOW];	
    int                mCircleSize;
    int                mCirclePos;	
    float              mRMSSum;
    float              mThreshold;
    float              mGain;
    float              mAttackFactor;
    float              mDecayFactor;	
    float              mLastLevel;
    float              mGainDB;
    float              AvgCircle(float value);
    void               Follow(float x, float *outEnv, int maxBack);
    float              DoCompression(float value, float env);
    void               drc_cleanup(void);
#define ONE_CHUNK 1000			 
    float              follow[ONE_CHUNK/2];
    float              value[ONE_CHUNK/2];

  public:
                          AUDMAudioFilterLimiter(AUDMAudioFilter *previous, DRCparam *param);
    virtual                ~AUDMAudioFilterLimiter();
    virtual    uint32_t   fill(uint32_t max,float *output,AUD_Status *status);
};
#endif
