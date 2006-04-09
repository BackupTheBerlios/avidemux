/***************************************************************************
                          main.cpp  -  description
                             -------------------
	Initialize the env.

    begin                : Sat Feb 2 2002
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


#include <stdlib.h>
#include <glib.h>
#include <signal.h>
#include <config.h>

#include <gtk/gtk.h>

#ifdef HAVE_DCGETTEXT
#include <libintl.h>
#include <locale.h>
#endif
#include "interface.h"
#include "support.h"

#define __DECLARE__
#include "avi_vars.h"
//#include "ADM_gui/GUI_vars.h"

#include "ADM_encoder/adm_encConfig.h"
#include "prefs.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_script/ADM_JSGlobal.h"

#ifdef USE_XVID_4
extern void xvid4_init(void);
#endif

typedef void *FCT_VOID(void *);
uint8_t lavformat_init(void);
#ifdef USE_FFMPEG
     extern "C" {
     extern void        avcodec_init(void );
     extern  void 	avcodec_register_all(void );
     extern uint8_t     ADM_InitMemcpy(void);
                       };
#endif                       

#ifdef USE_SDL
	extern "C" {
	#include "SDL/SDL.h"
	}
#endif
#include "ADM_toolkit/ADM_cpuCap.h"
void onexit( void );
//extern void automation(int argc, char **argv);
extern int automation(void );
extern void registerVideoFilters( void );
extern void filterCleanUp( void );
extern void register_Encoders( void )  ;
extern uint8_t dloadXvid( void );
extern uint8_t dloadXvidCVS( void );

extern void  buildDistMatrix( void );
extern void initScaleTab( void );
extern uint8_t initGUI( void );
extern void ADM_memStat( void );
extern uint8_t oplug_mpegInit(void);
extern void     COL_init(void);
extern uint8_t  initFileSelector(void);

extern "C"
{
extern void     VPInitLibrary();


};


void sig_segfault_handler(int signo);

extern uint8_t  quotaInit(void);
extern void ADMImage_stat( void );
extern uint8_t win32_netInit(void);
int global_argc;
char **global_argv;
int CpuCaps::myCpuCaps=0;
int main(int argc, char *argv[])
{
// Check for big files

#ifdef     __USE_LARGEFILE
#ifdef   __USE_LARGEFILE64
printf("\n LARGE FILE AVAILABLE : %d offset\n",  __USE_FILE_OFFSET64	);
#endif
#endif

/*
	Initialize Gettext if available
*/
#ifdef HAVE_DCGETTEXT
  setlocale (LC_ALL, "");

//#define ALOCALES "/usr/local/share/locale"
  bindtextdomain ("avidemux", ADMLOCALE);
  textdomain ("avidemux");
  printf("Locales for %s appear to be in %s\n","avidemux", ADMLOCALE);
  printf("\nI18N : %s \n",dgettext("avidemux","_File"));
#endif



// thx smurf uk :)
     signal(11, sig_segfault_handler); // show stacktrace on default

    	printf("\n*******************\n");
    	printf("  Avidemux 2, v  " VERSION "\n");
    	printf("*******************\n");
	printf(" http://fixounet.free.fr/avidemux\n");
	printf(" Code      : Mean & JSC \n");
	printf(" GFX       : Nestor Di , nestordi@augcyl.org\n");
	printf(" Testing   : Jakub Misak\n");
	printf(" FreeBSD   : Anish Mistry, amistry@am-productions.biz\n");


#if (defined( ARCH_X86)  || defined(ARCH_X86_64))
	printf("Arcc X86 X86_64 activated.\n"); 	
#endif
   

#ifdef USE_XX_XVID_CVS
	printf("Probing XvidCVS library....\n");
 	dloadXvidCVS(  );
#endif
   VPInitLibrary();
   register_Encoders( );
    atexit(onexit);

#ifndef CYG_MANGLING    
    g_thread_init(NULL);
#else
    win32_netInit();    
#endif
    gtk_set_locale();
    gtk_init(&argc, &argv);
    gdk_rgb_init();
#ifdef USE_XVID_4
	xvid4_init();
#endif
        initFileSelector();
	CpuCaps::init();
	ADM_InitMemcpy();
	
// Load .avidemuxrc
    quotaInit();
    prefs->load();

   if(!initGUI())
    	{
		printf("\n Fatal : could not init GUI\n");
		exit(-1);	
	}

    video_body = new ADM_Composer;
#ifdef HAVE_ENCODER
     registerVideoFilters(  );
#endif
     
#ifdef USE_FFMPEG
  
                		avcodec_init();
	                 	avcodec_register_all();
                                lavformat_init();
#endif
#ifdef HAVE_AUDIO
    AVDM_audioInit();
#endif    
    buildDistMatrix();
    initScaleTab();

    loadEncoderConfig();
    COL_init();
    
   
    if (argc >= 2)
    {
			  global_argc=argc;
			  global_argv=argv;
			  gtk_timeout_add( 300, (GtkFunction )automation, NULL );
				//automation();				
		}
   #ifdef USE_SDL
   	printf("Global SDL init...\n");
   	SDL_Init(0); //SDL_INIT_AUDIO+SDL_INIT_VIDEO);
   #endif
    oplug_mpegInit();
	if(SpidermonkeyInit() == true)
		printf("Spidermonkey initialized.\n");

    gtk_main();

	SpidermonkeyDestroy();

    return 0;
}
void onexit( void )
{
	filterCleanUp();
	ADMImage_stat();
	ADM_memStat();
	printf("\n Goodbye...\n\n");
}
void sig_segfault_handler(int signo)
{
     ADMImage_stat();
     g_on_error_stack_trace ("avidemux");
     printf("Memory stat:\n");

//   g_on_error_query (programname);


     signo=0; // will keep GCC happy
     exit(1); // _exit(1) ???
}
