
#ifndef __ADM_LAME_H
#define __ADM_LAME_H

typedef enum ADM_LAME_PRESET
{
	ADM_LAME_PRESET_CBR,
	ADM_LAME_PRESET_ABR,
	ADM_LAME_PRESET_EXTREME
};

typedef struct ADM_PRESET_DEFINITION
{
	ADM_LAME_PRESET preset;
	const char	*name;
}ADM_PRESET_DEFINITION;


#ifdef HAVE_LIBMP3LAME
#ifndef LAME_NO_DECLARE
//_____________________________________________
class AVDMProcessAudio_Lame : public AVDMBufferedAudioStream
{
protected:
         uint32_t _mode;
         uint32_t _bitrate;
         uint32_t _fq;
         uint32_t _size;
				 uint8_t  _bufferin[PROCESS_BUFFER_SIZE];
          virtual uint32_t 	grab(uint8_t *obuffer);
		
public:
						
				~AVDMProcessAudio_Lame();
				uint8_t	initLame(uint32_t frequence,
						 uint32_t mode,
						 uint32_t bitrate,
						ADM_LAME_PRESET preset);

        AVDMProcessAudio_Lame(AVDMGenericAudioStream *instream	);		
     		
};
#endif
#endif


#endif 
