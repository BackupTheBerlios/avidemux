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
  uint32_t faac=0,faad=0,libdca=0,aften=0,libamrnb=0,lame=0,sdl=0,oss=0,xvideo=0,x86=0,x86_64=0;
  uint32_t powerpc=0,gettext=0;
  
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
#ifdef CYG_MANGLING
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
#ifdef USE_XV
	xvideo=1;
#endif
#ifdef ARCH_X86
	x86=1;
#endif
#ifdef ARCH_X86_64
	x86-64=1;
#endif
#ifdef ARCH_POWERPC
	powerpc=1;
#endif
#ifdef HAVE_GETTEXT
	gettext=1;
#endif
    
	diaElemFrame videoFrame(_("Video codecs:"));
	diaElemNotch tXvid4(xvid4, _("Xvid"));
	diaElemNotch tX264(X264, _("x264"));

	videoFrame.swallow(&tXvid4);
	videoFrame.swallow(&tX264);

	diaElemFrame audioFrame(_("Audio codecs:"));	
	diaElemNotch tAften(aften, _("Aften"));	
	diaElemNotch tLibamrnb(libamrnb, _("amrnb"));
    diaElemNotch tFaac(faac, _("FAAC"));
    diaElemNotch tFaad(faad, _("FAAD2"));
	diaElemNotch tLame(lame, _("LAME"));
	diaElemNotch tA52dec(a52dec, _("liba52"));
	diaElemNotch tLibdca(libdca, _("libdca"));
	diaElemNotch tMad(mad, _("MAD"));
	diaElemNotch tVorbis(vorbis, _("Ogg Vorbis"));

	audioFrame.swallow(&tAften);
	audioFrame.swallow(&tLibamrnb);	
	audioFrame.swallow(&tFaac);
	audioFrame.swallow(&tFaad);
	audioFrame.swallow(&tLame);
	audioFrame.swallow(&tA52dec);
	audioFrame.swallow(&tLibdca);
	audioFrame.swallow(&tMad);
	audioFrame.swallow(&tVorbis);

	diaElemFrame miscFrame(_("Miscellaneous libraries:"));
	diaElemNotch tArts(arts, _("aRts"));
	diaElemNotch tEsd(esd, _("ESD"));
	diaElemNotch tFreetype(freetype, _("FreeType 2"));
	diaElemNotch tGettext(gettext, _("gettext"));
	diaElemNotch tOss(oss, _("OSS"));
	diaElemNotch tSdl(sdl, _("SDL"));
	diaElemNotch tXvideo(xvideo, _("XVideo"));

	miscFrame.swallow(&tArts);
	miscFrame.swallow(&tEsd);
	miscFrame.swallow(&tFreetype);
	miscFrame.swallow(&tGettext);
	miscFrame.swallow(&tOss);
	miscFrame.swallow(&tSdl);
	miscFrame.swallow(&tXvideo);
    
	diaElemFrame systemFrame(_("CPU:"));
    diaElemNotch tAltivec(altivec, _("AltiVec"));
	diaElemNotch tPowerPc(powerpc, _("PowerPC"));
	diaElemNotch tX86(x86, _("x86"));
	diaElemNotch tX86_64(x86_64, _("x86-64"));

	systemFrame.swallow(&tAltivec);
	systemFrame.swallow(&tPowerPc);
	systemFrame.swallow(&tX86);
	systemFrame.swallow(&tX86_64);

	diaElem *codecElems[] = {&videoFrame, &audioFrame};
	diaElem *miscElems[] = {&miscFrame, &systemFrame};

	diaElemTabs tabCodec(_("Codecs"), 2, codecElems);
	diaElemTabs tabMisc(_("Miscellaneous"), 2, miscElems);

	diaElemTabs *tabs[] = {&tabCodec, &tabMisc};

    diaFactoryRunTabs(_("Built-in Support"), 2, tabs);

    return 1;
}
