#ifndef _ADM_encoder_Config_
#define _ADM_encoder_Config_

void saveEncoderConfig (void);
void loadEncoderConfig (void);

const char *videoCodecGetName (void);
int videoCodecSelectByName (const char *name);


void videoCodecSetConf (char *name, uint32_t extraLen, uint8_t * extraData);
uint8_t videoCodecGetConf (uint32_t * nbData, uint8_t ** data);

// Returns the mode (CQ/CBR...) as a string, suitable for a script
const char *videoCodecGetMode (void);
uint8_t videoCodecSetFinalSize (uint32_t size);

//
void setPSP_X264Preset(void);
#endif
