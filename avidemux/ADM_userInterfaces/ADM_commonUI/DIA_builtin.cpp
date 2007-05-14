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
  uint32_t faac=0,faad=0,libdca=0,aften=0,libamrnb=0;
  
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

        
#define CREATE_TOGGLE(x)  diaElemNotch     t##x(x,_("Option "#x));
    
    
    CREATE_TOGGLE(mad)
    CREATE_TOGGLE(a52dec)
    CREATE_TOGGLE(xvid4)
    CREATE_TOGGLE(X264)
    CREATE_TOGGLE(freetype)
    CREATE_TOGGLE(esd)
    CREATE_TOGGLE(arts)
    CREATE_TOGGLE(vorbis)
    CREATE_TOGGLE(faac)
    CREATE_TOGGLE(faad)
    
    CREATE_TOGGLE(libdca)
    CREATE_TOGGLE(altivec)
    CREATE_TOGGLE(win32)
    CREATE_TOGGLE(aften)
	CREATE_TOGGLE(libamrnb)
    
      diaElem *elems[]={
#undef CREATE_TOGGLE
#define CREATE_TOGGLE(x) &(t##x),
    CREATE_TOGGLE(mad)
    CREATE_TOGGLE(a52dec)
    CREATE_TOGGLE(xvid4)
    CREATE_TOGGLE(X264)
    CREATE_TOGGLE(freetype)
    CREATE_TOGGLE(esd)
    CREATE_TOGGLE(arts)
    CREATE_TOGGLE(vorbis)
    CREATE_TOGGLE(faac)
    CREATE_TOGGLE(faad)
    
    CREATE_TOGGLE(aften)
    CREATE_TOGGLE(libdca)
    CREATE_TOGGLE(altivec)
    CREATE_TOGGLE(win32)
	CREATE_TOGGLE(libamrnb)
      
      };
    diaFactoryRun(_("Built-in Support"),sizeof(elems)/sizeof(diaElem *),elems);

    return 1;

}
