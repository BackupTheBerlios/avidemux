#ifndef _ADM_encoder_Config_
#define _ADM_encoder_Config_

void saveEncoderConfig( void );
void loadEncoderConfig( void );

const char 	*videoCodecGetName( void );
void 		videoCodecSelectByName( const char *name);


void 		videoCodecSetConf(  char *name,uint32_t extraLen, uint8_t *extraData);
const char  	*videoCodecGetConf( uint32_t *nbData , uint8_t **data);


#endif
