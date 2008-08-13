
/***************************************************************************
    \file  guiplay.cpp
	\brief Playback loop
    
    copyright            : (C) 2001/2008 by mean
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
 
#include <math.h>
#include "prefs.h"
#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_assert.h" 

#include "DIA_fileSel.h"
#include "prototype.h"
#include "ADM_audiodevice/audio_out.h"

#include "DIA_coreToolkit.h"
#include "ADM_audio/aviaudio.hxx"
#include "ADM_audiofilter/audioprocess.hxx"
#include "ADM_videoFilter.h"
#include "ADM_videoFilter_internal.h"
#include "gtkgui.h"
#include "ADM_userInterfaces/ADM_render/GUI_render.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_libraries/ADM_utilities/avidemutils.h"
#include "ADM_preview.h"
//___________________________________
#define AUDIO_PRELOAD 150
//___________________________________

static void resetTime(void);
static void ADM_playPreloadAudio(void);
static void ADM_playFillAudio(void);
extern void UI_purge(void);
#define EVEN(x) (x&0xffffffe)

//___________________________________
uint8_t stop_req;
static uint32_t vids = 0, auds = 0, dauds = 0;
static int32_t delta;

static uint16_t audio_available = 0;
static uint32_t one_audio_frame = 0;
static uint32_t one_frame;
static float *wavbuf = NULL;
AUDMAudioFilter *playback = NULL;
extern renderZoom currentZoom;
static Clock    ticktock;

/**
    \fn         GUI_PlayAvi
    \brief      MainLoop for internal movie playback

*/
void GUI_PlayAvi(void)
{
    
    uint32_t framelen,flags;
    AVDMGenericVideoStream *filter;
    uint32_t max,err;
   
    // check we got everything...
    if (!avifileinfo)	return;
    if (!avifileinfo->fps1000)        return;
    if((curframe+1)>= avifileinfo->nb_frames-1)
    {
      printf("No frame left\n");
      return;
    }
    
    if (playing)
      {
        stop_req = 1;
        return;
      }

	uint32_t priorityLevel;

	originalPriority = getpriority(PRIO_PROCESS, 0);
	prefs->get(PRIORITY_PLAYBACK,&priorityLevel);
	setpriority(PRIO_PROCESS, 0, ADM_getNiceValue(priorityLevel));

  uint32_t played_frame=0;
  uint32_t remaining=avifileinfo->nb_frames-curframe;

    
    if(getPreviewMode()==ADM_PREVIEW_OUTPUT)
    {
            filter=getLastVideoFilter(curframe,remaining);
    }
    else
    {
            filter=getFirstVideoFilter(curframe,remaining );
    }
    
    stop_req = 0;
    playing = 1;


    ADM_playPreloadAudio();


    admPreview::deferDisplay(1,curframe);
    admPreview::update(played_frame);
    uint64_t firstPts,lastPts;
    
    uint32_t movieTime;
    uint32_t systemTime;
    firstPts=admPreview::getCurrentPts();
    ticktock.reset();
    do
    {
        vids++;
        admPreview::displayNow(played_frame);;
        update_status_bar();
      
        if((played_frame)>=(max-1))
        {
            printf("\nEnd met (%lu  / %lu )\n",played_frame,max);
            goto abort_play;
         }
        
        admPreview::update(played_frame+1);;
        curframe++;
        played_frame++;
        ADM_playFillAudio();
        lastPts=admPreview::getCurrentPts();
        systemTime = ticktock.getElapsedMS();
        movieTime=(uint32_t)((lastPts-firstPts)/1000);
        printf("[Playback] systemTime: %lu movieTime : %lu  \r",systemTime,movieTime);
        if(systemTime>movieTime) // We are late, the current PTS is after current closk
        {

        }
	    else
	    {
                delta=movieTime-systemTime;                
                // a call to whatever sleep function will last at leat 10 ms
                // give some time to GTK                		
                if (delta > 10)
                    GUI_Sleep(delta - 10);
                UI_purge();
                if(getPreviewMode()==ADM_PREVIEW_SEPARATE )
                {
                  UI_purge();
                  UI_purge(); 
                }
        }
      }
    while (!stop_req);

abort_play:
	// ___________________________________
    // Flush buffer   
    // go back to normal display mode
    //____________________________________
    playing = 0;
          
	   getFirstVideoFilter( );

       admPreview::deferDisplay(0,0);
       UI_purge();
       // Updated by expose ? 
       admPreview::update(curframe);
       UI_purge();
       update_status_bar();

    if (currentaudiostream)
      {
          if (wavbuf)
              ADM_dealloc(wavbuf);
          wavbuf=NULL;
          AVDM_AudioClose();
      }
    // done.
	setpriority(PRIO_PROCESS, 0, originalPriority);
};

/**
    \fn ADM_playFillAudio
    \brief send ~ worth of one video frame of audio
*/

void ADM_playFillAudio(void)
{
    uint32_t oaf = 0;
    uint32_t load = 0;
	uint8_t channels;
	uint32_t fq;

    if (!audio_available)	    return;
    if (!currentaudiostream)	return;			// audio ?

    one_audio_frame=256*2*2; // why not ?
    channels= playback->getInfo()->channels;
    fq=playback->getInfo()->frequency;  
	  double db_vid, db_clock, db_wav;

	  db_vid = vids;
	  db_vid *= 1000.;
      db_vid /= avifileinfo->fps1000;  // In second

     do
      {
      db_clock = ticktock.getElapsedMS();
      db_clock /= 1000;  // in seconds

      db_wav = dauds;	// for ms
      db_wav /= fq;

	  delta = (long int) floor(1000. * (db_wav - db_vid));
      // if delta grows, it means we are pumping
      // too much audio (audio will come too early)
      // if delta is small, it means we are late on audio
      if (delta < AUDIO_PRELOAD)
      {
          AUD_Status status;
             if (! (oaf = playback->fill(2*one_audio_frame,  (wavbuf + load),&status)))
             {
                  printf("[Playback] Error reading audio stream...\n");
                  return;
             }
            dauds += oaf/channels;
            load += oaf;
      }
    }
    while (delta < AUDIO_PRELOAD);
    AVDM_AudioPlay(wavbuf, load);
}

/**
    \fn ADM_playPreloadAudio
    \brief Preload audio
*/
void ADM_playPreloadAudio(void)

{
    uint32_t state,latency, one_sec;
    uint32_t small_;
    uint32_t channels;

    wavbuf = 0;

    if (!currentaudiostream)	  return;
    
    double db;
    uint64_t startPts=video_body->getTime(curframe);

    playback = buildPlaybackFilter(currentaudiostream,startPts/1000, 0xffffffff);
    
    channels= playback->getInfo()->channels;
    one_audio_frame = (one_frame * wavinfo->frequency * channels);	// 1000 *nb audio bytes per ms
    one_audio_frame /= 1000; // In elemtary info (float)
    printf("1 audio frame = %lu bytes\n", one_audio_frame);
    // 3 sec buffer..               
    wavbuf =  (float *)  ADM_alloc((3 *  2*channels * wavinfo->frequency*wavinfo->channels));
    ADM_assert(wavbuf);
    // Call it twice to be sure it is properly setup
     state = AVDM_AudioSetup(playback->getInfo()->frequency,  channels );
     AVDM_AudioClose();
     state = AVDM_AudioSetup(playback->getInfo()->frequency,  channels );
     latency=AVDM_GetLayencyMs();
     printf("[Playback] Latency : %d ms\n",latency);
      if (!state)
      {
          GUI_Error_HIG(QT_TR_NOOP("Trouble initializing audio device"), NULL);
          return;
      }
    // compute preload                      
    //_________________
    // we preload 1/4 a second

     one_sec = (wavinfo->frequency *  channels)  >> 2;
     one_sec+=(latency*wavinfo->frequency *  channels*2)/1000;
     AUD_Status status;
    uint32_t fill=0;
    while(fill<one_sec)
    {
      if (!(small_ = playback->fill(one_sec-fill, wavbuf,&status)))
      {
        break;
      }
    fill+=small_;
    }
    dauds += fill/channels;  // In sample
    AVDM_AudioPlay(wavbuf, fill);
    // Let audio latency sets in...
    ADM_usleep(latency*1000);
    audio_available = 1;
}

// EOF
