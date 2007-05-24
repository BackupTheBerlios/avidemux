#ifndef AUDM_VORBIS_PARAM_H
#define AUDM_VORBIS_PARAM_H

typedef enum 
{
  ADM_VORBIS_CBR,
  ADM_VORBIS_VBR,
  ADM_VORBIS_QUALITY
  
}ADM_VORBIS_MODE;

typedef struct VORBIS_encoderParam
{
  ADM_VORBIS_MODE    mode;   // 0 cbr 1 vbr 2 quality
  float              quality;
};
#endif
