//
/**/
/***************************************************************************
                          DIA_hue
                             -------------------

                           Ui for hue & sat

    begin                : 08 Apr 2005
    copyright            : (C) 2004/5 by mean
    email                : fixounet@free.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/



#include <config.h>


#include <string.h>
#include <stdio.h>
#include <math.h>

#include "default.h"
#include "ADM_osSupport/ADM_misc.h"
#include "DIA_factory.h"
#include "ADM_assert.h"
#include "ADM_libraries/ADM_libwrapper/libwrapper_global.h"

/**
    \fn DIA_builtin(void)
    \brief Display component that are built in. They are detected at configure time.

*/

uint8_t DIA_builtin(void)
{
  uint32_t altivec=0,mad=0,a52dec=0,xvid4=0,X264=0,freetype=0,esd=0,arts=0,vorbis=0,win32=0;
  uint32_t faac=0,faad=0,libdca=0,aften=0,libamrnb=0,lame=0,sdl=0,oss=0,xvideo=0,x86=0,x86_64=0,alsa=0;
  uint32_t powerpc=0,gettext=0,fontconfig=0;
#ifdef USE_FONTCONFIG
  fontconfig=1;
#endif
#ifdef USE_ALTIVEC
        altivec=1;
#endif
#ifdef USE_MP3
        mad=1;
#endif
#ifdef USE_AC3
        a52dec=1;
#endif
#ifdef USE_XVID_4
        xvid4=1;
#endif
#ifdef USE_X264
        X264=1;
#endif
#ifdef USE_FREETYPE
        freetype=1;
#endif
#ifdef USE_ESD
        esd=1;
#endif
#ifdef USE_ARTS
        arts=1;
#endif
#ifdef USE_VORBIS
        vorbis=1;
#endif
#ifdef ADM_WIN32
        win32=1;
#endif
#ifdef USE_FAAC
        faac=1;
#endif
#ifdef USE_FAAD
        faad=1;
#endif
#ifdef USE_LIBDCA
	if (dca->isAvailable())
        libdca=1;
#endif
#ifdef USE_AFTEN
        aften=1;
#endif
#ifdef USE_AMR_NB
	if (amrnb->isAvailable())
		libamrnb=1;
#endif
#ifdef HAVE_LIBMP3LAME
	lame=1;
#endif
#ifdef USE_SDL
	sdl=1;
#endif
#ifdef OSS_SUPPORT
	oss=1;
#endif
#ifdef ALSA_SUPPORT
	alsa=1;
#endif

#ifdef USE_XV
	xvideo=1;
#endif
#ifdef ARCH_X86
	x86=1;
#endif
#ifdef ARCH_X86_64
	x86_64=1;
#endif
#ifdef ARCH_POWERPC
	powerpc=1;
#endif
#ifdef HAVE_GETTEXT
	gettext=1;
#endif
    
	diaElemFrame videoFrame(QT_TR_NOOP("Video Codecs"));
	diaElemNotch tXvid4(xvid4, QT_TR_NOOP("Xvid"));
	diaElemNotch tX264(X264, QT_TR_NOOP("x264"));

	videoFrame.swallow(&tXvid4);
	videoFrame.swallow(&tX264);

	diaElemFrame audioFrame(QT_TR_NOOP("Audio Codecs"));	
	diaElemNotch tAften(aften, QT_TR_NOOP("Aften"));	
	diaElemNotch tLibamrnb(libamrnb, QT_TR_NOOP("amrnb"));
    diaElemNotch tFaac(faac, QT_TR_NOOP("FAAC"));
    diaElemNotch tFaad(faad, QT_TR_NOOP("FAAD2"));
	diaElemNotch tLame(lame, QT_TR_NOOP("LAME"));
	diaElemNotch tA52dec(a52dec, QT_TR_NOOP("liba52"));
	diaElemNotch tLibdca(libdca, QT_TR_NOOP("libdca"));
	diaElemNotch tMad(mad, QT_TR_NOOP("MAD"));
	diaElemNotch tVorbis(vorbis, QT_TR_NOOP("Vorbis"));

	audioFrame.swallow(&tAften);
	audioFrame.swallow(&tLibamrnb);	
	audioFrame.swallow(&tFaac);
	audioFrame.swallow(&tFaad);
	audioFrame.swallow(&tLame);
	audioFrame.swallow(&tA52dec);
	audioFrame.swallow(&tLibdca);
	audioFrame.swallow(&tMad);
	audioFrame.swallow(&tVorbis);

	diaElemNotch tArts(arts, QT_TR_NOOP("aRts"));
	diaElemNotch tEsd(esd, QT_TR_NOOP("ESD"));
        diaElemNotch tFontConfig(fontconfig, QT_TR_NOOP("Fontconfig"));
	diaElemNotch tFreetype(freetype, QT_TR_NOOP("FreeType 2"));
	diaElemNotch tGettext(gettext, QT_TR_NOOP("gettext"));
        diaElemNotch tAlsa(alsa, QT_TR_NOOP("ALSA"));
	diaElemNotch tOss(oss, QT_TR_NOOP("OSS"));
	diaElemNotch tSdl(sdl, QT_TR_NOOP("SDL"));
	diaElemNotch tXvideo(xvideo, QT_TR_NOOP("XVideo"));

	diaElemNotch tAltivec(altivec, QT_TR_NOOP("AltiVec"));
	diaElemNotch tPowerPc(powerpc, QT_TR_NOOP("PowerPC"));
	diaElemNotch tX86(x86, QT_TR_NOOP("x86"));
	diaElemNotch tX86_64(x86_64, QT_TR_NOOP("x86-64"));


	diaElem *codecElems[] = {&videoFrame, &audioFrame};
	diaElem *libsElems[] = {&tArts, &tEsd, &tFontConfig, &tFreetype, &tGettext, &tAlsa, &tOss, &tSdl, &tXvideo};
	diaElem *CPUElems[] = {&tAltivec, &tPowerPc, &tX86, &tX86_64};

	diaElemTabs tabCodec(QT_TR_NOOP("Codecs"), 2, codecElems);
	diaElemTabs tabLibs(QT_TR_NOOP("Libraries"), 9, libsElems);
	diaElemTabs tabCPU(QT_TR_NOOP("CPU"), 4, CPUElems);

	diaElemTabs *tabs[] = {&tabCodec, &tabLibs, &tabCPU};

    diaFactoryRunTabs(QT_TR_NOOP("Built-in Support"), 3, tabs);

    return 1;
}
