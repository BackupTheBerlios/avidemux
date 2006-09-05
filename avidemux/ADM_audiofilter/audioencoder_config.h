#ifndef AUDIOENCODER_CONFIG_H
#define AUDIOENCODER_CONFIG_H

#include "audioencoder_faac_param.h"
#include "audioencoder_lame_param.h"
#include "audioencoder_twolame_param.h"
#include "audioencoder_vorbis_param.h"
#include "audioencoder_aften_param.h"

extern int DIA_getVorbisSettings(ADM_audioEncoderDescriptor *descriptor);
extern int DIA_getLameSettings(ADM_audioEncoderDescriptor *descriptor);
extern int DIA_defaultSettings(ADM_audioEncoderDescriptor *descriptor);
/**** Copy ****/

ADM_audioEncoderDescriptor copyDescriptor=
{
  AUDIOENC_COPY,
  DIA_defaultSettings,
  "Copy encoder",
  128,
  99,
  0,
  NULL
};
/**** FAAC ****/
FAAC_encoderParam aacParam={128};
ADM_audioEncoderDescriptor aacDescriptor=
{
        AUDIOENC_FAAC,
        DIA_defaultSettings,
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
  ADM_STEREO,
  2
};
ADM_audioEncoderDescriptor  lameDescriptor=
{
  AUDIOENC_MP3,
  DIA_getLameSettings,
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
  AUDIOENC_2LAME,
  DIA_defaultSettings,
  "TwoLame encoder",
  128,
  2,      // Lame can only do stereo
  sizeof(twolameParam),
  &twolameParam
};
/********** Lavcodec **************/
ADM_audioEncoderDescriptor  lavcodecMP2Descriptor=
{
  AUDIOENC_MP2,
  DIA_defaultSettings,
  "LAvcodec MP2 encoder",
  128,
  2,    
  0,
  NULL
};
ADM_audioEncoderDescriptor  lavcodecAC3Descriptor=
{
  AUDIOENC_AC3,
  DIA_defaultSettings,
  "LAvcodec AC3 encoder",
  128,
  6,    
  0,
  NULL
};

/************** Vorbis **************/
VORBIS_encoderParam vorbisParam=
{
  ADM_VORBIS_VBR,
  3
  
};
ADM_audioEncoderDescriptor  vorbisDescriptor=
{
  AUDIOENC_VORBIS,
  DIA_getVorbisSettings,
  "Vorbis encoder",
  128,
  6,      // Lame can only do stereo
  sizeof(vorbisParam),
  &vorbisParam
};
/********** PCM **************/
ADM_audioEncoderDescriptor  pcmDescriptor=
{
  AUDIOENC_NONE,
  NULL,
  "PCM encoder",
  128,
  6,    
  0,
  NULL
};
ADM_audioEncoderDescriptor  lpcmDescriptor=
{
  AUDIOENC_LPCM,
  NULL,
  "LPCM encoder",
  128,
  6,    
  0,
  NULL
};
//---------------- AFTEN ------------------
#ifdef USE_AFTEN
AFTEN_encoderParam aftenParam =
{
  ADM_STEREO
};

ADM_audioEncoderDescriptor  aftenDescriptor=
{
  AUDIOENC_AFTEN,
  DIA_defaultSettings,
  "Aften AC3 encoder",
  128,
  6,      // Lame can only do stereo
  sizeof(aftenParam),
  &aftenParam
};
#endif
ADM_audioEncoderDescriptor *allDescriptors[]=
{
      &copyDescriptor,
      &aacDescriptor,
      &twolameDescriptor,
      &lavcodecAC3Descriptor,
      &lavcodecMP2Descriptor,
      &vorbisDescriptor ,
      &pcmDescriptor,
      &lpcmDescriptor,
#ifdef USE_AFTEN
      &aftenDescriptor,
#endif      
      &lameDescriptor
};
#define NB_AUDIO_DESCRIPTOR (sizeof(allDescriptors)/sizeof(ADM_audioEncoderDescriptor *))
#endif
//EOF

