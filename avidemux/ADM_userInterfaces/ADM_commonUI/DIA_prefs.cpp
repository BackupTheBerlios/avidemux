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



# include <config.h>

#include "default.h"

# include "prefs.h"
#include "ADM_audiodevice/audio_out.h"

#include "ADM_assert.h"
#include "GUI_render.h"
#include "ADM_osSupport/ADM_cpuCap.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_commonUI/DIA_factory.h"


extern void 		AVDM_audioPref( void );



uint8_t DIA_Preferences(void);

/**
      \fn DIA_Preferences
      \brief Handle preference dialog
*/
uint8_t DIA_Preferences(void)
{
uint8_t ret=0;

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
uint32_t mthreads=0;
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
	olddevice=newdevice=AVDM_getCurrentDevice();

        // Default pp
         if(!prefs->get(DEFAULT_POSTPROC_TYPE,&pp_type)) pp_type=3;
         if(!prefs->get(DEFAULT_POSTPROC_VALUE,&pp_value)) pp_value=3;
#define DOME(x,y) y=!!(pp_type & x)
    
    DOME(1,hzd);
    DOME(2,vzd);
    DOME(4,dring);
     
        // Alsa
        if( prefs->get(DEVICE_AUDIO_ALSA_DEVICE, &alsaDevice) != RC_OK )
                alsaDevice = ADM_strdup("plughw:0,0");
        // autovbr
        prefs->get(FEATURE_AUTO_BUILDMAP,&autovbr);
        // autoindex
        prefs->get(FEATURE_AUTO_REBUILDINDEX,&autoindex);
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
        if(!prefs->get(FEATURE_MULTI_THREAD, &mthreads))
                mthreads=0;
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
        // Master or PCM for audio
        if(!prefs->get(FEATURE_AUDIOBAR_USES_MASTER, &useMaster))
                useMaster=0;
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
        diaElemToggle useSysTray(&useTray,_("Use systray while encoding"));
        diaElemToggle allowAnyMpeg(&mpeg_no_limit,_("Accept non-standard audio frequency for DVD"));
        diaElemToggle useLavcodec(&lavcodec_mpeg,_("Use lavcodec mpeg2 decoder"));
        diaElemToggle openDml(&use_odml,_("Create openDML file"));
        diaElemToggle autoIndex(&useAutoIndex,_("Automatically index mpeg files"));
        diaElemToggle autoSwap(&useSwap,_("Automatically swap A and B if A>B"));
        diaElemToggle nuvAudio(&useNuv,_("Disable NUV audio sync"));
        diaElemToggle loadEx(&activeXfilter,_("Load external filters"));
        
        diaElemToggle togAutoVbr(&autovbr,_("Automatically build VBR map"));
        diaElemToggle togAutoIndex(&autoindex,_("Automatically rebuild index"));
        diaElemToggle togAutoUnpack(&autounpack,_("Automatically remove packed bitstream"));
                       
        diaElemUInteger multiThread(&mthreads,_("Number of threads"),0,10);

		diaMenuEntry priorityEntries[] = {
                             {0,       _("High"),NULL}
                             ,{1,      _("Above Normal"),NULL}
                             ,{2,      _("Normal"),NULL}
							 ,{3,      _("Below Normal"),NULL}
							 ,{4,      _("Low"),NULL}
        };
		diaElemMenu menuEncodePriority(&encodePriority,_("Encoding priority"), sizeof(priorityEntries)/sizeof(diaMenuEntry), priorityEntries,"");
		diaElemMenu menuIndexPriority(&indexPriority,_("Indexing / unpacking priority"), sizeof(priorityEntries)/sizeof(diaMenuEntry), priorityEntries,"");
		diaElemMenu menuPlaybackPriority(&playbackPriority,_("Playback priority"), sizeof(priorityEntries)/sizeof(diaMenuEntry), priorityEntries,"");

        diaElemUInteger autoSplit(&autosplit,_("Split mpegs every (MB)"),10,4096);
        
        diaElemToggle   togTagMp3(&alternate_mp3_tag,_("Use alternative tag for mp3 in .mp4"));
        
        diaMenuEntry videoMode[]={
                             {RENDER_GTK,      _("Gtk (Slow)"),NULL}
#ifdef USE_XV
                             ,{RENDER_XV,   _("Xv (Best)"),NULL}
#endif
#ifdef USE_SDL
                             ,{RENDER_SDL,      _("SDL (Good)"),NULL}
#endif
        };        
        diaElemMenu menuVideoMode(&render,_("Video Display"), sizeof(videoMode)/sizeof(diaMenuEntry),videoMode,"");
        
        
        
        diaMenuEntry msgEntries[]={
                             {0,       _("No alerts"),NULL}
                             ,{1,      _("Display only error alerts"),NULL}
                             ,{2,      _("Display all alerts"),NULL}
        };
        diaElemMenu menuMessage(&msglevel,_("Message level"), sizeof(msgEntries)/sizeof(diaMenuEntry),msgEntries,"");
        
        
        
        diaMenuEntry volumeEntries[]={
                             {0,       _("PCM"),NULL}
                             ,{1,      _("Master"),NULL}};
        diaElemMenu menuVolume(&useMaster,_("Volume control"), sizeof(volumeEntries)/sizeof(diaMenuEntry),volumeEntries,"");
        
        
        
         diaMenuEntry mixerEntries[]={
                             {0,       _("No downmixing"),NULL}
                             ,{1,       _("Stereo"),NULL}
                             ,{2,      _("Prologic"),NULL}
                              ,{3,      _("Prologic II"),NULL}
         };
        diaElemMenu menuMixer(&downmix,_("Local Playback Downmixing"), sizeof(mixerEntries)/sizeof(diaMenuEntry),mixerEntries,"");
#undef MKADID        
#define MKADID(x) {DEVICE_##x,_(#x),NULL}
        diaMenuEntry audioEntries[]=
{
	
#ifdef OSS_SUPPORT	
	MKADID(OSS),
#endif	
#ifdef USE_ARTS
	MKADID(ARTS),
#endif	
#ifdef ALSA_SUPPORT
	MKADID(ALSA),
#endif	
#ifdef CONFIG_DARWIN
	MKADID(COREAUDIO),
#endif
#if	defined(USE_SDL) && !defined(CYG_MANGLING)	
	MKADID(SDL),
#endif	
#ifdef CYG_MANGLING	
	MKADID(WIN32),
#endif	
#ifdef USE_ESD	
	MKADID(ESD),
#endif	
        MKADID(DUMMY)
};
        diaElemMenu menuAudio(&newdevice,_("Audio output"), sizeof(audioEntries)/sizeof(diaMenuEntry),audioEntries,"");
        
        diaElemText entryAlsaDevice(&alsaDevice,"Alsa Device",NULL);
#ifdef ALSA_SUPPORT
          int z,m;
          m=sizeof(audioEntries)/sizeof(diaMenuEntry);
          for(z=0;z<m;z++)
          {
            if(audioEntries[z].val==DEVICE_ALSA)
                menuAudio.link(&(audioEntries[z]),1,&entryAlsaDevice);
          }
#endif
        // default Post proc
     diaElemToggle     fhzd(&hzd,_("Horizontal deblocking"));
     diaElemToggle     fvzd(&vzd,_("Vertical deblocking"));
     diaElemToggle     fdring(&dring,_("Deringing"));
     diaElemUInteger   postProcStrength(&pp_value,_("Strength:"),0,5);
     diaElemFrame      framePP(_("Default Postprocessing"));
     
     framePP.swallow(&fhzd);
     framePP.swallow(&fvzd);
     framePP.swallow(&fdring);
     framePP.swallow(&postProcStrength);
     
        // Filter path
        if( prefs->get(FILTERS_AUTOLOAD_PATH, &filterPath) != RC_OK )
#ifndef CYG_MANGLING
               filterPath = ADM_strdup("/tmp");
#else
               filterPath = ADM_strdup("c:\\");
#endif
        diaElemDirSelect  entryFilterPath(&filterPath,_("Filter directory"),"");
        
        /**********************************************************************/
        /* First Tab : user interface */
        diaElem *diaUser[]={&useSysTray,&menuMessage};
        diaElemTabs tabUser("User Interface",2,diaUser);
        
         /* First Tab bis: user interface : Auto*/
        diaElem *diaAuto[]={&autoSwap,&togAutoVbr,&togAutoIndex,&togAutoUnpack,&autoIndex,};
        diaElemTabs tabAuto("Automatically",5,diaAuto);
        
        /* Second Tab : input */
        diaElem *diaInput[]={&nuvAudio,&useLavcodec};
        diaElemTabs tabInput("Input",2,(diaElem **)diaInput);
        
        /* Third Tab : output */
        diaElem *diaOutput[]={&autoSplit,&openDml,&allowAnyMpeg,&togTagMp3};
        diaElemTabs tabOutput("Output",4,(diaElem **)diaOutput);
        
        /* Fourth Tab : audio */
        diaElem *diaAudio[]={&menuMixer,&menuVolume,&menuAudio,&entryAlsaDevice};
        diaElemTabs tabAudio("Audio",4,(diaElem **)diaAudio);
        
        /* Fifth Tab : video */
        
        diaElem *diaVideo[]={&menuVideoMode,&framePP};
        diaElemTabs tabVideo("Video",2,(diaElem **)diaVideo);
        
        /* Sixth Tab : mthread */
        diaElem *diaCpu[]={&multiThread, &menuEncodePriority, &menuIndexPriority, &menuPlaybackPriority};
        diaElemTabs tabCpu("CPU",4,(diaElem **)diaCpu);
        
        /* seventh Tab : Xfilter */
        diaElem *diaXFilter[]={&loadEx,&entryFilterPath};
        diaElemTabs tabXfilter("External Filters",2,(diaElem **)diaXFilter);
                                    
// SET
        diaElemTabs *tabs[]={&tabUser,&tabAuto,&tabInput,&tabOutput,&tabAudio,&tabVideo,&tabCpu,&tabXfilter};
        if( diaFactoryRunTabs(_("Preferences"),8,tabs))
	{
		ret=1;
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
                if(alsaDevice)
                {
                   prefs->set(DEVICE_AUDIO_ALSA_DEVICE, alsaDevice);
                   ADM_dealloc(alsaDevice);
                   alsaDevice=NULL;
                }
                // Device
                if(olddevice!=newdevice)
                {
                      AVDM_switch((AUDIO_DEVICE)newdevice);
                }
                // Downmixing (default)
                prefs->set(DOWNMIXING_PROLOGIC,downmix);
                // Master or PCM
                prefs->set(FEATURE_AUDIOBAR_USES_MASTER, useMaster);
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
                if(mthreads<2) mthreads=0;
                prefs->set(FEATURE_MULTI_THREAD, mthreads);
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
                if(filterPath)
                  prefs->set(FILTERS_AUTOLOAD_PATH, filterPath);
                // Alternate mp3 tag (haali)
                prefs->set(FEATURE_ALTERNATE_MP3_TAG,alternate_mp3_tag);
	}
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
