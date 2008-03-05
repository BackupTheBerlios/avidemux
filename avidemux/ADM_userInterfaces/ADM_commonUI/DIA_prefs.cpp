/***************************************************************************
  DIA_prefs.cpp
  (C) 2007 Mean Fixounet@free.fr 
***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include "config.h"
#include "default.h"
# include "prefs.h"

#include "ADM_audiodevice/audio_out.h"
#include "ADM_assert.h"
#include "GUI_render.h"
#include "ADM_osSupport/ADM_cpuCap.h"
#include "ADM_toolkit/toolkit.hxx"
#include "DIA_factory.h"

#ifdef __WIN32
#include "GUI_sdlRender.h"
#endif

extern void 		AVDM_audioPref( void );



uint8_t DIA_Preferences(void);

/**
      \fn DIA_Preferences
      \brief Handle preference dialog
*/
uint8_t DIA_Preferences(void)
{
uint32_t olddevice,newdevice;

uint32_t	lavcodec_mpeg=0;
uint32_t        use_odml=0;
uint32_t	autosplit=0;
uint32_t render;
uint32_t useTray=0;
uint32_t useMaster=0;
uint32_t useAutoIndex=0;
uint32_t useSwap=0;
uint32_t useNuv=0;
uint32_t lavcThreads=0,x264Threads=0,xvidThreads=0;
uint32_t encodePriority=2;
uint32_t indexPriority=2;
uint32_t playbackPriority=0;
uint32_t downmix;
uint32_t mpeg_no_limit=0;
uint32_t msglevel=2;
uint32_t activeXfilter=0;
uint32_t mixer=0;
char     *filterPath=NULL;
char     *alsaDevice=NULL;
uint32_t autovbr=0;
uint32_t autoindex=0;
uint32_t autounpack=0;
uint32_t alternate_mp3_tag=1;
uint32_t pp_type=3;
uint32_t pp_value=5;
uint32_t hzd,vzd,dring;
uint32_t capsMMX,capsMMXEXT,caps3DNOW,caps3DNOWEXT,capsSSE,capsSSE2,capsSSE3,capsSSSE3,capsAll;

uint32_t useGlobalGlyph=0;
char     *globalGlyphName=NULL;

	olddevice=newdevice=AVDM_getCurrentDevice();

        // Default pp
         if(!prefs->get(DEFAULT_POSTPROC_TYPE,&pp_type)) pp_type=3;
         if(!prefs->get(DEFAULT_POSTPROC_VALUE,&pp_value)) pp_value=3;
#define DOME(x,y) y=!!(pp_type & x)
    
    DOME(1,hzd);
    DOME(2,vzd);
    DOME(4,dring);
     
// Cpu caps
#define CPU_CAPS(x)    	if(CpuCaps::myCpuMask & ADM_CPUCAP_##x) caps##x=1; else caps##x=0;
    
    	if(CpuCaps::myCpuMask==ADM_CPUCAP_ALL) capsAll=1; else capsAll=0;
    	CPU_CAPS(MMX);
    	CPU_CAPS(MMXEXT);
    	CPU_CAPS(3DNOW);
    	CPU_CAPS(3DNOWEXT);
    	CPU_CAPS(SSE);
    	CPU_CAPS(SSE2);
    	CPU_CAPS(SSE3);
    	CPU_CAPS(SSSE3);
    
        // Alsa
#ifdef ALSA_SUPPORT
        if( prefs->get(DEVICE_AUDIO_ALSA_DEVICE, &alsaDevice) != RC_OK )
                alsaDevice = ADM_strdup("plughw:0,0");
#endif
        // autovbr
        prefs->get(FEATURE_AUTO_BUILDMAP,&autovbr);
        // autoindex
        prefs->get(FEATURE_AUTO_REBUILDINDEX,&autoindex);
        // Global glyph
        prefs->get(FEATURE_GLOBAL_GLYPH_ACTIVE,&useGlobalGlyph);
        prefs->get(FEATURE_GLOBAL_GLYPH_NAME,&globalGlyphName);
         // autoindex
        prefs->get(FEATURE_AUTO_UNPACK,&autounpack);
        // Alternate mp3 tag (haali)
        prefs->get(FEATURE_ALTERNATE_MP3_TAG,&alternate_mp3_tag);
        
        // Video renderer
        if(prefs->get(DEVICE_VIDEODEVICE,&render)!=RC_OK)
        {       
                render=(uint32_t)RENDER_GTK;
        }
        // SysTray
        if(!prefs->get(FEATURE_USE_SYSTRAY,&useTray)) 
                useTray=0;
        // Accept mpeg for DVD when fq!=48 kHz
        if(!prefs->get(FEATURE_MPEG_NO_LIMIT,&mpeg_no_limit)) mpeg_no_limit=0;

        // Multithreads
        prefs->get(FEATURE_THREADING_LAVC, &lavcThreads);
        prefs->get(FEATURE_THREADING_X264, &x264Threads);
        prefs->get(FEATURE_THREADING_XVID, &xvidThreads);

		// Encoding priority
		if(!prefs->get(PRIORITY_ENCODING, &encodePriority))
                encodePriority=2;
		// Indexing / unpacking priority
		if(!prefs->get(PRIORITY_INDEXING, &indexPriority))
                indexPriority=2;
		// Playback priority
		if(!prefs->get(PRIORITY_PLAYBACK, &playbackPriority))
                playbackPriority=0;

        // VCD/SVCD split point		
        if(!prefs->get(SETTINGS_MPEGSPLIT, &autosplit))
                autosplit=690;		
                        
        if(!prefs->get(FEATURE_USE_LAVCODEC_MPEG, &lavcodec_mpeg))
                lavcodec_mpeg=0;
        // Open DML (Gmv)
        if(!prefs->get(FEATURE_USE_ODML, &use_odml))
          use_odml=0;
#if defined(ALSA_SUPPORT) || defined (OSS_SUPPORT)
		// Master or PCM for audio
        if(!prefs->get(FEATURE_AUDIOBAR_USES_MASTER, &useMaster))
                useMaster=0;
#endif
        // Autoindex files
        if(!prefs->get(FEATURE_TRYAUTOIDX, &useAutoIndex))
                useAutoIndex=0;

        // SWAP A&B if A>B
        if(!prefs->get(FEATURE_SWAP_IF_A_GREATER_THAN_B, &useSwap))
                useSwap=0;
        // No nuv sync
        if(!prefs->get(FEATURE_DISABLE_NUV_RESYNC, &useNuv))
                useNuv=0;
        // Get level of message verbosity
        prefs->get(MESSAGE_LEVEL,&msglevel);
        // External filter
         prefs->get(FILTERS_AUTOLOAD_ACTIVE,&activeXfilter);
        // Downmix default
        if(prefs->get(DOWNMIXING_PROLOGIC,&downmix)!=RC_OK)
        {       
            downmix=0;
        }
        olddevice=newdevice=AVDM_getCurrentDevice();
        // Audio device
        /************************ Build diaelems ****************************************/
        diaElemToggle useSysTray(&useTray,QT_TR_NOOP("_Use systray while encoding"));
        diaElemToggle allowAnyMpeg(&mpeg_no_limit,QT_TR_NOOP("_Accept non-standard audio frequency for DVD"));
        diaElemToggle useLavcodec(&lavcodec_mpeg,QT_TR_NOOP("_Use libavcodec MPEG-2 decoder"));
        diaElemToggle openDml(&use_odml,QT_TR_NOOP("Create _OpenDML files"));
        diaElemToggle autoIndex(&useAutoIndex,QT_TR_NOOP("Automatically _index MPEG files"));
        diaElemToggle autoSwap(&useSwap,QT_TR_NOOP("Automatically _swap A and B if A>B"));
        diaElemToggle nuvAudio(&useNuv,QT_TR_NOOP("_Disable NUV audio sync"));        
        
        diaElemToggle togAutoVbr(&autovbr,QT_TR_NOOP("Automatically _build VBR map"));
        diaElemToggle togAutoIndex(&autoindex,QT_TR_NOOP("Automatically _rebuild index"));
        diaElemToggle togAutoUnpack(&autounpack,QT_TR_NOOP("Automatically remove _packed bitstream"));

        diaElemFrame frameSimd(QT_TR_NOOP("SIMD"));

		diaElemToggle capsToggleAll(&capsAll,QT_TR_NOOP("Enable all SIMD"));
        diaElemToggle capsToggleMMX(&capsMMX, QT_TR_NOOP("Enable MMX"));
		diaElemToggle capsToggleMMXEXT(&capsMMXEXT, QT_TR_NOOP("Enable MMXEXT"));
		diaElemToggle capsToggle3DNOW(&caps3DNOW, QT_TR_NOOP("Enable 3DNOW"));
		diaElemToggle capsToggle3DNOWEXT(&caps3DNOWEXT, QT_TR_NOOP("Enable 3DNOWEXT"));
		diaElemToggle capsToggleSSE(&capsSSE, QT_TR_NOOP("Enable SSE"));
		diaElemToggle capsToggleSSE2(&capsSSE2, QT_TR_NOOP("Enable SSE2"));
		diaElemToggle capsToggleSSE3(&capsSSE3, QT_TR_NOOP("Enable SSE3"));
		diaElemToggle capsToggleSSSE3(&capsSSSE3, QT_TR_NOOP("Enable SSSE3"));

		capsToggleAll.link(0, &capsToggleMMX);
		capsToggleAll.link(0, &capsToggleMMXEXT);
		capsToggleAll.link(0, &capsToggle3DNOW);
		capsToggleAll.link(0, &capsToggle3DNOWEXT);
		capsToggleAll.link(0, &capsToggleSSE);
		capsToggleAll.link(0, &capsToggleSSE2);
		capsToggleAll.link(0, &capsToggleSSE3);
		capsToggleAll.link(0, &capsToggleSSSE3);

		frameSimd.swallow(&capsToggleAll);
		frameSimd.swallow(&capsToggleMMX);
		frameSimd.swallow(&capsToggleMMXEXT);
		frameSimd.swallow(&capsToggle3DNOW);
		frameSimd.swallow(&capsToggle3DNOWEXT);
		frameSimd.swallow(&capsToggleSSE);
		frameSimd.swallow(&capsToggleSSE2);
		frameSimd.swallow(&capsToggleSSE3);
		frameSimd.swallow(&capsToggleSSSE3);

		diaElemThreadCount lavcThreadCount(&lavcThreads, QT_TR_NOOP("_lavc threads:"));
		diaElemThreadCount x264ThreadCount(&x264Threads, QT_TR_NOOP("_x264 threads:"));
		diaElemThreadCount xvidThreadCount(&xvidThreads, QT_TR_NOOP("X_vid threads:"));

		diaElemFrame frameThread(QT_TR_NOOP("Multi-threading"));
		frameThread.swallow(&lavcThreadCount);
		frameThread.swallow(&x264ThreadCount);
		frameThread.swallow(&xvidThreadCount);

		diaMenuEntry priorityEntries[] = {
                             {0,       QT_TR_NOOP("High"),NULL}
                             ,{1,      QT_TR_NOOP("Above normal"),NULL}
                             ,{2,      QT_TR_NOOP("Normal"),NULL}
							 ,{3,      QT_TR_NOOP("Below normal"),NULL}
							 ,{4,      QT_TR_NOOP("Low"),NULL}
        };
		diaElemMenu menuEncodePriority(&encodePriority,QT_TR_NOOP("_Encoding priority:"), sizeof(priorityEntries)/sizeof(diaMenuEntry), priorityEntries,"");
		diaElemMenu menuIndexPriority(&indexPriority,QT_TR_NOOP("_Indexing/unpacking priority:"), sizeof(priorityEntries)/sizeof(diaMenuEntry), priorityEntries,"");
		diaElemMenu menuPlaybackPriority(&playbackPriority,QT_TR_NOOP("_Playback priority:"), sizeof(priorityEntries)/sizeof(diaMenuEntry), priorityEntries,"");

		diaElemFrame framePriority(QT_TR_NOOP("Prioritisation"));
		framePriority.swallow(&menuEncodePriority);
		framePriority.swallow(&menuIndexPriority);
		framePriority.swallow(&menuPlaybackPriority);

        diaElemUInteger autoSplit(&autosplit,QT_TR_NOOP("_Split MPEG files every (MB):"),10,4096);
        
        diaElemToggle   togTagMp3(&alternate_mp3_tag,QT_TR_NOOP("_Use alternative tag for MP3 in .mp4"));
        
        diaMenuEntry videoMode[]={
                             {RENDER_GTK,      QT_TR_NOOP("GTK+ (slow)"),NULL}
#ifdef USE_XV
                             ,{RENDER_XV,   QT_TR_NOOP("XVideo (best)"),NULL}
#endif
#ifdef USE_SDL
#ifdef __WIN32
                             ,{RENDER_SDL,      QT_TR_NOOP("SDL (GDI)"),NULL}
							 ,{RENDER_DIRECTX,      QT_TR_NOOP("SDL (DirectX)"),NULL}
#else
							 ,{RENDER_SDL,      QT_TR_NOOP("SDL (good)"),NULL}
#endif
#endif
        };        
        diaElemMenu menuVideoMode(&render,QT_TR_NOOP("Video _display:"), sizeof(videoMode)/sizeof(diaMenuEntry),videoMode,"");
        
        
        
        diaMenuEntry msgEntries[]={
                             {0,       QT_TR_NOOP("No alerts"),NULL}
                             ,{1,      QT_TR_NOOP("Display only error alerts"),NULL}
                             ,{2,      QT_TR_NOOP("Display all alerts"),NULL}
        };
        diaElemMenu menuMessage(&msglevel,QT_TR_NOOP("_Message level:"), sizeof(msgEntries)/sizeof(diaMenuEntry),msgEntries,"");
        
        
#if defined(ALSA_SUPPORT) || defined (OSS_SUPPORT)
        diaMenuEntry volumeEntries[]={
                             {0,       QT_TR_NOOP("PCM"),NULL}
                             ,{1,      QT_TR_NOOP("Master"),NULL}};
        diaElemMenu menuVolume(&useMaster,QT_TR_NOOP("_Volume control:"), sizeof(volumeEntries)/sizeof(diaMenuEntry),volumeEntries,"");
#endif
        
        
         diaMenuEntry mixerEntries[]={
                             {0,       QT_TR_NOOP("No downmixing"),NULL}
                             ,{1,       QT_TR_NOOP("Stereo"),NULL}
                             ,{2,      QT_TR_NOOP("Pro Logic"),NULL}
                              ,{3,      QT_TR_NOOP("Pro Logic II"),NULL}
         };
        diaElemMenu menuMixer(&downmix,QT_TR_NOOP("_Local playback downmixing:"), sizeof(mixerEntries)/sizeof(diaMenuEntry),mixerEntries,"");

		diaMenuEntry audioEntries[] =
		{
		#ifdef ALSA_SUPPORT
			{DEVICE_ALSA, QT_TR_NOOP("ALSA")},
		#endif
		#ifdef USE_ARTS
			{DEVICE_ARTS, QT_TR_NOOP("aRts")},
		#endif
		#ifdef __APPLE__
			{DEVICE_COREAUDIO, QT_TR_NOOP("Core Audio")},
		#endif
		#ifdef USE_ESD
			{DEVICE_ESD, QT_TR_NOOP("ESD")},
		#endif
		#ifdef USE_JACK
			{DEVICE_JACK, QT_TR_NOOP("JACK")},
		#endif
		#ifdef OSS_SUPPORT
			{DEVICE_OSS, QT_TR_NOOP("OSS")},
		#endif
		#if	defined(USE_SDL) && !defined(__WIN32)
			{DEVICE_SDL, QT_TR_NOOP("SDL")},
		#endif
		#ifdef __WIN32
			{DEVICE_WIN32, QT_TR_NOOP("Win32")},
		#endif
			{DEVICE_DUMMY, QT_TR_NOOP("None")}
		};

        diaElemMenu menuAudio(&newdevice,QT_TR_NOOP("_Audio output:"), sizeof(audioEntries)/sizeof(diaMenuEntry),audioEntries,"");
                
#ifdef ALSA_SUPPORT
		diaElemText entryAlsaDevice(&alsaDevice,QT_TR_NOOP("ALSA _device:"),NULL);

          int z,m;
          m=sizeof(audioEntries)/sizeof(diaMenuEntry);
          for(z=0;z<m;z++)
          {
            if(audioEntries[z].val==DEVICE_ALSA)
                menuAudio.link(&(audioEntries[z]),1,&entryAlsaDevice);
          }
#endif
        // default Post proc
     diaElemToggle     fhzd(&hzd,QT_TR_NOOP("_Horizontal deblocking"));
     diaElemToggle     fvzd(&vzd,QT_TR_NOOP("_Vertical deblocking"));
     diaElemToggle     fdring(&dring,QT_TR_NOOP("De_ringing"));
     diaElemUInteger   postProcStrength(&pp_value,QT_TR_NOOP("_Strength:"),0,5);
     diaElemFrame      framePP(QT_TR_NOOP("Default Postprocessing"));
     
     framePP.swallow(&fhzd);
     framePP.swallow(&fvzd);
     framePP.swallow(&fdring);
     framePP.swallow(&postProcStrength);
     
        // Filter path
        if( prefs->get(FILTERS_AUTOLOAD_PATH, &filterPath) != RC_OK )
#ifndef __WIN32
               filterPath = ADM_strdup("/tmp");
#else
               filterPath = ADM_strdup("c:\\");
#endif
        diaElemDirSelect  entryFilterPath(&filterPath,QT_TR_NOOP("_Filter directory:"),QT_TR_NOOP("Select filter directory"));
		diaElemToggle loadEx(&activeXfilter,QT_TR_NOOP("_Load external filters"));
		loadEx.link(1, &entryFilterPath);

		diaElemToggle togGlobalGlyph(&useGlobalGlyph, QT_TR_NOOP("Use _Global GlyphSet"));
		diaElemFile  entryGLyphPath(0,&globalGlyphName,QT_TR_NOOP("Gl_yphSet:"), NULL, QT_TR_NOOP("Select GlyphSet file"));
		togGlobalGlyph.link(1, &entryGLyphPath);

        /* User Interface */
        diaElem *diaUser[]={&useSysTray,&menuMessage};
        diaElemTabs tabUser(QT_TR_NOOP("User Interface"),2,diaUser);
        
         /* Automation */
        diaElem *diaAuto[]={&autoSwap,&togAutoVbr,&togAutoIndex,&togAutoUnpack,&autoIndex,};
        diaElemTabs tabAuto(QT_TR_NOOP("Automation"),5,diaAuto);
        
        /* Input */
        diaElem *diaInput[]={&nuvAudio,&useLavcodec};
        diaElemTabs tabInput(QT_TR_NOOP("Input"),2,(diaElem **)diaInput);
        
        /* Output */
        diaElem *diaOutput[]={&autoSplit,&openDml,&allowAnyMpeg,&togTagMp3};
        diaElemTabs tabOutput(QT_TR_NOOP("Output"),4,(diaElem **)diaOutput);
        
        /* Audio */
#if defined(ALSA_SUPPORT)
        diaElem *diaAudio[]={&menuMixer,&menuVolume,&menuAudio,&entryAlsaDevice};
        diaElemTabs tabAudio(QT_TR_NOOP("Audio"),4,(diaElem **)diaAudio);
#elif defined(OSS_SUPPORT)
        diaElem *diaAudio[]={&menuMixer,&menuVolume,&menuAudio};
        diaElemTabs tabAudio(QT_TR_NOOP("Audio"),3,(diaElem **)diaAudio);
#else
        diaElem *diaAudio[]={&menuMixer,&menuAudio};
        diaElemTabs tabAudio(QT_TR_NOOP("Audio"),2,(diaElem **)diaAudio);
#endif
        
        /* Video */
        diaElem *diaVideo[]={&menuVideoMode,&framePP};
        diaElemTabs tabVideo(QT_TR_NOOP("Video"),2,(diaElem **)diaVideo);
        
        /* CPU tab */
		diaElem *diaCpu[]={&frameSimd};
		diaElemTabs tabCpu(QT_TR_NOOP("CPU"),1,(diaElem **)diaCpu);

        /* Threading tab */
		diaElem *diaThreading[]={&frameThread, &framePriority};
		diaElemTabs tabThreading(QT_TR_NOOP("Threading"),2,(diaElem **)diaThreading);

        /* Global Glyph tab */
        diaElem *diaGlyph[]={&togGlobalGlyph,&entryGLyphPath};
        diaElemTabs tabGlyph(QT_TR_NOOP("Global GlyphSet"),2,(diaElem **)diaGlyph);

        /* Xfilter tab */
        diaElem *diaXFilter[]={&loadEx,&entryFilterPath};
        diaElemTabs tabXfilter(QT_TR_NOOP("External Filters"),2,(diaElem **)diaXFilter);
                                    
// SET
        diaElemTabs *tabs[]={&tabUser,&tabAuto,&tabInput,&tabOutput,&tabAudio,&tabVideo,&tabCpu,&tabThreading,&tabGlyph,&tabXfilter};
        if( diaFactoryRunTabs(QT_TR_NOOP("Preferences"),10,tabs))
	{
        	
        	// cpu caps
        		if(capsAll)
        		{
        			CpuCaps::myCpuMask=ADM_CPUCAP_ALL;
        		}else
        		{
        			CpuCaps::myCpuMask=0;
#undef CPU_CAPS
#define CPU_CAPS(x)    	if(caps##x) CpuCaps::myCpuMask|= ADM_CPUCAP_##x;        	    	
        	    	CPU_CAPS(MMX);
        	    	CPU_CAPS(MMXEXT);
        	    	CPU_CAPS(3DNOW);
        	    	CPU_CAPS(3DNOWEXT);
        	    	CPU_CAPS(SSE);
        	    	CPU_CAPS(SSE2);
        	    	CPU_CAPS(SSE3);
        	    	CPU_CAPS(SSSE3);
        		}
        		prefs->set(FEATURE_CPU_CAPS,CpuCaps::myCpuMask);

        		// Glyphs
               prefs->set(FEATURE_GLOBAL_GLYPH_ACTIVE,useGlobalGlyph);
               prefs->set(FEATURE_GLOBAL_GLYPH_NAME,globalGlyphName);

                // Postproc
                #undef DOME
                #define DOME(x,y) if(y) pp_type |=x;
                pp_type=0;
                DOME(1,hzd);
                DOME(2,vzd);
                DOME(4,dring);
                prefs->set(DEFAULT_POSTPROC_TYPE,pp_type);
                prefs->set(DEFAULT_POSTPROC_VALUE,pp_value);
                //
                 prefs->set(FEATURE_AUTO_UNPACK,autounpack);
                 // autovbr
                prefs->set(FEATURE_AUTO_BUILDMAP,autovbr);
                // autoindex
                prefs->set(FEATURE_AUTO_REBUILDINDEX,autoindex);
                // Alsa
#ifdef ALSA_SUPPORT
                if(alsaDevice)
                {
                   prefs->set(DEVICE_AUDIO_ALSA_DEVICE, alsaDevice);
                   ADM_dealloc(alsaDevice);
                   alsaDevice=NULL;
                }
#endif
                // Device
                if(olddevice!=newdevice)
                {
                      AVDM_switch((AUDIO_DEVICE)newdevice);
                }
                // Downmixing (default)
                prefs->set(DOWNMIXING_PROLOGIC,downmix);
#if defined(ALSA_SUPPORT) || defined (OSS_SUPPORT)
                // Master or PCM
                prefs->set(FEATURE_AUDIOBAR_USES_MASTER, useMaster);
#endif
                // allow non std audio fq for dvd
                prefs->set(FEATURE_MPEG_NO_LIMIT, mpeg_no_limit);
                // Video render
                prefs->set(DEVICE_VIDEODEVICE,render);
                // Mpeg /lavcodec
                prefs->set(FEATURE_USE_LAVCODEC_MPEG, lavcodec_mpeg);
                // Odml
                prefs->set(FEATURE_USE_ODML, use_odml);
				// Split
                prefs->set(SETTINGS_MPEGSPLIT, autosplit);
                
                // number of threads
                prefs->set(FEATURE_THREADING_LAVC, lavcThreads);
				prefs->set(FEATURE_THREADING_X264, x264Threads);
				prefs->set(FEATURE_THREADING_XVID, xvidThreads);

				// Encoding priority
				prefs->set(PRIORITY_ENCODING, encodePriority);
				// Indexing / unpacking priority
				prefs->set(PRIORITY_INDEXING, indexPriority);
				// Playback priority
				prefs->set(PRIORITY_PLAYBACK, playbackPriority);

                // Auto index mpeg
                prefs->set(FEATURE_TRYAUTOIDX, useAutoIndex);
                // Auto swap A/B
                prefs->set(FEATURE_SWAP_IF_A_GREATER_THAN_B, useSwap);

                // Disable nuv sync
                prefs->set(FEATURE_DISABLE_NUV_RESYNC, useNuv);
                // Use tray while encoding
                prefs->set(FEATURE_USE_SYSTRAY,useTray);
                // Filter directory
				prefs->set(FILTERS_AUTOLOAD_ACTIVE, activeXfilter);

                if(filterPath)
                  prefs->set(FILTERS_AUTOLOAD_PATH, filterPath);
                // Alternate mp3 tag (haali)
                prefs->set(FEATURE_ALTERNATE_MP3_TAG,alternate_mp3_tag);

			#if defined(__WIN32) && defined(USE_SDL)
				// Initialise SDL again as driver may have changed
				initSdl();
			#endif
	}

	ADM_dealloc(filterPath);
	ADM_dealloc(globalGlyphName);

	return 1;
}
extern int DIA_getMPParams( uint32_t *pplevel, uint32_t *ppstrength,uint32_t *swap);
void setpp(void)
{
        uint32_t type,strength,uv=0;

        if(!prefs->get(DEFAULT_POSTPROC_TYPE,&type)) type=3;
        if(!prefs->get(DEFAULT_POSTPROC_VALUE,&strength)) strength=3;
        if( DIA_getMPParams( &type,&strength,&uv))
        {
                prefs->set(DEFAULT_POSTPROC_TYPE,type);
                prefs->set(DEFAULT_POSTPROC_VALUE,strength);
        }
//	video_body->setPostProc(type, strength, uv);
}
//EOF 
