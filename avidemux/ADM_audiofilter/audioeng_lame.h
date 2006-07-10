
#ifndef __ADM_LAME_H
#define __ADM_LAME_H




#ifdef HAVE_LIBMP3LAME
#ifndef LAME_NO_DECLARE
#include "ADM_audiofilter/audioencoder_lame_param.h"
//_____________________________________________
class AVDMProcessAudio_Lame : public AVDMBufferedAudioStream
{
protected:
	ADM_LAME_PRESET _preset;
          virtual uint32_t 	grab(uint8_t *obuffer);

		
public:
						
				~AVDMProcessAudio_Lame();
				uint8_t	initLame(uint32_t frequence,
						 uint32_t mode,
						 uint32_t bitrate,
						ADM_LAME_PRESET preset);
		
		virtual 	uint8_t	isVBR(void );
        AVDMProcessAudio_Lame(AVDMGenericAudioStream *instream	);		
     		
};
#endif
#endif


#endif 
