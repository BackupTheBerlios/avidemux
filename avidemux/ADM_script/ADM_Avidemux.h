#ifndef _ADM_AVIDEMUX_H
#define _ADM_AVIDEMUX_H

#include "ADM_JSAvidemuxAudio.h"
#include "ADM_JSAvidemuxVideo.h"

class ADM_Avidemux
{
public:
	ADM_Avidemux(void);
	virtual ~ADM_Avidemux(void);

	// member variables
	JSObject *m_pAudio;
	JSObject *m_pVideo;
	JSString *m_pContainer;
	int m_nCurrentFrame;
	double m_dFPS;

};

#endif
