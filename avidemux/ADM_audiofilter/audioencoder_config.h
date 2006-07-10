#ifndef AUDIOENCODER_CONFIG_H
#define AUDIOENCODER_CONFIG_H

#include "audioencoder_faac_param.h"
FAAC_encoderParam aacParam={128};
ADM_audioEncoderDescriptor aacDescriptor=
{
        "FAAC encoder",
        128,
        sizeof(aacParam),
        &aacParam
};



#endif
//EOF

