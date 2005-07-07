/***************************************************************************
                          ADM_xvid.cpp  -  description
                             -------------------

	Use dlopen to open xvid and use it as a separate encoder
 	Decoding is still done by divx

    begin                : Fri Jul 12 2002
    copyright            : (C) 2002 by mean
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
#include "config.h"
 
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <math.h>

#include "ADM_library/default.h"
#include "ADM_codecs/ADM_codec.h"
//#include "ADM_gui/GUI_xvidparam.h"

uint8_t xvid_cvs_loaded=0;
static myENC_RESULT xvid_res;


#ifdef USE_XX_XVID_CVS
#include "ADM_codecs/ADM_xvid.h"
#include "xvidcvs.h"
#include <ADM_assert.h>

static void *dllHandle=NULL;

// api v2.0

typedef int(*XVID_PROTO) (void *handle, int opt,  void *param1, void *param2);

static XVID_PROTO xx_xvid_init=NULL;
static XVID_PROTO xx_xvid_encore=NULL;
static XVID_PROTO xx_xvid_decore=NULL;

//
static XVID_ENC_FRAME 	fr;
static XVID_ENC_PARAM 	xparam;
static XVID_ENC_STATS 	xstats;
#define XVID_INIT xx_xvid_init
#define XVID_ENCORE xx_xvid_encore


//
//	Dlopen Xvid and check everything is ok !
//
uint8_t dloadXvidCVS( void )
{
uint8_t ok=1;

     dllHandle=  dlopen("libxvidcore.so.3",RTLD_NOW);
     if(!dllHandle)
     			{

               	printf( "XvidCVS library not found, deactivated!\n");
            		return 0;
          		}
     // now setup
      #define   SYMBOL(x) if(ok) { \
      				xx_##x=(XVID_PROTO)dlsym(dllHandle,#x); \
          			 if(dlerror()) \
              			{ok=0;printf(#x);} \
                 }

      				SYMBOL(  xvid_init);
      				SYMBOL(  xvid_encore);
      				SYMBOL(  xvid_decore);
                 if(ok)
                 		{
                      	printf("XvidCVS library successfully loaded !\n");
			xvid_cvs_loaded=1;
                       	return 1;
                     }
                 	printf("XvidCVS failed !\n");

            		return 0;

}

#endif
