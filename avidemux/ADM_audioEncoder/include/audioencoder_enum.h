/**
    \fn audioencoder_enum
    \brief Temporary, will be removed
*/

#ifndef audioencoder_enum_h
#define audioencoder_enum_h

typedef enum 
{
	AUDIOENC_NONE,
	AUDIOENC_MP3,
	AUDIOENC_MP2,
	AUDIOENC_AC3,
	AUDIOENC_2LAME,
	AUDIOENC_FAAC,
	AUDIOENC_VORBIS,
    AUDIOENC_COPY,
    AUDIOENC_LPCM,
    AUDIOENC_AFTEN,
    AUDIOENC_DUMMY
}AUDIOENCODER;

#endif