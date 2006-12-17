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

#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <signal.h>

#ifdef HAVE_GETTEXT
#include <libintl.h>
#include <locale.h>
#endif
#include "default.h"

#ifdef CYG_MANGLING
#define WIN32_CLASH
#include "windows.h"
#include "wingdi.h"
#endif

#define __DECLARE__
#include "avi_vars.h"

#include "ADM_encoder/adm_encConfig.h"
#include "prefs.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_script/ADM_JSGlobal.h"
#include "ADM_toolkit/ADM_intfloat.h"
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
#include "ADM_osSupport/ADM_cpuCap.h"
#include "ADM_osSupport/ADM_threads.h"
void onexit( void );
//extern void automation(int argc, char **argv);

extern void registerVideoFilters( void );
extern void filterCleanUp( void );
extern void register_Encoders( void )  ;
extern uint8_t dloadXvid( void );
extern uint8_t dloadXvidCVS( void );

extern void  buildDistMatrix( void );
extern void initScaleTab( void );
extern uint8_t initGUI( void );
extern uint8_t oplug_mpegInit(void);
extern void     COL_init(void);
extern uint8_t  initFileSelector(void);
extern void AUDMEncoder_initDither(void);
extern void ADM_memStat( void );
extern void ADM_memStatInit( void );
extern void ADM_memStatEnd( void );


void sig_segfault_handler(int signo);

extern uint8_t  quotaInit(void);
extern void ADMImage_stat( void );
extern uint8_t win32_netInit(void);

extern int UI_Init(int nargc,char **nargv);
extern int UI_RunApp(void);
int CpuCaps::myCpuCaps=0;

int main(int argc, char *argv[])
{
// Check for big files
int sdl_version=0;

/*
	Initialize Gettext if available
*/



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

#ifdef     __USE_LARGEFILE
#ifdef   __USE_LARGEFILE64
printf("\n LARGE FILE AVAILABLE : %d offset\n",  __USE_FILE_OFFSET64	);
#endif
#endif
#ifdef HAVE_GETTEXT
  char *local=setlocale (LC_ALL, "");
  bindtextdomain ("avidemux", ADMLOCALE);
  bind_textdomain_codeset ("avidemux", "UTF-8");
  
  if(local)
    printf("[Locale] setlocale %s \n",local);
  local=textdomain(NULL);
  textdomain ("avidemux");
  if(local)
    printf("[Locale] Textdomain was %s \n",local);
  local=textdomain(NULL);
  if(local)
    printf("[Locale] Textdomain is now  %s \n",local);
  
  printf("[Locale] Files  for %s appear to be in %s\n","avidemux", ADMLOCALE);
  printf("[Locale] Test : %s \n",dgettext("avidemux","_File"));
#endif

   // Start counting memory
  ADM_memStatInit(  );
  ADM_intFloatInit();
    printf("Initializing prefs\n");
    initPrefs();

  register_Encoders( );
  atexit(onexit);
  
#ifdef USE_SDL
    sdl_version=(SDL_Linked_Version()->major*1000)+(SDL_Linked_Version()->minor*100)+
          (SDL_Linked_Version()->patch);
    printf("SDL support on Version %d\n",sdl_version);
  if(sdl_version>1209)
  {
      printf("Global SDL init...\n");
      SDL_Init(SDL_INIT_EVERYTHING); //SDL_INIT_AUDIO+SDL_INIT_VIDEO);
  }
#endif


    
#ifdef CYG_MANGLING    
    win32_netInit();
#endif
    UI_Init(argc,argv);
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

    COL_init();

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
    else
    {
      ADM_assert(0); 
    }

    
    UI_RunApp();
    printf("Normal exit\n");
    return 0;
}
void onexit( void )
{
  printf("Cleaning up\n");
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
        ADM_memStat(  );
        ADM_memStatEnd(  );
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
