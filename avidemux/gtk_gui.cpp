/***************************************************************************
                          gtk_gui.cpp  -  description
                             -------------------
    begin                : Mon Dec 10 2001
    copyright            : (C) 2001 by mean
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
//#define TEST_MP2


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>


#include <time.h>
#include <sys/time.h>

#include "config.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "ADM_lavcodec.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/bitmap.h"
#include "subchunk.h"
#include "avilist.h"




#include "ADM_audio/aviaudio.hxx"
#include "ADM_audio/audioex.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"
#include "gtkgui.h"
//#include "ADM_gui/GUI_vars.h"
#include "ADM_gui/GUI_mux.h"
#include "oplug_mpeg/op_mpeg.h"
#include "oplug_mpegFF/oplug_vcdff.h"
#include "ADM_audiofilter/audioeng_buildfilters.h"
#include "ADM_audiofilter/audioeng_piper.h"
#include "prefs.h"
#include "ADM_encoder/adm_encConfig.h"
#include "ADM_gui2/GUI_render.h"
#include "ADM_gui2/GUI_ui.h"
#include "ADM_colorspace/colorspace.h"
#include "ADM_audiodevice/audio_out.h"

#include "ADM_dialog/DIA_busy.h"
#include "ADM_dialog/DIA_working.h"

#include "ADM_video/ADM_genvideo.hxx"
#include "ADM_filter/video_filters.h"
#include "ADM_dialog/DIA_enter.h"

#include "ADM_encoder/ADM_vidEncode.hxx"


#include "ADM_codecs/ADM_ffmpeg.h"

#include "mpeg2enc/ADM_mpeg2enc.h"



void A_handleSecondTrack (int tracktype);
void A_saveImg (char *name);
void A_saveBunchJpg( char *name);
void A_requantize(void);
void A_saveJpg (char *name);
int A_loadWave (char *name);
int A_loadAC3 (char *name);
void A_saveAudioDecodedTest (char *name);
void A_openBrokenAvi (char *name);
int A_openAvi2 (char *name, uint8_t mode);
void A_appendAvi (char *name);
extern void A_SaveAudioNVideo (char *name);
void HandleAction (Action action);
void A_rebuildKeyFrame (void);
extern int GUI_handleFilter (void);
extern void filterCleanUp (void);
int A_audioSave(char *name);
static void ReSync (void);
static void cleanUp (void);
extern uint8_t getPreviewToggleStatus (void);
extern void getPreviewToggleStatus (uint8_t y);
extern void GUI_setName (char *n);
extern void DIA_properties( void);
extern uint8_t DIA_Preferences(void);
extern void  GUI_displayBitrate( void );
uint8_t GUI_InitRender (GtkWidget * g, uint32_t w, uint32_t h);
void test_mpeg (char *name);
uint8_t GUI_getDoubleValue (double *valye, float min, float max,
			    const char *title);
extern void GUI_setMarks (uint32_t a, uint32_t b);
extern void saveMpegFile (char *name);
//static void A_selectEncoder ( void );
extern void A_SaveAudioDualAudio (void);
static void secondTrackMP3 (char *name);
extern uint8_t ADM_aviUISetMuxer(  void );
static void secondTrackAC3 (char *name);

static void updateSecondAudioTrack (void);

extern void A_Save( char *name);
static uint32_t getAudioByteCount( uint32_t start, uint32_t end);
extern void mpegToIndex (char *name);
static void A_mpegIndexer (void);
extern uint8_t indexMpeg (char *mpeg, char *file, uint8_t aid);
void ADM_cutWizard (void);
void computeIT (int size, int nb, int brate, uint32_t * frame,
		uint32_t * rsize);
void ADM_saveRaw (char *name);
static void A_saveWorkbench (char *name);
void A_loadWorkbench (char *name);
void updateLoaded (void);
extern void encoderSetLogFile (char *name);
extern void videoCodecSelect (void);
extern uint8_t DIA_about( void );
extern uint8_t DIA_RecentFiles( char **name );
extern void mpeg_passthrough(  char *name );
static void A_videoCheck( void);
extern void parseScript(char *scriptname);
#define USE_FFMPEG_AS_DEFAULT
#ifdef USE_FFMPEG
//uint8_t       use_fast_ffmpeg=1;
#ifdef USE_FFMPEG_AS_DEFAULT
uint8_t use_fast_ffmpeg = 1;
#else
uint8_t use_fast_ffmpeg = 0;

#endif

#else
uint8_t use_fast_ffmpeg = 0;
#endif

//__________




extern int ignore_change;

PARAM_MUX muxMode = MUX_REGULAR;
int muxParam = 0;

extern int audioShift;
extern int audioDelay;


//___________________________________________
// serialization of user event throught gui
//
// Independant from the toolkit used
// QT or other port should be easy
//___________________________________________

void
HandleAction (Action action)
{
  static int recursive = 0;

  uint32_t nf = 0;
  uint32_t old;
  // handle out of band actions
  // independant load not loaded
//------------------------------------------------

  switch (action)
    {
    case ACT_RunScript:
    			 GUI_FileSelRead ("Select script to load ", parseScript);
    		return;
		
    case ACT_RecentFiles:
    	char *file;
		if(		DIA_RecentFiles(&file))
		{
			A_openAvi2 (file, 0);
		}
		return;
    case ACT_About :
    		 DIA_about( );
		 return;
    case ACT_VideoCodec:
      videoCodecSelect ();
      return;
    case ACT_AudioCodec:
      audioCodecSelect ();
      return;

    case ACT_AudioFilters:
      audioFilter_configureFilters ();
      return;
    case ACT_Pref:
    	if(DIA_Preferences())
	{
	saveEncoderConfig ();
 	#ifdef HAVE_AUDIO     
      		AVDM_audioSave();
 	#endif     
      	prefs->save ();	
	}      
	return;
    case ACT_SavePref:
      saveEncoderConfig ();
 #ifdef HAVE_AUDIO     
      AVDM_audioSave();
 #endif     
      prefs->save ();
      return;
    case ACT_SetLogFile:
      GUI_FileSelWrite ("Select log File to use", encoderSetLogFile);
      return;
      break;
    case ACT_LoadWork:
      GUI_FileSelRead ("Select workbench to load ", A_loadWorkbench);

      return;
      break;

    case ACT_SetMuxParam:
      ADM_aviUISetMuxer();
      return;
      break;
    case ACT_OuputToggle:
      guiOutputDisplay ^= 1;
      return;

    case ACT_MpegIndex:
      A_mpegIndexer ();
      return;
      break;
#ifdef HAVE_AUDIO      
    	case ACT_SelectDevOSS:
				   AVDM_switch (DEVICE_OSS);
				   return;break;
    	case ACT_SelectDevDummy:
				   AVDM_switch (DEVICE_DUMMY);
				   return;break;
    	case ACT_SelectDevArts:
				   AVDM_switch (DEVICE_ARTS);
				   return;break;
	case ACT_SelectDevALSA:
				   AVDM_switch (DEVICE_ALSA);
				   return;break;
#endif                                  
    case ACT_Fast:
      use_fast_ffmpeg = 1 - use_fast_ffmpeg;
      printf (" Fast FFmpeg is now :%d\n", use_fast_ffmpeg);
      return;
      break;
    case ACT_Exit:
      { uint8_t saveprefsonexit;
         prefs->get(FEATURE_SAVEPREFSONEXIT,&saveprefsonexit);
         if( saveprefsonexit )
            prefs->save ();
      }
      cleanUp ();
      delete prefs;
      prefs=NULL;
      exit (0);
    case ACT_AudioModeToggle:
      if( !recursive )
      {
        audioProcessMode ^= 1;
        recursive = 1;
        UI_setAProcessToggleStatus(audioProcessMode);
        recursive = 0;
        printf ("\n audio is now : %d\n", audioProcessMode);
      }
      return;
    case ACT_VideoModeToggle:
      if( !recursive )
      {
        videoProcessMode ^= 1;
        recursive = 1;
        UI_setVProcessToggleStatus(videoProcessMode);
        recursive = 0;
        printf ("\n video is now : %d\n", videoProcessMode);
      }
      return;

    case ACT_AudioModeProcess:
      if( !recursive )
      {
        audioProcessMode = 1;
        recursive = 1;
        UI_setAProcessToggleStatus(audioProcessMode);
        recursive = 0;
        printf ("\n audio is now : %d\n", audioProcessMode);
      }
      return;
      break;

    case ACT_AudioModeCopy:
      if( !recursive )
      {
        audioProcessMode = 0;
        recursive = 1;
        UI_setAProcessToggleStatus(audioProcessMode);
        recursive = 0;
        printf ("\n audio is now : %d\n", audioProcessMode);
      }
      return;
      break;

    case ACT_VideoModeCopy:
      if( !recursive )
      {
        videoProcessMode = 0;
        recursive = 1;
        UI_setVProcessToggleStatus(videoProcessMode);
        recursive = 0;
        printf ("\n video is now : %d\n", videoProcessMode);
      }
      return;
      break;
    case ACT_VideoModeProcess:
      if( !recursive )
      {
        videoProcessMode = 1;
        recursive = 1;
        UI_setVProcessToggleStatus(videoProcessMode);
        recursive = 0;
        printf ("\n video is now : %d\n", videoProcessMode);
      }
      return;
      break;
/*			case ACT_SelectEncoder:
				
						A_selectEncoder();
  	  			return;
						break;*/
    case ACT_SelectEncoder:
      GUI_Alert ("Obsolete !");
      break;

    default:
      break;

    }
//------------------------------------------------                                      

  if (playing)			// only allow some action
    {
      switch (action)
	{
	case ACT_PlayAvi:

	case ACT_StopAvi:
	  A_playAvi ();
	  break;
	default:
	  return;
	}
    }
  // not playing
  // allow all actions

  // restict disabled uncoded actions
  if ((int) action > 100)
    {
      GUI_Alert ("Not coded on this version");
      return;

    }
  // allow only if avi loaded
  if (!avifileinfo)
    {
      switch (action)
	{


	case ACT_OpenAvi:
	  GUI_FileSelRead ("Select AVI file...", A_openAvi);
	  break;

	case ACT_BrokenAvi:
	  printf ("\n Opening in broken mode...\n");
	  GUI_FileSelRead ("Select AVI file...", A_openBrokenAvi);
	  break;


	default:
	  ;
	}
      return;
    }

//#define TEST_UNPACK    
  // we have an AVI loaded
  switch (action)
    {
#ifdef TEST_UNPACK
    case ACT_Requant:
    			video_body->unpackPacked();
			break;
#else    
    case ACT_Requant:
    			A_requantize();
			break;
#endif
			
    case ACT_SaveDVDPS:
    			// if we are in process mode
			if(videoProcessMode)
			{
    				oplug_mpeg_dvd_ps(NULL);
			}
			else // copy mode
			{
				printf("Using pass through\n");
				GUI_FileSelWrite ("Select Mpeg file...", mpeg_passthrough);
			}
    			break;			
    case ACT_FrameChanged:
    			printf("FrameChanged\n");
			break;
  	case ACT_TimeChanged:
			printf("TimeChanged\n");
      			break;			
    case ACT_Pipe2Lame:
		 A_Pipe(P_TOOLAME);
    	break;
 case ACT_PipeLame:
		 A_Pipe(P_LAME);
    	break;
case ACT_Pipe2Other:
		 A_Pipe(P_OTHER);
    	break;

    case ACT_SaveWork:
      GUI_FileSelWrite ("Select workbench to save ", A_saveWorkbench);
      break;

	case ACT_JumpToFrame: 
		// read value	
			printf("Jump!\n");	 
			nf=UI_readCurFrame();
			if(nf>0 && nf< avifileinfo->nb_frames)
			{
				GUI_GoToFrame(nf);
			}
			UI_JumpDone();
		break;
	
    case ACT_SaveRaw:
      GUI_FileSelWrite ("Select raw file to save ", ADM_saveRaw);
      break;
    case ACT_CutWizard:
      ADM_cutWizard ();
      break;
    case ACT_SecondAudioAC3:
	      A_handleSecondTrack (2);
      break;
    case ACT_SecondAudioMP3:
      A_handleSecondTrack (1);
      break;
    case ACT_SecondAudioNONE:
      A_handleSecondTrack (0);
      break;

    case ACT_OpenAvi:
      GUI_FileSelRead ("Select AVI file...", A_openAvi);
      break;
    case ACT_BrokenAvi:
      GUI_FileSelRead ("Select AVI file...", A_openBrokenAvi);
      break;
    case ACT_AppendAvi:
      GUI_FileSelRead ("Select AVI file to append...", A_appendAvi);
      break;
    case ACT_SaveWave:
      	{
	GUI_FileSelWrite ("Select file to save audio",(SELFILE_CB *)A_audioSave);
	
	}
      break;


    case ACT_AviInfo:
      DIA_properties ();
      break;
    case ACT_Begin:
      GUI_GoToFrame (0);

      break;
	case ACT_BitRate:
		 GUI_displayBitrate(  );
	break;

    case ACT_End:

      nf = avifileinfo->nb_frames;
      GUI_GoToFrame (nf - 1);

      break;

    case ACT_PlayAvi:
      GUI_PlayAvi ();
      break;

    case ACT_SaveDualAudio:
      //GUI_FileSelWrite ("Select AVI to save ",
      A_SaveAudioDualAudio();
      break;

    case ACT_SaveBunchJPG:
    	GUI_FileSelWrite ("Select Jpeg sequence to save ", A_saveBunchJpg);      
    	break;
    case ACT_SaveImg:
      GUI_FileSelWrite ("Select BMP to save ", A_saveImg);
      //GUI_FileSelWrite ("Select Jpg to save ", A_saveJpg);
      break;
    case ACT_SaveJPG :
    	GUI_FileSelWrite ("Select Jpeg to save ", A_saveJpg);
      	//GUI_FileSelWrite ("Select Jpg to save ", A_saveJpg);
      	break;
    
    case ACT_Scale:
      if (!ignore_change)
	{
	  ignore_change++;
	  nf = GUI_GetScale ();
	  GUI_GoToKFrame (nf);
	  ignore_change--;

	}
//        printf("\n new frame : %lu",nf);
      break;
    case ACT_PreviewToggle:

      mode_preview = UI_getPreviewToggleStatus ();

      printf ("\n Preview is now : %d", mode_preview);
      if (mode_preview)
	{

	  editorReignitPreview ();
	  video_body->activateCache();
	  editorUpdatePreview (curframe);

	}
      else
      {
	editorKillPreview ();
	//video_body->desactivateCache();
      }

      break;
    case ACT_StopAvi:
      if (playing)
	GUI_PlayAvi ();
      break;
    case ACT_SetPostProcessing:
      GUI_Alert ("Obsolete");
      break;

    case ACT_NextFrame:
      GUI_NextFrame ();
      break;
    case ACT_NextKFrame:
      GUI_NextKeyFrame ();
      break;
    case ACT_NextBlackFrame:
      GUI_NextPrevBlackFrame(1);
      break;
    case ACT_PrevBlackFrame:
      GUI_NextPrevBlackFrame(-1);
      break;

    case ACT_PreviousFrame:
      if (curframe)
	{
	  DIA_StartBusy ();
	  GUI_GoToFrame (curframe - 1);
	  DIA_StopBusy ();
	  

	}

      break;

    case ACT_Forward50Frames:
      GUI_GoToFrame (curframe + 50);
      break;

    case ACT_Forward25Frames:
      GUI_GoToFrame (curframe + 25);
      break;

    case ACT_Back50Frames:
      if (curframe >= 50)
	{
	  DIA_StartBusy ();
	  GUI_GoToFrame (curframe - 50);
	  DIA_StopBusy ();
	}
      break;

    case ACT_Back25Frames:
      if (curframe >= 25)
	{
	  DIA_StartBusy ();
	  GUI_GoToFrame (curframe - 25);
	  DIA_StopBusy ();

	}
      break;

    case ACT_PreviousKFrame:
      GUI_PreviousKeyFrame ();
      break;
    case ACT_AudioSourceAvi:
      //currentaudiostream=aviaudiostream;
      changeAudioStream (aviaudiostream, AudioAvi);
      //wavinfo= currentaudiostream->getInfo();
      break;

    case ACT_AudioSourceMP3:
      GUI_FileSelRead ("Select MP3 to load ", (SELFILE_CB *)GUI_loadMP3);
      break;
    case ACT_AudioSourceAC3:
      GUI_FileSelRead ("Select AC3 to load ", (SELFILE_CB *)A_loadAC3);
      break;
    case ACT_AudioSourceWAV:
      GUI_FileSelRead ("Select WAV to load ",(SELFILE_CB *) A_loadWave);
      break;
    case ACT_AudioSourceNone:
      //currentaudiostream=(AVDMGenericAudioStream *)NULL;
      changeAudioStream ((AVDMGenericAudioStream *) NULL, AudioNone);
      break;

  
    case ACT_MarkA:
    case ACT_MarkB:
      uint8_t swapit;
      if( prefs->get(FEATURE_SWAP_IF_A_GREATER_THAN_B, &swapit) != RC_OK )
         swapit = 1;
      if (action == ACT_MarkA)
	frameStart = curframe;
      else
	frameEnd = curframe;
      if (frameStart > frameEnd && swapit )	// auto swap
	{
	  uint32_t y;
	  y = frameStart;
	  frameStart = frameEnd;
	  frameEnd = y;
	}
      UI_setMarkers (frameStart, frameEnd);
      break;
    case ACT_GotoMarkA:
    case ACT_GotoMarkB:

      if (action == ACT_GotoMarkA)
	nf = frameStart;
      else
	nf = frameEnd;
      GUI_GoToFrame (nf);
      break;
    case ACT_Goto:
      uint32_t fn;
      fn=curframe;
      if (DIA_GetIntegerValue ((int *)&fn,0,avifileinfo->nb_frames,"Go to Frame","Enter new frame number"))
	{
	  if (fn)
	    {			// 0 probably means garbage
	      if (fn < avifileinfo->nb_frames)
		{
		  curframe = fn;
		  GUI_GoToFrame (curframe);
		}
	      else
		GUI_Alert ("Out of bounds!");
	    }
	}
      break;
//----------------------test-----------------------
    case ACT_SaveAvi:
      GUI_FileSelWrite ("Select  file to save ",A_Save); // A_SaveAudioNVideo);
      break;
//---------------------------------------------------
    case ACT_Copy:
      		if( frameEnd < frameStart ){
      		   GUI_Alert("Marker A < B: can't copy");
      		}else{
    		   video_body->copyToClipBoard (frameStart,frameEnd);
      		}
		break;
    case ACT_Paste:

      		video_body->pasteFromClipBoard(curframe);
		 old=curframe;
      		ReSync ();
	  	if (!video_body->updateVideoInfo (avifileinfo))
		{
	  		GUI_Alert ("Something bad happened (II)...");
		}
     		 rebuild_status_bar ();
      		UI_setMarkers (frameStart, frameEnd);
 		curframe=old;
        	GUI_GoToFrame (curframe);
		break;
      break;

    case ACT_VideoCheck:
    		A_videoCheck();
		break;
    case ACT_ResetSegments:
       if(avifileinfo)
	if(GUI_Question("Are you sure ?"))
	{
		video_body->resetSeg();
  		video_body->getVideoInfo (avifileinfo);
		frameEnd=avifileinfo->nb_frames-1;
      		frameStart=0;
      		rebuild_status_bar ();
      		UI_setMarkers (frameStart, frameEnd);
      		ReSync ();

	}
	break;

    case ACT_Delete:
    case ACT_Cut:
      uint32_t count;

      aviInfo info;
      assert (video_body->getVideoInfo (&info));
      count = frameEnd - frameStart;
      old=frameStart;
      if( frameEnd < frameStart ){
         GUI_Alert("Marker A < B: can't delete");
         return;
      }
      if (count >= info.nb_frames - 1)
	{
	  GUI_Alert ("You don't want to \n remove all frames !");
	  return;
	}

      video_body->dumpSeg ();
      if (!video_body->removeFrames (frameStart, frameEnd))
	{
	  GUI_Alert ("Something bad happened...");
	}
      video_body->dumpSeg ();
      //resync GUI and video
      if (!video_body->updateVideoInfo (avifileinfo))
	{
	  GUI_Alert ("Something bad happened (II)...");
	}
      if (old >= avifileinfo->nb_frames)
	{			// we removed too much
	  old = avifileinfo->nb_frames - 1;
	}


      frameEnd=avifileinfo->nb_frames-1;
      frameStart=0;
      rebuild_status_bar ();
      UI_setMarkers (frameStart, frameEnd);
      ReSync ();
      curframe=old;
      printf(" goiing back to %u\n",old);
      GUI_GoToFrame (old);
      break;

    case ACT_AudioMap:
      printf ("\n making audio timeline\n");
      if (currentaudiostream)
	{

	  currentaudiostream->buildAudioTimeLine ();

	}
      break;

    case ACT_ChangeFPS:
    	{
      	float  fps;
	 aviInfo info;
	 video_body->getVideoInfo (&info);
       	fps=info.fps1000;
	fps/=1000.;


      	if (DIA_GetFloatValue (&fps, 1., 60., "Change Frame per Second","New FPS ?"))
	{
	
	  info.fps1000 = (uint32_t) (floor (fps * 1000.+0.49));
	  video_body->updateVideoInfo (&info);
	  // update display
	  video_body->getVideoInfo (avifileinfo);
	 }
	}
      break;
      // set decoder option (post processing ...)
    case ACT_DecoderOption:
      video_body->setDecodeParam ( curframe);

      break;
    case ACT_VideoParameter:
      // first remove current viewer
      if (mode_preview)
	editorKillPreview ();
      GUI_handleFilter ();
      if (mode_preview)
	editorUpdatePreview (curframe);
      break;

    case ACT_RebuildKF:
      if (GUI_Question ("Rebuild all Keyframes?"))
	{
	  A_rebuildKeyFrame ();
	  GUI_Alert ("Save your file and restart Avidemux!");
	}
      break;

    default:
      printf ("\n unhandled action %d\n", action);
      assert (0);
      return;

    }


  // gtk_widget_grab_focus(sb_frame);
}

#ifdef USE_MJPEG


void
A_saveMpeg (char *name)
{

  oplug_mpeg_vcd (name);

}


#endif

//_____________________________________________________________
//
// Open AVI File
//    mode 0: normal
//    mode 1: Suspicious
//_____________________________________________________________

void
A_openBrokenAvi (char *name)
{
  A_openAvi2 (name, 1);
}

void
A_openAvi (char *name)
{
  A_openAvi2 (name, 0);
}
extern void GUI_PreviewEnd (void);
int A_openAvi2 (char *name, uint8_t mode)
{
  uint8_t res;

  if (playing)
    return 0;
  /// check if name exists
  FILE *fd;
  fd = fopen (name, "rb");
  if (!fd)
    return 0;
  fclose (fd);


  if (avifileinfo)		// already opened ?
    {				// delete everything
      // if preview is on
      if (mode_preview)
	GUI_PreviewEnd ();
      delete avifileinfo;
      //delete wavinfo;
      wavinfo = (WAVHeader *) NULL;
      avifileinfo = (aviInfo *) NULL;
      video_body->cleanup ();
      curframe = 0;
      currentaudiostream = NULL;
      changeAudioStream (NULL, AudioNone);
      filterCleanUp ();


      if (rdr_decomp_buffer)
	{
	  free (rdr_decomp_buffer);
	  rdr_decomp_buffer = NULL;
	}

    }
  DIA_StartBusy ();
  if (mode)
    res = video_body->addFile (name, 1);
  else
    res = video_body->addFile (name);
  DIA_StopBusy ();
  if (res!=ADM_OK)			// an error occured
    {
    	if(ADM_IGN==res) 
	{
		return 0;
	}
	
      currentaudiostream = NULL;
      avifileinfo = NULL;

      GUI_Alert ("Problem opening that file!");
      return 0;
    }
	{ char *longname = PathCanonize(name);
	  int i;
		// remember this file
		prefs->set_lastfile(longname);
		updateLoaded ();
		for(i=strlen(longname);i>=0;i--)
			if( longname[i] == '/' ){
				i++;
				break;
			}
		UI_setTitle(longname+i);
		free(longname);
	}
	return 1;
}

void  updateLoaded ()
{
  if (avifileinfo)
    {
      delete avifileinfo;
      avifileinfo = NULL;
    }
  avifileinfo = new aviInfo;
  if (!video_body->getVideoInfo (avifileinfo))
    {
//      err1:
      printf ("\n get info failed...cancelling load...\n");
      delete avifileinfo;
      avifileinfo = (aviInfo *) NULL;
      currentaudiostream = aviaudiostream = NULL;


      return;

    }

  curframe = 0;

  video_body->flushCache();
  assert (rdr_decomp_buffer =
	  (uint8_t *) malloc (3 * avifileinfo->width * avifileinfo->height));

  //frameStart = 0;
  //frameEnd = avifileinfo->nb_frames - 1;
  video_body->getMarkers(&frameStart,&frameEnd);
  UI_setMarkers (frameStart, frameEnd);

  // now get audio information if exists
  wavinfo = video_body->getAudioInfo ();	//wavinfo); // will be null if no audio
  if (!wavinfo)
    {
      printf ("\n *** NO AUDIO ***\n");
      wavinfo = (WAVHeader *) NULL;
      currentaudiostream = aviaudiostream = (AVDMGenericAudioStream *) NULL;
    }
  else
    {
      /*
      ** <JSC> Sat Feb 21 15:30:15 CET 2004
      ** problem: second edl load will raise a crash
      ** why: 1) the last run generates aviaudiostream [video_body->getAudioStream()]
      **         which is set to currentaudiostream    [changeAudioStream()]
      **      2) second call of video_body->getAudioStream() will free(aviaudiostream)
      **         which is the same as free(currentaudiostream)
      **      3) following changeAudioStream() call will use the free()'d memory
      **         [currentaudiostream->isDestroyable()]
      **      efence with EF_PROTECT_FREE=1 will raise SIGSEGV and show you
      **      currentaudiostream is not accessable
      **      warning: without efense it will crash somethere other
      **
      ** I'm not sure why aviaudiostream is global. This issue will not
      ** occure if aviaudiostream is local and be lost at end of this block.
      ** nasty workaround: only the raise condition will be fixed
      */
      if( currentaudiostream == aviaudiostream ){
         aviaudiostream = NULL; // free of memory done in changeAudioStream()
      }
      /* </JSC> */
      video_body->getAudioStream (&aviaudiostream);
      changeAudioStream (aviaudiostream, AudioAvi);
      if (aviaudiostream)
	if (!aviaudiostream->isDecompressable ())
	  {
	    GUI_Alert ("Beware: No valid audio codec found!\n"
		       "Save(A+V) will generate bad AVI\nSave audio will work");
	  }

    }

  // Init renderer
  renderResize (avifileinfo->width, avifileinfo->height);
  curframe = 0;
  rebuild_status_bar ();

  // Draw first frame
  if (!video_body->getUncompressedFrame (curframe, rdr_decomp_buffer))
    {
      GUI_Alert ("Problem decompressing frame!");
    }
  else
    {
      renderUpdateImage (rdr_decomp_buffer);
      renderRefresh ();
    }
   printf("\n** conf updated **\n");
}

//___________________________________________
//  Append an AVI to the existing one
//___________________________________________
void
A_appendAvi (char *name)
{


  if (playing)
    return;
  DIA_StartBusy ();
  if (!video_body->addFile (name))
    {
      DIA_StopBusy ();
      GUI_Alert ("Something failed ...");
      return;
    }
  DIA_StopBusy ();


  video_body->dumpSeg ();
  if (!video_body->updateVideoInfo (avifileinfo))
    {
      GUI_Alert ("Something bad happened (II)...");
    }

  ReSync ();

}

//
//      Whenever a changed happened in the the stream, resync
//  related infos including audio & video filters

void ReSync (void)
{
  uint8_t isaviaud;

  frameStart = 0;
  frameEnd = avifileinfo->nb_frames - 1;
  // update audio stream
  // If we were on avi , mark it...
  if (currentaudiostream == aviaudiostream)
    {
      isaviaud = 1;
      changeAudioStream ((AVDMGenericAudioStream *) NULL, AudioNone);

    }
  else
    isaviaud = 0;
  rebuild_status_bar ();

  // Since we modified avi stream, rebuild audio stream accordingly
  video_body->getAudioStream (&aviaudiostream);
  if (isaviaud)
    {
      changeAudioStream (aviaudiostream, AudioAvi);
    }
  	//updateVideoFilters ();
	getFirstVideoFilter();

}

//_____________________________________________________________
//    Save current stream (generally avi...)
//     in raw mode
//_____________________________________________________________
void
A_saveAudio (char *name)
{

// debug audio seek
  uint32_t len2;
  uint32_t written, max;
  DIA_working *work;
  FILE *out;

#define BITT 500
  uint8_t buffer[BITT];

  if (!currentaudiostream)
    return;
  if (!currentaudiostream->isCompressed ())
    {
      GUI_Alert
	("The audio is wav PCM. \nPlease select Audio Process mode \notherwise the audio stream will be\n unreadable (raw PCM))");
      return;
    }

  work=new DIA_working("Saving audio");
  out = fopen (name, "wb");
  assert (out);

// compute start position
  currentaudiostream->goToTime (video_body->getTime (frameStart));
// compute max bytes to write (with little margin)
  max =
    currentaudiostream->convTime2Offset (video_body->
					 getTime (frameEnd + 1)) -
    currentaudiostream->convTime2Offset (video_body->getTime (frameStart));
  written = 0;
  while (1)
    {
      len2 = currentaudiostream->read (BITT, (uint8_t *) buffer);
      fwrite (buffer, len2, 1, out);
      if (len2 != BITT)
	break;
      written += len2;
      if (written > max)
	break;			// we wrote everything !

      work->update(written, max);
      if(!work->isAlive()) break;
    };
  fclose (out);
  delete work;
  printf ("\n wanted %u bytes, written %u\n", max, written);


}



//____________________________________________________________
//
//      Play AVI  + Audio if possible
//_____________________________________________________________
void
A_playAvi (void)
{

}

#ifndef TEST_MP2
/**________________________________________________________
 Save a Jpg image from current display buffer
________________________________________________________*/
void A_saveJpg (char *name)
{
  ffmpegEncoderFFMjpeg *codec=NULL;
  uint32_t sz,fl;
  FILE *fd;
  uint8_t *buffer=NULL;


	sz = avifileinfo->width* avifileinfo->height * 3;
	buffer=new uint8_t [sz];
	assert(buffer);


		codec=new  ffmpegEncoderFFMjpeg( avifileinfo->width,avifileinfo->height,FF_MJPEG)  ;
		codec->init( 95,25000);
		if(!codec->encode(rdr_decomp_buffer,
				 	buffer,
					&sz,
					&fl))
			{
				GUI_Alert("Problem encoding that frame!");
				delete [] buffer;
				delete codec;
				return ;
			}

	fd=fopen(name,"wb");
	if(!fd)
	{
				GUI_Alert("Problem opening file!");
				delete [] buffer;
				delete codec;
				return ;

	}
	fwrite (buffer, sz, 1, fd);
    	fclose(fd);
    	delete [] buffer;
	delete codec;
  	GUI_Alert ("Done.");
}
#else

void A_saveJpg (char *name)
{
Mpeg2encSVCD *codec=NULL;
  uint32_t sz,fl,size;
  FILE *fd;
  uint8_t *buffer=NULL;
  uint8_t *buffer_out=NULL;

	fd=fopen(name,"wb");
	if(!fd)
	{
				GUI_Alert("Problem opening file!");
				return ;

	}
	sz = avifileinfo->width* avifileinfo->height * 3;
	buffer=new uint8_t [sz];
	buffer_out=new uint8_t [sz];
	assert(buffer);

		// mpeg1 hardcoded
		codec=new  Mpeg2encSVCD( avifileinfo->width,avifileinfo->height)  ;
		codec->init( 5,2100,avifileinfo->fps1000);

		for(uint32_t w=0;w<avifileinfo->nb_frames;w++)
		{
			video_body->getUncompressedFrame(w,buffer,&fl);
			codec->encode(buffer,buffer_out,&size,&fl);
			fwrite (buffer_out, size, 1, fd);
		}


	fclose(fd);
    	delete [] buffer;
	delete [] buffer_out;
	delete codec;
  	GUI_Alert ("Done.");
}
#endif
/**
	Save the selection  as a bunch of jpeg 95% qual
	Used mainly to make animated DVD menu 

*/
void A_saveBunchJpg(char *name)
{
  ffmpegEncoderFFMjpeg *codec=NULL;
  uint32_t sz,fl;
  FILE *fd;
  uint8_t *buffer=NULL;
  uint8_t *src=NULL;
  uint32_t curImg;
  char	 fullName[2048],*ext;


  	if(frameStart>frameEnd)
		{
			GUI_Alert("Set markers correctly!");
			return;
		}
	// Split name into base + extension
	PathSplit(name,&name,&ext);
	
	
	sz = avifileinfo->width* avifileinfo->height * 3;
	buffer=new uint8_t [sz];
	assert(buffer);
	src=new uint8_t [sz];
	assert(src);


		codec=new  ffmpegEncoderFFMjpeg( avifileinfo->width,avifileinfo->height,FF_MJPEG)  ;
		codec->init( 95,25000);
		
	for(curImg=frameStart;curImg<=frameEnd;curImg++)
	{		
		if (!video_body->getUncompressedFrame (curImg, src))
		{
			GUI_Alert("Cannot uncompress video!");
			goto _bunch_abort;
		}
		if(!codec->encode(src,
				 	buffer,
					&sz,
					&fl))
			{
				GUI_Alert("Problem encoding that frame!");
				goto _bunch_abort;
				
			}
		sprintf(fullName,"%s%04d.jpg",name,curImg-frameStart);
		fd=fopen(fullName,"wb");
		if(!fd)
		{
				GUI_Alert("Problem writing file!");
				goto _bunch_abort;

		}
		fwrite (buffer, sz, 1, fd);
    		fclose(fd);
	}
	GUI_Alert ("Done.");
_bunch_abort:	
    	delete [] buffer;
	delete [] src;
	delete codec;
	return ;


}
/**________________________________________________________
 Save a BMP image from current display buffer
________________________________________________________*/

void
A_saveImg (char *name)
{
  BITMAPFILEHEADER bmfh;
  BITMAPINFOHEADER bmph;
  FILE *fd;
  uint32_t sz;
  uint16_t s16;
  uint32_t s32;


sz = avifileinfo->width* avifileinfo->height * 3;


  printf ("\n Size : %X\n", bmfh.size);
  bmfh.xHotspot = bmfh.yHotspot = 0;
  bmfh.offsetToBits = sizeof (bmfh) + sizeof (bmph);
//_________________________________________
  bmph.biSize = sizeof (bmph);
  bmph.biWidth = avifileinfo->width;
  bmph.biHeight = avifileinfo->height;
  bmph.biPlanes = 1;
  bmph.biBitCount = 24;
  bmph.biCompression = 0;	// COMPRESSION NONE
  bmph.biSizeImage = sz;
  bmph.biXPelsPerMeter = 0;
  bmph.biYPelsPerMeter = 0;
  bmph.biClrUsed = 0;
  bmph.biClrImportant = 0;
/*
	bmph.resolutionUnits=0;
	bmph.origin=0;
	bmph.colorEncoding=0;
*/



  printf ("\n %u x %u=%u\n", bmph.biWidth, bmph.biHeight, sz);

  uint8_t *out;

  	out=(uint8_t *)malloc(sz);
	if(!out)
	{
		GUI_Alert("Memory error!");
		return;
	}

	 if(!COL_yv12rgbBMP(bmph.biWidth, bmph.biHeight,rdr_decomp_buffer, out))
	 {
		GUI_Alert("Error converting to BMP !");
		return;
 	}
 	fd = fopen (name, "wb");
  	if (!fd)
    	{
      	GUI_Alert ("Something bad happened.");
	free(out);
      	return;
    	}

	// Bitmpap file header, not using tructure due to gcc padding it
#ifdef ADM_BIG_ENDIAN
	s16 = 0x424D;
#else	
  	s16 = 0x4D42;
#endif	
  	s32 = 14 + sizeof (bmph) + sz;
#ifdef ADM_BIG_ENDIAN	
	#define SWAP32(x) x=R32(x)	
#else
	#define SWAP32(x) ; 
#endif
	SWAP32(s32);	
  	fwrite (&s16, 2, 1, fd);
  	fwrite (&s32, 4, 1, fd);
  	s32 = 0;
  	fwrite (&s32, 4, 1, fd);
  	s32 = 14 + sizeof (bmph);
	SWAP32(s32);
  	fwrite (&s32, 4, 1, fd);
#ifdef ADM_BIG_ENDIAN
//void Endian_BitMapInfo( BITMAPINFOHEADER *b)
	Endian_BitMapInfo(&bmph);
#endif
	fwrite (&bmph, sizeof (bmph), 1, fd);
      	fwrite (out, sz, 1, fd);

    	fclose(fd);
    	free(out);

  GUI_Alert ("Done.");

}

//_____________________________________________________________
//
//              Load AC3
//
//
//_____________________________________________________________
int
A_loadAC3 (char *name)
{
  if (!avifileinfo)
    return 0;


  AVDMAC3AudioStream *ac3 = new AVDMAC3AudioStream ();

  if (ac3->open (name) == 0)
    {
      GUI_Alert ("Failed to open this file \n (Not a WAV file?)\n");
      printf ("WAV open file failed...");
      delete ac3;
      return 0;
    }
  //currentaudiostream=wav;
  changeAudioStream (ac3, AudioAC3);
  wavinfo = currentaudiostream->getInfo ();
  return 1;
}

//_____________________________________________________________
//
//              Load wave
//              
//
//_____________________________________________________________
int
A_loadWave (char *name)
{
  if (!avifileinfo)
    return 0;
  AVDMWavAudioStream *wav = new AVDMWavAudioStream ();

  if (wav->open (name) == 0)
    {
      GUI_Alert ("Failed to open this file \n (Not a WAV?)\n");
      printf ("WAV open file failed...");
      delete wav;
      return 0;
    }
  //currentaudiostream=wav;
  changeAudioStream (wav, AudioWav);
  wavinfo = currentaudiostream->getInfo ();
  return 1;
}

//________________________________________________________
// Change audio stream and delete the old one if needed
//________________________________________________________
void
changeAudioStream (AVDMGenericAudioStream * newaudio, AudioSource nwsource)
{
  if (currentaudiostream)
    {
      if (currentaudiostream->isDestroyable ())
	delete currentaudiostream;
    }
  currentaudiostream = newaudio;
  GUI_UpdateAudioToggle (nwsource);
  if (currentaudiostream)
    wavinfo = currentaudiostream->getInfo ();

}

//_____________________________________________________________
//    Save current stream (generally avi...)
//     in decoded mode (assuming MP3)
//_____________________________________________________________
void
A_saveAudioDecodedTest (char *name)
{

// debug audio seek
  uint32_t len2, gauge = 0;
  uint32_t written = 0, max;
  FILE *out;
  AVDMGenericAudioStream *saveFilter;
#undef BITT
#define BITT 4*1152
#define OUTCHUNK 1024*1024
  uint8_t *outbuffer;

  if (!currentaudiostream)
    return;
  if (currentaudiostream->isCompressed ())
    if (!currentaudiostream->isDecompressable ())
      {
	GUI_Alert ("Cannot decompress audio frame!");
	return;
      }


  if (!(out = fopen (name, "wb")))
    {
      GUI_Alert ("Cannot write file");
      return;
    }

  outbuffer = (uint8_t *) malloc (2 * OUTCHUNK);	// 1Meg cache;
  if (!outbuffer)
    {
      GUI_Alert ("Memory Error!");
      return;
    }
// compute max bytes to read (with little margin)
  max = getAudioByteCount( frameStart, frameEnd);

  printf ("\n input : %u bytes\n", max);


// re-ignite first filter...

  currentaudiostream->beginDecompress ();


  // Write Wav header

  /* Sat Nov 09 06:11:52 CET 2002 Fixes from Maik Broemme <mbroemme@plusserver.de> */
  /* If you set negative delay and save the audio stream, the saved stream was shorter than the video stream. */

  /* Example: video stream is 10 minutes long, audio stream perhaps 20 minutes, you need the audio stream from */
  /*          minute 1 until 11, so you setup an audio delay from -60 seconds, but this 60 seconds were removed */
  /*          from begin and end of the audio stream. That was not good :) Now it runs correctly also if you use */
  /*          audio stream with same length then video, therefore is premature ending :) */

  uint32_t delay = 0;
  if (audioDelay > 0)
    {
      saveFilter =
	buildAudioFilter (currentaudiostream,
			  video_body->getTime (frameStart), max);
    }
  else
    {
      delay = currentaudiostream->convTime2Offset (-audioDelay);
      saveFilter =
	buildAudioFilter (currentaudiostream,
			  video_body->getTime (frameStart), max + delay);
    }

    DIA_working *work=new DIA_working("Saving audio");


//
//  Create First filter that is null filter
//
  saveFilter->writeHeader (out);
  printf (".. Starting ..\n");
  if (audioDelay > 0)
    {
      max = saveFilter->getLength ();
    }
  else
    {
      max = saveFilter->getLength () - delay;
    }
  printf ("\n Computed length : %u\n", max);
  while (1)
    {
      len2 = saveFilter->readDecompress (BITT, outbuffer + gauge);
      //      printf("Got : %lu\n",len2);
      gauge += len2;
      // update GUI                   
	if (work->update (written, max))	// abort request ?
	    goto abortSaveAudio;
      if ((len2 == 0) || (gauge > OUTCHUNK))	// either out buffer is full
	// or we read nothing -> end of file
	{
	  fwrite (outbuffer, 1, gauge, out);
	  //      fflush(out);    
	  written += gauge;
	  gauge = 0;
	  if (len2 == 0)
	    break;		// premature ending
	 
	  //printf("\n written %lu/%lu max\n",written,max);
	}
      //      if(written>max) break; // we wrote everything !
    };
abortSaveAudio:
  saveFilter->endWrite (out, written);
  fclose (out);
  free (outbuffer);
  delete work;
  deleteAudioFilter ();
  currentaudiostream->endDecompress ();
  printf (" actually written %u\n", written);


}




//      Clean up
//      free all pending stuff, make leakchecker happy
//
void
cleanUp (void)
{
  if (avifileinfo)
  {
    delete avifileinfo;
    avifileinfo=NULL;
   }
  if (currentaudiostream)
    {
      // always destroyable on exit           if(currentaudiostream->isDestroyable())
      delete currentaudiostream;
      currentaudiostream=NULL;
    }
  if (video_body)
  {
    delete video_body;
    video_body=NULL;
   }
  //
  filterCleanUp ();


}
extern uint8_t selecEncoder (uint8_t * codec);


/*void A_selectEncoder ( void )
{
		
       selecEncoder( &codec_in_use );
} */
/*
  		0-> None
    	1->	MP3
     	2-> AC3

*/
void
A_handleSecondTrack (int tracktype)
{
  /* lock which one is selected in GUI */



  switch (tracktype)
    {

    case 0:
      if (secondaudiostream)
	{
	  delete secondaudiostream;
	  secondaudiostream = NULL;
	  printf ("\n second audio stream destroyed\n");
	}
      updateSecondAudioTrack ();

      break;
    case 1:			// MP3
      if (secondaudiostream && secondaudiostream_isac3 == 0)
	return;
      if (secondaudiostream)
	{
	  delete secondaudiostream;
	  secondaudiostream = NULL;
	  printf ("\n second audio stream destroyed\n");
	}

      secondaudiostream_isac3 = 0;
      GUI_FileSelRead ("Select MP3", secondTrackMP3);
      break;

    case 2:			// AC3
      if (secondaudiostream && secondaudiostream_isac3 == 1)
	return;

      if (secondaudiostream)
	{
	  delete secondaudiostream;
	  secondaudiostream = NULL;
	  printf ("\n second audio stream destroyed\n");
	}

      secondaudiostream_isac3 = 1;
      GUI_FileSelRead ("Select AC3", secondTrackAC3);
      break;



    }

}


void
secondTrackAC3 (char *name)
{
  AVDMAC3AudioStream *tmp;

  tmp = new AVDMAC3AudioStream ();
  if (!tmp->open (name))
    {
      delete tmp;
      printf ("\n Cancelled AC3 load\n");
    }
  else
    {
      secondaudiostream = tmp;
      secondaudiostream_isac3 = 1;
      printf ("\n AC3 loaded\n");
    }
  updateSecondAudioTrack ();
}


void
secondTrackMP3 (char *name)
{
  AVDMMP3AudioStream *tmp;

  tmp = new AVDMMP3AudioStream ();
  if (!tmp->open (name))
    {
      delete tmp;
      printf ("\n Cancelled MP3 load\n");
    }
  else
    {
      secondaudiostream = tmp;
      secondaudiostream_isac3 = 0;
      printf ("\n MP3 loaded\n");
    }
  updateSecondAudioTrack ();

}

void
updateSecondAudioTrack (void)
{
  return;
#if 0
//#define GOGLE(x,y) gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (x), y)
#define GOGLE(x,y) {gtk_check_menu_item_set_show_toggle (GTK_CHECK_MENU_ITEM (x), y);}


  if (!secondaudiostream)
    {
      GOGLE (second_track_none, TRUE);
      GOGLE (second_track_ac3, FALSE);
      GOGLE (second_track_mp3, FALSE);

    }
  else
    {
      GOGLE (second_track_none, FALSE);

      if (secondaudiostream_isac3)
	{
	  GOGLE (second_track_ac3, TRUE);
	  GOGLE (second_track_mp3, FALSE);

	}
      else
	{
	  GOGLE (second_track_ac3, FALSE);
	  GOGLE (second_track_mp3, TRUE);
	}
    }
#endif

#warning fixme
}

extern void getCutPoints (int br);
void
ADM_cutWizard (void)
{

  int br;
  if (currentaudiostream)
    {
      br = currentaudiostream->getInfo ()->byterate * 8 / 1000;
    }
  else
    br = 128;

  getCutPoints (br);

}

void
computeIT (int size, int nb, int brate, uint32_t * frame, uint32_t * rsize)
{
  uint32_t curframe, cursize;
  uint32_t lastframe, lastsize;
  uint32_t lsize = size;
  curframe = cursize = lastframe = lastsize = 0;
  uint32_t one_audio;

//               uint8_t tmp[768*768*3];
//               uint32_t len2;
  uint32_t len, flag;

  uint8_t ret;

  lastframe = frameStart;
//
  double fi = brate >> 3;
  fi = fi / avifileinfo->fps1000;
  fi *= 1000000.;
  one_audio = (uint32_t) floor (fi);

  printf ("\n search for %u\n", lsize);
  printf ("\n one audio  %u\n", one_audio);
  for (uint32_t f = frameStart; f < avifileinfo->nb_frames; f++)
    {

      cursize += 32 + 32 + 2 * 32 * nb;	// index + header
      cursize += one_audio * nb;
      len = 0;
      ret = video_body->getFrameSize (f, &len);
      video_body->getFlags (f, &flag);
      if (!ret)
	{
	  printf ("\n Error!\n");
	  *frame = lastframe;
	  *rsize = lastsize;
	  return;
	}
      cursize += len;
      // check if we went beyonf
      if (cursize > lsize)
	{
	  *frame = lastframe;
	  *rsize = lastsize;
	  printf ("\n found : Frame %u  %u\n", lastframe, lastsize);
	  return;
	}
      // if it is a key f we memorize it
      if (flag & AVI_KEY_FRAME)
	{
	  lastframe = f;
	  lastsize = size;
	}
    }


  printf ("\n oops\n");
  *rsize = 0;
  *frame = 0;
}
extern int DIA_mpegIndexer (char **mpegFile, char **indexFile, int *aid,
			    int already = 0);

//________________________________
void
A_mpegIndexer (void)
{
  char *mpeg, *file;
  int id;
  if (DIA_mpegIndexer (&mpeg, &file, &id))
    {
      if ((mpeg == NULL) || (file == NULL))
	{
	  GUI_Alert ("Please select at least one file to process!");
	  return;
	}
      printf ("\n indexing :%s to \n%s\n (audio %x)", mpeg, file,id);
      if (indexMpeg (mpeg, file, (uint8_t) id))
	GUI_Alert ("Index done");

    }
  else
    {
      printf ("\n cancelled\n");
    }
}
/*
	Save a raw video stream without any container
	Usefull to cut mpeg stream or extract raw h263/mpeg4 stream

*/
void ADM_saveRaw (char *name)
{
  uint32_t len, flags;
  FILE *fd, *fi;
  uint8_t *buffer = new uint8_t[avifileinfo->width * avifileinfo->height * 3];
  char *idx;
  DIA_working *work;

  idx = new char[strlen (name) + 8];
  strcpy (idx, name);
  strcat (idx, ".idx");
  fd = fopen (name, "wb");
  fi = fopen (idx, "wt");
  if (!fd)
    return;
  work=new DIA_working("Saving raw video stream");

  // preamble
  video_body->getRawStart (frameStart, buffer, &len);
  fwrite (buffer, len, 1, fd);

  for (uint32_t i = frameStart; i < frameEnd; i++)
    {
      work->update (i - frameStart, frameEnd - frameStart);
      if(!work->isAlive()) goto _abt;
      assert (video_body->getFlags (i, &flags));
      if (flags & AVI_B_FRAME)	// oops
	{
	  // se search for the next i /p
	  uint32_t found = 0;

	  for (uint32_t j = i + 1; j < frameEnd; j++)
	    {
	      assert (video_body->getFlags (j, &flags));
	      if (!(flags & AVI_B_FRAME))
		{
		  found = j;
		  break;
		}

	    }
	  if (!found)
	    goto _abt;
	  // Write the found frame
	  video_body->getRaw (found, buffer, &len);
	  fwrite (buffer, len, 1, fd);
	  // and the B frames
	  for (uint32_t j = i; j < found; j++)
	    {
	      video_body->getRaw (j, buffer, &len);
	      fwrite (buffer, len, 1, fd);
	    }
	  i = found;		// Will be plussed by for
	}
      else			// P or I frame
	{
	  video_body->getRaw (i, buffer, &len);
	  fwrite (buffer, len, 1, fd);
	  fprintf (fi, "%u,\n", len);
	}

    }
_abt:
  fclose (fd);
  fclose (fi);
  delete work;

}

void
A_saveWorkbench (char *name)
{
  video_body->saveWorbench (name);
}

void A_loadWorkbench (char *name)
{
  /// check if name exists
  FILE *fd;
  fd = fopen (name, "rb");
  if (!fd)
    return;
  fclose (fd);

 if( video_body->loadWorbench (name))
   {
    updateLoaded ();
      // remember this file
     prefs->set_lastfile(name);
    }
}
//---------------------
extern int DIA_audioEncoder(int *pmode, int *pbitrate,const char *title);
/**---------------------------------------------------
		Pipe to toolame
-------------------------------------------------------**/
extern  void audioCodecSetcodec(AUDIOENCODER codec);
extern uint8_t DIA_lame(char **lame);
extern uint8_t DIA_pipe(char **cmd, char **param);

extern uint8_t audioPiperLame(char *file, AVDMProcessAudioStream *in,char *extra);
extern uint8_t audioPipeTo(char *file, AVDMProcessAudioStream *in,char *cmd, char *param);


void A_Pipe(pipID who,char *outfile)
{
  AVDMProcessAudioStream *in;
  uint32_t max=0;
  char *param=NULL;
  char *cmd=NULL;

  if (!currentaudiostream)
    return;
  if (currentaudiostream->isCompressed ())
    if (!currentaudiostream->isDecompressable ())
      {
	GUI_Alert ("Cannot decompress audio frame!");
	return;
      }

      switch(who)
	{
		case P_TOOLAME:
    				// ask audio to set up parameters

				 audioCodecSetcodec(AUDIOENC_MP2);
				// configure it ..
				if(!isQuiet())
					audioCodecConfigure();
				break;
		case P_LAME:
				if(!DIA_lame(&param)) return;
				break;

		case P_OTHER:
				if(!DIA_pipe(&cmd,&param)) return;
				break;
	}


	// ask for filename
	char *file=NULL;
		if(!outfile)
		{
			if(who!=P_OTHER)
				GUI_FileSelWrite("Output file for piped", &file);
		}
		else
		{
			file=strdup(outfile);
		}

	if(!file) return;


// compute max bytes to read (with little margin)
  max =getAudioByteCount( frameStart,frameEnd);
// re-ignite first filter...
  currentaudiostream->beginDecompress ();
  in =buildInternalAudioFilter (currentaudiostream, video_body->getTime (frameStart), max);

  printf("\n***%u bytes to feed \n",max);
	// now we have the incoming stream and the filename
	// ask piper to do it
	switch(who)
	{
		case P_TOOLAME:
    				audioPiper2Lame(file, in);
				break;
		case P_LAME:
				audioPiperLame(file,in,param);
				break;

		case P_OTHER:
				audioPipeTo(file,in,cmd,param);
				break;
	}
    deleteAudioFilter();
    if(param) free(param);
    if(cmd) free(cmd);
    if(file) free(file);
    GUI_Alert("Done!");

}

/**
	Return the # of byte to go from start frame to end frame

*/
uint32_t getAudioByteCount( uint32_t start, uint32_t end)
{
uint32_t max=0;
// compute max bytes to read (with little margin)
  max = video_body->getTime (end + 1) - video_body->getTime (start);
  //convert time in ms to bytes

  double db;
  db = max;
  // 01/10/2002 Fix for 5.1->stereo conversion
  if (wavinfo->channels > 2)
    db *= 2;
  else
    db *= wavinfo->channels;
  db *= wavinfo->frequency;
  db /= 500;			// 1000 because ms, 2 bytes / sample
  max = (uint32_t) floor (db);

  return max;
}
/*
	Unpack all frames without displaying them to check for error

*/
void A_videoCheck( void)
{
uint32_t nb=0;
uint32_t buf[720*576*2];
uint32_t error=0;
DIA_working *work;

	nb = avifileinfo->nb_frames;
	work=new DIA_working("Checking video");

  for(uint32_t i=0;i<nb;i++)
  {
	work->update(i, nb);
      	if(!work->isAlive()) break;
	if(!video_body->getUncompressedFrame (i, (uint8_t *)buf))
	{
		error ++;
		printf("Frame %u has error\n",i);
	}

    };
  delete work;
  if(error==0)
  	GUI_Alert("No error found !");
else
	{
		char str[400];
		sprintf(str,"There was %u / %u frames with error",error,nb);
		GUI_Alert(str);

	}
	GUI_GoToFrame(curframe);

}
//___________ save audio _____________
int A_audioSave(char *name)
{
	if (!currentaudiostream)	// yes it is checked 2 times so what ?
	return 0;
	if (audioProcessMode)
	{
		if (currentaudiostream->isCompressed ())
			if (!currentaudiostream->isDecompressable ())
		  	{
		    		GUI_Alert
		      ("Cannot decompress the requested audio stream\nPlease switch to Audio Copy mode");
		   		return 0;
		  	}
		// if we get here, either not compressed
		// or decompressable
		A_saveAudioDecodedTest(name);
	    }
	else			// copy mode...
	    {
	       A_saveAudio(name);
	    }
	return 1;
}
// EOF

