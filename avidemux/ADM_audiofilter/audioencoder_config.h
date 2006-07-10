#ifndef AUDIOENCODER_CONFIG_H
#define AUDIOENCODER_CONFIG_H

#include "audioencoder_faac_param.h"
#include "audioencoder_lame_param.h"
#include "audioencoder_twolame_param.h"
/**** FAAC ****/
FAAC_encoderParam aacParam={128};
ADM_audioEncoderDescriptor aacDescriptor=
{
        "FAAC encoder",
        128,
        6,      // AAC can do 5.1
        sizeof(aacParam),
        &aacParam
};
/**** LAME ****/
LAME_encoderParam lameParam=
{
  ADM_LAME_PRESET_CBR,
  ADM_STEREO
};
ADM_audioEncoderDescriptor  lameDescriptor=
{
  "Lame encoder",
  128,
  2,      // Lame can only do stereo
  sizeof(lameParam),
  &lameParam
};
/**** TWOLAME ****/
TWOLAME_encoderParam twolameParam=
{
  ADM_STEREO
};
ADM_audioEncoderDescriptor  twolameDescriptor=
{
  "TwoLame encoder",
  128,
  2,      // Lame can only do stereo
  sizeof(twolameParam),
  &twolameParam
};
/********** Lavcodec **************/
ADM_audioEncoderDescriptor  lavcodecDescriptor=
{
  "LAvcodec encoder",
  128,
  6,    
  0,
  NULL
};


#endif
//EOF

