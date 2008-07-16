/**
    \file audioencoderInternal.h
    \brief interface to audio encoder plugins

*/
#ifndef AUDIOENCODERINTERNAL_H
#define AUDIOENCODERINTERNAL_H

#define ADM_AUDIO_ENCODER_API_VERSION 1
#include "audioencoder.h"
class AUDMEncoder;
class AUDMAudioFilter;


typedef struct
{
    uint32_t     apiVersion;            // const
    AUDMEncoder *(*create)(AUDMAudioFilter *head);  
    void         (*destroy)(AUDMEncoder *codec);
    uint8_t      (*configure)(void);    
    const char   *codecName;        // Internal name (tag)
    const char   *menuName;         // Displayed name (in menu)
    const char   *description;
    uint32_t     maxChannels;       // Const
    uint32_t     major,minor,patch;     // Const
    uint32_t     wavTag;                // const Avi fourcc
    uint32_t     priority;              // const Higher means the codec is prefered and should appear first in the list
    uint8_t      (*getConfigurationData)(uint32_t *l, uint8_t **d); // Get the encoder private conf
    uint8_t      (*setConfigurationData)(uint32_t l, uint8_t *d); // Get the encoder private conf

    uint32_t     (*getBitrate)(void);
    void         (*setBitrate)(uint32_t br);
 
    uint8_t      (*setOption)(const char *paramName, uint32_t value);

    void         *opaque;               // Hide stuff in here
}ADM_audioEncoder;

// Macros to declare audio encoder

#endif
//EOF
