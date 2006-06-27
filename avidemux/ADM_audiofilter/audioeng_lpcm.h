
#ifndef __ADM_LPCM_H
#define __ADM_LPCM_H

class AVDMProcessAudio_Lpcm : public AVDMBufferedAudioStream
{
	protected:
		virtual uint32_t      grab(uint8_t *obuffer);

	public:
		AVDMProcessAudio_Lpcm(AVDMGenericAudioStream *instream  );
		~AVDMProcessAudio_Lpcm();
		void init();
};
#endif
