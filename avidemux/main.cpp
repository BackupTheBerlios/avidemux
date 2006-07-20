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
#include "ADM_toolkit/ADM_threads.h"
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
extern void AUDMEncoder_initDither(void);
extern "C"
{
extern void VPInitLibrary(void);
extern void VPDeInitLibrary(void);


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
int sdl_version=0;
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
  bind_textdomain_codeset ("avidemux", "UTF-8");
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


#if defined( ARCH_X86)  
      printf("Compiled for X86 Arch.\n");
#endif
#if defined(ARCH_X86_64)
      printf("Compiled for X86_64 Arch.\n");
#endif
   

    // the one and only global preferences object
    printf("Initializing prefs\n");
    initPrefs();
    

   VPInitLibrary();
   register_Encoders( );
    atexit(onexit);
#ifdef USE_SDL
    sdl_version=(SDL_Linked_Version()->major*1000)+(SDL_Linked_Version()->minor*100)+
          (SDL_Linked_Version()->patch);
    
    printf("SDL support on Version %d\n",sdl_version);
#endif
 #ifdef USE_SDL
  if(sdl_version>1209)
  {
   	printf("Global SDL init...\n");
   	SDL_Init(SDL_INIT_EVERYTHING); //SDL_INIT_AUDIO+SDL_INIT_VIDEO);
  }
   #endif

#ifndef CYG_MANGLING    
    g_thread_init(NULL);
#else
    win32_netInit();    
#endif
    gdk_threads_init();
    gdk_threads_enter();
    gtk_set_locale();
    gtk_init(&argc, &argv);
    gdk_rgb_init();
    AUDMEncoder_initDither();
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
  if(sdl_version<=1209)
  {
   	printf("Global SDL init...\n");
   	SDL_Init(0); //SDL_INIT_AUDIO+SDL_INIT_VIDEO);
  }
#endif
      oplug_mpegInit();
	if(SpidermonkeyInit() == true)
		printf("Spidermonkey initialized.\n");

    gtk_main();
    gdk_threads_leave();
    printf("Normal exit\n");
    return 0;
}
void onexit( void )
{
  printf("Cleaning up\n");
        VPDeInitLibrary();
        delete video_body;
	// wait for thread to finish executing
	printf("Waiting for Spidermonkey to finish...\n");
	pthread_mutex_lock(&g_pSpiderMonkeyMutex);
	printf("Cleaning up Spidermonkey.\n");
	SpidermonkeyDestroy();
	pthread_mutex_unlock(&g_pSpiderMonkeyMutex);
        destroyPrefs();
        filterCleanUp();
  printf("End of cleanup\n");
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
