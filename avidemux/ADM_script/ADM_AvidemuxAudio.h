#ifndef _ADM_AVIDEMUXAUDIO_H
#define _ADM_AVIDEMUXAUDIO_H


class ADM_AvidemuxAudio
{
public:
	ADM_AvidemuxAudio(void) : m_bNormalize(false), m_bDownsample(false), m_nResample(0), m_nDelay(0), m_bFilm2PAL(false), m_bPAL2Film(false), m_bMono2Stereo(false), m_bStereo2Mono(false) {}
	virtual ~ADM_AvidemuxAudio(void);

	// member variables
        bool m_bAudioProcess;
	bool m_bNormalize;
	bool m_bDownsample;
	unsigned long m_nResample;
	int m_nDelay;
	bool m_bFilm2PAL;
	bool m_bPAL2Film;
	bool m_bMono2Stereo;
	bool m_bStereo2Mono;
};

#endif
