#include "ADM_Avidemux.h"

extern JSContext *g_pCx;
extern JSObject *g_pObject;

ADM_Avidemux::ADM_Avidemux(void) :  m_pAudio(NULL), m_pVideo(NULL), m_pContainer(NULL), m_nCurrentFrame(0), m_dFPS(0)
{// begin ADM_Avidemux

	// initialize audio property
	JSObject *pTempObject = ADM_JSAvidemuxAudio::JSInit(g_pCx,g_pObject);
	ADM_JSAvidemuxAudio *pAudio = new ADM_JSAvidemuxAudio();
	pAudio->setObject(new ADM_AvidemuxAudio());
	JS_SetPrivate(g_pCx,pTempObject,pAudio);
	m_pAudio = pTempObject;

	// initialize video property
	pTempObject = ADM_JSAvidemuxVideo::JSInit(g_pCx,g_pObject);
	ADM_JSAvidemuxVideo *pVideo = new ADM_JSAvidemuxVideo();
	pVideo->setObject(new ADM_AvidemuxVideo());
	JS_SetPrivate(g_pCx,pTempObject,pVideo);
	m_pVideo = pTempObject;
}// end ADM_Avidemux

ADM_Avidemux::~ADM_Avidemux()
{// begin ~ADM_Avidemux

}// end ~ADM_Avidemux
