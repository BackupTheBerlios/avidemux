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

#ifdef __MINGW32__
#include <windows.h>
#include <excpt.h>
#endif

#include "config.h"
#include "ADM_default.h"
#include "ADM_threads.h"

#define __DECLARE__
#include "avi_vars.h"

#include "ADM_encoder/adm_encConfig.h"
#include "prefs.h"
#include "ADM_audiodevice/audio_out.h"
#include "ADM_toolkit/ADM_intfloat.h"

#ifdef USE_XVID_4
extern void xvid4_init(void);
#endif

extern uint8_t filterDynLoad(const char *path);

extern void  ADM_lavInit();
extern void  ADM_lavDestroy();
extern "C" {
     extern uint8_t     ADM_InitMemcpy(void);
};

#ifdef USE_SDL
extern "C" {
	#include "SDL.h"
}

#include "ADM_userInterfaces/ADM_commonUI/GUI_sdlRender.h"
#endif


void onexit( void );
//extern void automation(int argc, char **argv);

extern void registerVideoFilters( void );
extern void filterCleanUp( void );
extern void register_Encoders( void )  ;

extern void  buildDistMatrix( void );
extern void initScaleTab( void );
extern uint8_t initGUI( void );
extern void destroyGUI(void);
extern void COL_init(void);
extern uint8_t initFileSelector(void);
extern void AUDMEncoder_initDither(void);
extern void ADM_memStat( void );
extern void ADM_memStatInit( void );
extern void ADM_memStatEnd( void );
extern void getUIDescription(char*);
extern uint8_t ADM_ad_loadPlugins(const char *path);

#ifdef __MINGW32__
extern EXCEPTION_DISPOSITION exceptionHandler(struct _EXCEPTION_RECORD* pExceptionRec, void* pEstablisherFrame, struct _CONTEXT* pContextRecord, void* pDispatcherContext);
#else
extern void installSigHandler(void);
#endif

#ifdef __WIN32
extern bool getWindowsVersion(char* version);
extern void redirectStdoutToFile(void);
#endif

extern uint8_t  quotaInit(void);
extern void ADMImage_stat( void );
extern uint8_t win32_netInit(void);

extern int UI_Init(int nargc,char **nargv);
extern int UI_RunApp(void);

// Spidermonkey/Scripting stuff  
bool SpidermonkeyInit(void);
void SpidermonkeyDestroy(void);

extern pthread_mutex_t g_pSpiderMonkeyMutex;

#if defined(ADM_DEBUG) && defined(FIND_LEAKS)
extern const char* new_progname;
extern int check_leaks();
#endif

int main(int argc, char *argv[])
{
#if defined(__WIN32) && defined(USE_SDL)
	redirectStdoutToFile();
#endif

#if defined(ADM_DEBUG) && defined(FIND_LEAKS)
	new_progname = argv[0];
#endif

#ifndef __MINGW32__
	// thx smurf uk :)
    installSigHandler();
#endif

    printf("*************************\n");
    printf("  Avidemux v" VERSION);

  	if(ADM_SUBVERSION)
	{
		printf(" (r%04u)", ADM_SUBVERSION);
	}

    printf("\n*************************\n");
    printf(" http://www.avidemux.org\n");
    printf(" Code      : Mean, JSC, Gruntster \n");
    printf(" GFX       : Nestor Di , nestordi@augcyl.org\n");
    printf(" Design    : Jakub Misak\n");
    printf(" FreeBSD   : Anish Mistry, amistry@am-productions.biz\n");
    printf(" Audio     : Mihail Zenkov\n");
    printf(" MacOsX    : Kuisathaverat\n");
    printf(" Win32     : Gruntster\n\n");

#ifdef __GNUC__
	printf("Compiler: GCC %s\n", __VERSION__);
#endif

	printf("Build Target: ");

#if defined(__WIN32)
	printf("Microsoft Windows");
#elif defined(__APPLE__)
	printf("Apple");
#else
	printf("Linux");
#endif

#if defined(ADM_CPU_X86_32)
	printf(" (x86)");
#elif defined(ADM_CPU_X86_64)
	printf(" (x86-64)");
#elif defined(ADM_CPU_PPC)
	printf(" (PowerPC)");
#endif

	printf("\n");

	char uiDesc[15];
	getUIDescription(uiDesc);
	printf("User Interface: %s\n", uiDesc);

#ifdef __WIN32
	char version[250];

	if (getWindowsVersion(version))
		printf("Operating System: %s\n", version);
#endif

#if defined(__USE_LARGEFILE) && defined(__USE_LARGEFILE64)
	printf("\nLarge file available: %d offset\n", __USE_FILE_OFFSET64);
#endif

   // Start counting memory
	ADM_memStatInit();
	ADM_intFloatInit();

	printf("\nInitialising prefs\n");
	initPrefs();
	prefs->load();
    CpuCaps::init();
    
	register_Encoders();

#ifdef USE_SDL
	initSdl();
#endif

	atexit(onexit);

#ifdef __MINGW32__
    win32_netInit();
#endif

    UI_Init(argc,argv);
    AUDMEncoder_initDither();

#ifdef USE_XVID_4
    xvid4_init();
#endif

    initFileSelector();
    ADM_InitMemcpy();

	// Load .avidemuxrc
    quotaInit();

	if(!initGUI())
	{
		printf("\n Fatal : could not init GUI\n");
		exit(-1);
	}

    video_body = new ADM_Composer;

#ifdef HAVE_ENCODER
     registerVideoFilters();
#endif

	// Try load load external filter
	uint32_t loadpref=0;
	char *dynloadPath=NULL;

	prefs->get(FILTERS_AUTOLOAD_ACTIVE,&loadpref);
	prefs->get(FILTERS_AUTOLOAD_PATH,&dynloadPath);

	if(loadpref && dynloadPath)
	{
		filterDynLoad(dynloadPath);
	}
	
	ADM_dealloc(dynloadPath);
	//***************Plugins *********************
	// Load system wide audio decoder plugin
#ifdef __APPLE__
	char *adPlugins = ADM_getInstallRelativePath("../Resources/lib","ADM_plugins","audioDecoder");
#else
	char *adPlugins = ADM_getInstallRelativePath("lib","ADM_plugins","audioDecoder");
#endif

	ADM_ad_loadPlugins(adPlugins);
	delete [] adPlugins;

	// load local audio decoder plugins
	adPlugins=ADM_getHomeRelativePath("plugins","audioDecoder");
	ADM_ad_loadPlugins(adPlugins);
	delete [] adPlugins;
	//***************Plugins *********************
	
	
	
    ADM_lavInit();
#ifdef HAVE_AUDIO
    AVDM_audioInit();
#endif

    buildDistMatrix();
    initScaleTab();

    COL_init();	
    if(SpidermonkeyInit() == true)
        printf("Spidermonkey initialized.\n");
    else
		ADM_assert(0); 

#ifdef __MINGW32__
	__try1(exceptionHandler);
#endif

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
	ADM_lavDestroy();

#ifdef USE_SDL
	quitSdl();
#endif

#ifdef HAVE_AUDIO
	AVDM_cleanup();
#endif

	destroyGUI();

    printf("End of cleanup\n");
    ADMImage_stat();
    ADM_memStat();
    ADM_memStatEnd();
    printf("\nGoodbye...\n\n");

#if defined(ADM_DEBUG) && defined(FIND_LEAKS)
	check_leaks();
#endif
}
extern void checkCrashFile(void);
void dummyXref(void)
{
    checkCrashFile();
}
//EOF