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
 // This is used to test new features
 // should be commented out for regulat build
//#define TEST_MP2

#ifdef TEST_MP2
	#warning TEST_MP2 is ON
	#warning TEST_MP2 is ON
	#warning TEST_MP2 is ON
	#warning TEST_MP2 is ON
	#warning TEST_MP2 is ON
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>



#include <time.h>
#include <sys/time.h>
#include <fcntl.h>	/* O_RDONLY */
#include <errno.h>

#include "config.h"

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include "ADM_lavcodec.h"

#include "fourcc.h"
#include "avi_vars.h"
#include "ADM_assert.h"
#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit_gtk.h"
#include "prototype.h"
#include "ADM_toolkit/toolkit.hxx"
#include "ADM_toolkit/bitmap.h"
//#include "subchunk.h"
//#include "avilist.h"




#include "ADM_audio/aviaudio.hxx"
#include "ADM_audio/audioex.h"
#include "ADM_audiofilter/audioprocess.hxx"
#include "gui_action.hxx"
#include "gtkgui.h"
//#include "ADM_gui/GUI_vars.h"
#include "oplug_avi/GUI_mux.h"
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

#include "ADM_filter/video_filters.h"
#include "ADM_assert.h"
void A_handleSecondTrack (int tracktype);
int A_delete(uint32_t start, uint32_t end);
void A_saveImg (char *name);
void A_saveBunchJpg( char *name);
void A_requantize(void);
int A_saveJpg (char *name);
int A_loadWave (char *name);
int A_loadAC3 (char *name);
int A_loadMP3 (char *name);
int A_loadNone( void );
void A_saveAudioDecodedTest (char *name);
void A_openBrokenAvi (char *name);
int A_openAvi2 (char *name, uint8_t mode);
int A_appendAvi (char *name);
void A_externalAudioTrack( void );
extern void A_SaveAudioNVideo (char *name);
void HandleAction (Action action);
uint8_t A_rebuildKeyFrame (void);
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
extern void A_SaveAudioDualAudio (char *a);

extern uint8_t ADM_aviUISetMuxer(  void );


static void updateSecondAudioTrack (void);
void A_audioTrack(void);
extern int A_Save( char *name);
static uint32_t getAudioByteCount( uint32_t start, uint32_t end);
extern void mpegToIndex (char *name);
static void A_mpegIndexer (void);
extern uint8_t indexMpeg (char *mpeg, char *file, uint8_t aid);
void ADM_cutWizard (void);
void computeIT (int size, int nb, int brate, uint32_t * frame,
		uint32_t * rsize);
int ADM_saveRaw (char *name);
static char * actual_workbench_file;
void A_saveWorkbench (char *name);
void updateLoaded (void);
extern void encoderSetLogFile (char *name);
extern void videoCodecSelect (void);
extern uint8_t DIA_about( void );
extern uint8_t DIA_RecentFiles( char **name );
extern void mpeg_passthrough(  char *name,ADM_OUT_FORMAT format );
static void A_videoCheck( void);
extern uint8_t parseScript(char *scriptname);
int A_saveDVDPS(char *name);
static void	A_setPostproc( void );
extern uint8_t ogmSave(char  *name);
//
static uint8_t A_pass(char *name);
uint8_t A_jumpToTime(uint32_t hh,uint32_t mm,uint32_t ss);
//__________
extern uint8_t DIA_gotoTime(uint16_t *hh, uint16_t *mm, uint16_t *ss);
extern uint8_t ogmSave(char *fd);
extern uint8_t GUI_getFrame(uint32_t frameno, ADMImage *image, uint32_t *flags);
extern int A_SaveUnpackedVop( char *name);
extern void      videoCodecConfigureUI(void);
extern void audioCodecChanged(int newcodec);
extern void videoCodecChanged(int newcodec);
extern void DIA_Calculator(uint32_t *sizeInMeg, uint32_t *avgBitrate );
extern uint8_t A_autoDrive(Action action);
extern int ignore_change;

extern uint8_t ADM_ocr_engine( void);
uint8_t A_TimeShift(void);
PARAM_MUX muxMode = MUX_REGULAR;
int muxParam = 0;


extern bool parseECMAScript(const char *name);
//___________________________________________
// serialization of user event throught gui
//
// Independant from the toolkit used
// QT or other port should be easy
//___________________________________________

void HandleAction (Action action)
{
  static int recursive = 0;

  uint32_t nf = 0;
  uint32_t old;
  // handle out of band actions
  // independant load not loaded
//------------------------------------------------
int nw;
  switch (action)
    {
        case ACT_RECENT0:
        case ACT_RECENT1:        
        case ACT_RECENT2:
        case ACT_RECENT3:
                        const char **name;
                        int rank;
                                name=prefs->get_lastfiles();
                                rank=(int)action-ACT_RECENT0;
                                ADM_assert(name[rank]);
                                A_openAvi2 ((char *)name[rank], 0);
                                
                                
                return;
        case ACT_ViewMain: UI_toogleMain();return;
        case ACT_ViewSide: UI_toogleSide();return;
      case ACT_Ocr:
                ADM_ocr_engine( );
                return;
      case ACT_AudioConfigure:
    		audioCodecSelect();
		return;
	case ACT_VideoConfigure:
    		videoCodecSelect();
		return;
    case ACT_VideoCodecChanged:
    		nw=UI_getCurrentVCodec();
    		videoCodecChanged(nw);
		return;
   case ACT_AudioCodecChanged:
                nw=UI_getCurrentACodec();
   		audioCodecChanged(nw);
    		
		return;
    
    case ACT_RunScript:
    			 GUI_FileSelRead ("Select ecmascript to run ",(SELFILE_CB *) parseECMAScript);
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
      //videoCodecSelect ();
      videoCodecConfigureUI();
      return;
    case ACT_AudioCodec:
      //audioCodecSelect ();
      audioCodecConfigure();
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
//      GUI_FileSelWrite ("Select log File to use", encoderSetLogFile);
	GUI_Alert("Obsolete!");
      return;
      break;
    case ACT_SetMuxParam:
      ADM_aviUISetMuxer();
      return;
      break;
    case ACT_OuputToggle:
        guiOutputDisplay ^= 1;
        UI_setOutputToggleStatus (guiOutputDisplay);
        printf ("\n Ouput is now : %d", guiOutputDisplay);
      return;

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
      ADM_assert(0);
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
#if 0
    case ACT_AudioModeToggle:
      if( !recursive )
      {
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
#endif
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
  if ((int) action >= ACT_DUMMY)
    {
      GUI_Alert ("Not coded on this version");
      return;

    }
  // allow only if avi loaded
  if (!avifileinfo)
    {
      switch (action)
	{
        case ACT_TimeShift:
                A_TimeShift();
                break;
	case ACT_OpenAvi:
          GUI_FileSelRead ("Select AVI file...", (SELFILE_CB *)A_openAvi);
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
        case ACT_AUTO_VCD:
        case ACT_AUTO_SVCD:
        case ACT_AUTO_DVD:
                A_autoDrive( action);
                break;
     case ACT_TimeShift:
                A_TimeShift();
                break;

   case ACT_SelectTrack1:
                A_audioTrack();
                break;

    case ACT_Bitrate:
    			{
				uint32_t a,b;
				DIA_Calculator(&a,&b );
			}
    			break;
    case ACT_SaveUnpackedMpeg4:
    			if(GUI_Question("This is to be used to undo packed vop on mpeg4.\nContinue ?"))
			{ 
				GUI_FileSelWrite ("Select Avi file to write", (SELFILE_CB *)A_SaveUnpackedVop);
				
			}
    			break;
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
    			A_saveDVDPS(NULL);
    			break;
    case ACT_SaveOGM:
    			 GUI_FileSelWrite ("Select OGM file to write", (SELFILE_CB *)ogmSave);
    			break;
				
    case ACT_FrameChanged:
    			printf("FrameChanged\n");
			break;
  	case ACT_TimeChanged:
			printf("TimeChanged\n");
      			break;			
#if 0
    case ACT_Pipe2Lame:
		 A_Pipe(P_TOOLAME);
    	break;
 case ACT_PipeLame:
		 A_Pipe(P_LAME);
    	break;
case ACT_Pipe2Other:
		 A_Pipe(P_OTHER);
    	break;
#endif
    case ACT_SaveWork:
      GUI_FileSelWrite ("Select workbench to save ", A_saveWorkbench);
      break;
    case ACT_SaveCurrentWork:
      if( actual_workbench_file ){
        char *tmp = ADM_strdup(actual_workbench_file);
         A_saveWorkbench( tmp ); // will write "actual_workbench_file" itself
         ADM_dealloc(tmp);
      }else{
         GUI_FileSelWrite ("Select workbench to save ", A_saveWorkbench);
      }
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
    case ACT_GotoTime:
                {
                        uint16_t mm,hh,ss,ms;
                             frame2time(curframe,avifileinfo->fps1000,&hh,&mm,&ss,&ms);
                             if(DIA_gotoTime(&hh,&mm,&ss))
                                {
                                        A_jumpToTime(hh,mm,ss);
                                }
                }
                break;
    case ACT_SaveRaw:
      GUI_FileSelWrite ("Select raw file to save ", (SELFILE_CB *)ADM_saveRaw);
      break;
    case ACT_CutWizard:
      ADM_cutWizard ();
      break;
    case ACT_SecondAudioTrack:
                A_externalAudioTrack();
                break;

    case ACT_OpenAvi:
      GUI_FileSelRead ("Select AVI file...",(SELFILE_CB *) A_openAvi);
      break;
    case ACT_BrokenAvi:
      GUI_FileSelRead ("Select AVI file...", A_openBrokenAvi);
      break;
    case ACT_AppendAvi:
      GUI_FileSelRead ("Select AVI file to append...",(SELFILE_CB *) A_appendAvi);
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
      A_SaveAudioDualAudio(NULL);
      break;

    case ACT_SaveBunchJPG:
    	GUI_FileSelWrite ("Select Jpeg sequence to save ", A_saveBunchJpg);      
    	break;
    case ACT_SaveImg:
      GUI_FileSelWrite ("Select BMP to save ", A_saveImg);
      //GUI_FileSelWrite ("Select Jpg to save ", A_saveJpg);
      break;
    case ACT_SaveJPG :
    	GUI_FileSelWrite ("Select Jpeg to save ", (SELFILE_CB *)A_saveJpg);
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
      mode_preview^=1;
      UI_setPreviewToggleStatus (mode_preview);

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
      A_setPostproc();
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
    case ACT_AllBlackFrames:    
       GUI_FileSelWrite ("Select file to save", (SELFILE_CB *)A_ListAllBlackFrames);
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
      A_changeAudioStream (aviaudiostream, AudioAvi,NULL);
      //wavinfo= currentaudiostream->getInfo();
      break;

    case ACT_AudioSourceMP3:
      GUI_FileSelRead ("Select MP3 to load ", (SELFILE_CB *)A_loadMP3);
      break;
    case ACT_AudioSourceAC3:
      GUI_FileSelRead ("Select AC3 to load ", (SELFILE_CB *)A_loadAC3);
      break;
    case ACT_AudioSourceWAV:
      GUI_FileSelRead ("Select WAV to load ",(SELFILE_CB *) A_loadWave);
      break;
    case ACT_AudioSourceNone:
      //currentaudiostream=(AVDMGenericAudioStream *)NULL;
       A_changeAudioStream((AVDMGenericAudioStream *) NULL, AudioNone,NULL);
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
      GUI_FileSelWrite ("Select  file to save ",(SELFILE_CB *)A_Save); // A_SaveAudioNVideo);
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

		// forget last project file
		if( actual_workbench_file ){
			ADM_dealloc(actual_workbench_file);
			actual_workbench_file = NULL;
		}
	}
	break;

    case ACT_Delete:
    case ACT_Cut:
    
     
      old=frameStart;
      if( A_delete(frameStart,frameEnd))
      {
      	if (old >= avifileinfo->nb_frames)
	{			// we removed too much
	  old = avifileinfo->nb_frames - 1;
	}
      	curframe=old;	 
      	GUI_GoToFrame (old);      
      }
      
      
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
        {
                
	        editorKillPreview ();
        }
      GUI_handleFilter ();
      if (mode_preview)
      {
        editorReignitPreview();
	editorUpdatePreview (curframe);
      }
      break;

    case ACT_RebuildKF:
      if (GUI_Question ("Rebuild all Keyframes?"))
	{
	  A_rebuildKeyFrame ();
	  //GUI_Alert ("Save your file and restart Avidemux!");
	}
      break;

    default:
      printf ("\n unhandled action %d\n", action);
      ADM_assert (0);
      return;

    }


  // gtk_widget_grab_focus(sb_frame);
}



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

int
A_openAvi (char *name)
{
  return A_openAvi2 (name, 0);
}
extern void GUI_PreviewEnd (void);
int A_openAvi2 (char *name, uint8_t mode)
{
  uint8_t res;
  char str[512];
  char *longname;

  if (playing)
    return 0;
  /// check if name exists
  FILE *fd;
  fd = fopen (name, "rb");
  if (!fd){
    if( errno == EACCES ){
       snprintf(str,512,"can't open \"%s\": permission problem.\n",name);
       GUI_Alert(str);
    }
    if( errno == ENOENT ){
       snprintf(str,512,"can't open \"%s\": file does not exist.\n",name);
       GUI_Alert(str);
    }
    return 0;
  }
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
      A_changeAudioStream (NULL, AudioNone,NULL);
      filterCleanUp ();


      if (rdr_decomp_buffer)
	{
	  delete  rdr_decomp_buffer;
	  rdr_decomp_buffer = NULL;
	}

    }
  DIA_StartBusy ();
  /*
  ** we may get a relative path by cmdline
  */
  longname = PathCanonize(name);
  if (mode)
    res = video_body->addFile (longname, 1);
  else
    res = video_body->addFile (longname);
  DIA_StopBusy ();

  // forget last project file
  if( actual_workbench_file ){
     ADM_dealloc(actual_workbench_file);
     actual_workbench_file = NULL;
  }

  if (res!=ADM_OK)			// an error occured
    {
	ADM_dealloc(longname);
    	if(ADM_IGN==res) 
	{
		return 0;
	}
	
      currentaudiostream = NULL;
      avifileinfo = NULL;

      GUI_Alert ("Problem opening that file!");
      return 0;
    }

    { int fd,i;
      char magic[4];

	/* check myself it is a project file (transparent detected and read
        ** by video_body->addFile (name);
	*/
	if( (fd = open(longname,O_RDONLY)) != -1 ){
		if( read(fd,magic,4) == 4 ){
			/* remember a workbench file */
			if( !strncmp(magic,"ADMW",4) ){
				actual_workbench_file = ADM_strdup(longname);
			}
		}
		close(fd);
	}

	/* remember any video or workbench file to "recent" */
	prefs->set_lastfile(longname);
        UI_updateRecentMenu();
	updateLoaded ();
	for(i=strlen(longname);i>=0;i--)
#ifdef CYG_MANGLING
		if( longname[i] == '\\' ){
#else
		if( longname[i] == '/' ){
#endif
			i++;
			break;
		}
	UI_setTitle(longname+i);
    }
	ADM_dealloc(longname);
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
  getFirstVideoFilter(); // reinit first filter
  video_body->flushCache();
  ADM_assert (rdr_decomp_buffer =new ADMImage(avifileinfo->width,avifileinfo->height));

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
      A_changeAudioStream (aviaudiostream, AudioAvi,NULL);
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
  

  // Draw first frame
  rebuild_status_bar (); 
  getFirstVideoFilter(); // Rebuild filter if needed
  if(!GUI_getFrame( curframe, rdr_decomp_buffer,NULL))
  {
      GUI_Alert ("Problem decompressing frame!");
    }
  else
    {
      renderUpdateImage (rdr_decomp_buffer->data);
      renderRefresh ();
      update_status_bar(rdr_decomp_buffer);
    }
   
   printf("\n** conf updated **\n");
}

//___________________________________________
//  Append an AVI to the existing one
//___________________________________________
int
A_appendAvi (char *name)
{


  if (playing)
    return 0;
  DIA_StartBusy ();
  if (!video_body->addFile (name))
    {
      DIA_StopBusy ();
      GUI_Alert ("Something failed ...");
      return 0;
    }
  DIA_StopBusy ();


  video_body->dumpSeg ();
  if (!video_body->updateVideoInfo (avifileinfo))
    {
      GUI_Alert ("Something bad happened (II)...");
      return 0;
    }

  ReSync ();
  return 1;
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
      A_changeAudioStream ((AVDMGenericAudioStream *) NULL, AudioNone,NULL);

    }
  else
    isaviaud = 0;
  rebuild_status_bar ();

  // Since we modified avi stream, rebuild audio stream accordingly
  video_body->getAudioStream (&aviaudiostream);
  if (isaviaud)
    {
      A_changeAudioStream (aviaudiostream, AudioAvi,NULL);
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

#define ONE_STRIKE (64*1024)
  uint8_t *buffer=NULL;

  if (!currentaudiostream)
    return;
  if (!currentaudiostream->isCompressed ())
    {
      GUI_Alert
	("The audio is wav PCM. \nPlease select Audio Process mode \n"
	"otherwise the audio stream will be\n unreadable (raw PCM))");
      return;
    }

  
  out = fopen (name, "wb");
  if (!out) return;
  
  work=new DIA_working("Saving audio");
  
  uint32_t timeEnd,timeStart,sample,hold,len;
  uint64_t tgt_sample,cur_sample;
  double   duration;

  // compute start position and duration in samples
    
   timeStart=video_body->getTime (frameStart);
   timeEnd=video_body->getTime (frameEnd+1);

   currentaudiostream->goToTime (timeStart);
   duration=timeEnd-timeStart;
   printf("Duration:%f ms\n",duration);
   if(duration<0) duration=-duration;
  
   duration/=1000;   
   duration*=currentaudiostream->getInfo()->frequency;
   
   tgt_sample=(uint64_t)floor(duration);  
   
   cur_sample=0;
   written = 0;
   hold=0;
   buffer=new uint8_t[ONE_STRIKE*2];
   while (1)
    {
    	if(!currentaudiostream->getPacket(buffer+hold,&len,&sample)) break;
	hold+=len;
	written+=len;
	cur_sample+=sample;
	if(hold>ONE_STRIKE)
	{
		fwrite(buffer,hold,1,out);
		hold=0;		
	}
	if(cur_sample>tgt_sample)
		break;   
      work->update(cur_sample>>10, tgt_sample>>10);
      if(!work->isAlive()) break;
    };
  if(hold)
  {
  	fwrite(buffer,hold,1,out);
	hold=0;  
  }
  
  fclose (out);
  delete work;
  delete buffer;
  printf ("\n wanted %llu samples, goto %llu samples, written %u bytes\n", tgt_sample,cur_sample, written);


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
int A_saveJpg (char *name)
{
  ffmpegEncoderFFMjpeg *codec=NULL;
  uint32_t sz,fl;
  FILE *fd;
  uint8_t *buffer=NULL;


	sz = avifileinfo->width* avifileinfo->height * 3;
	buffer=new uint8_t [sz];
	ADM_assert(buffer);


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
				return 0;
			}

	fd=fopen(name,"wb");
	if(!fd)
	{
				GUI_Alert("Problem opening file!");
				delete [] buffer;
				delete codec;
				return 0;

	}
	fwrite (buffer, sz, 1, fd);
    	fclose(fd);
    	delete [] buffer;
	delete codec;
  	GUI_Info_HIG ("Done", "Saved \"%s\".", GetFileName(name));
	return 1;
}
#else

int A_saveJpg (char *name)
{
static int b=1;
         video_body->changeAudioStream(0,b);
        b^=1;
        return 1;
 	
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
  ADMImage *src=NULL;
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
	ADM_assert(buffer);
	//src=new uint8_t [sz];
	src=new ADMImage(avifileinfo->width,avifileinfo->height);
	ADM_assert(src);


		codec=new  ffmpegEncoderFFMjpeg( avifileinfo->width,avifileinfo->height,FF_MJPEG)  ;
		codec->init( 95,25000);
		
	for(curImg=frameStart;curImg<=frameEnd;curImg++)
	{		
		if (!GUI_getFrame (curImg, src,NULL))
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
	GUI_Info_HIG("Done", "Saved %d images.", curImg-frameStart);
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

  	out=(uint8_t *)ADM_alloc(sz);
	if(!out)
	{
		GUI_Alert("Memory error!");
		return;
	}

	 if(!COL_yv12rgbBMP(bmph.biWidth, bmph.biHeight,rdr_decomp_buffer->data, out))
	 {
		GUI_Alert("Error converting to BMP !");
		return;
 	}
 	fd = fopen (name, "wb");
  	if (!fd)
    	{
      	GUI_Alert ("Something bad happened.");
	ADM_dealloc(out);
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
    	ADM_dealloc(out);

  GUI_Info_HIG ("Done", "Saved \"%s\".", GetFileName(name));

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
  A_changeAudioStream (ac3, AudioAC3,name);
  wavinfo = currentaudiostream->getInfo ();
  return 1;
}
int A_loadNone( void )
{
 	A_changeAudioStream ((AVDMGenericAudioStream *) NULL, AudioNone,NULL);
}
//_____________________________________________________________
//
//              Load wave
//              
//
//_____________________________________________________________
AudioSource currentAudioSource=AudioAvi;
AudioSource secondAudioSource=AudioNone;
char *currentAudioName=NULL;
//_____________________________________________________________
//
//              Load MP3 and identify wavfmt infos to fill avi header
//              -> use mad ?
//
//_____________________________________________________________
int A_loadMP3(char *name)
{
    if (!avifileinfo)
        return 0;
    AVDMMP3AudioStream *mp3 = new AVDMMP3AudioStream();

    if (mp3->open(name) == 0)
      {
          printf("MP3 open file failed...");
          delete mp3;
          return 0;
      }
    //currentaudiostream=mp3;
    A_changeAudioStream(mp3, AudioMP3,name);
    wavinfo = currentaudiostream->getInfo();
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
  {
        printf("No video loaded\n");
        return 0;
   }
  AVDMWavAudioStream *wav = new AVDMWavAudioStream ();

  if (wav->open (name) == 0)
    {
      GUI_Alert ("Failed to open this file \n (Not a WAV?)\n");
      printf ("WAV open file failed...");
      delete wav;
      return 0;
    }
  //currentaudiostream=wav;
  A_changeAudioStream (wav, AudioWav,name);
  wavinfo = currentaudiostream->getInfo ();
  return 1;
}
AudioSource getCurrentAudioSource(char **name)
{
        *name=currentAudioName;
        return currentAudioSource;
}
//________________________________________________________
// Change audio stream and delete the old one if needed
//________________________________________________________
uint8_t A_changeAudioStream (AVDMGenericAudioStream * newaudio, AudioSource nwsource,char *myname)
{
  if (currentaudiostream)
    {
      if (currentaudiostream->isDestroyable ())
	delete currentaudiostream;
      currentAudioSource=AudioNone;
      if(currentAudioName) ADM_dealloc(currentAudioName);
      currentAudioName=NULL;
    }
  currentaudiostream = newaudio;
//  GUI_UpdateAudioToggle (nwsource);
  if (currentaudiostream)
  {
    wavinfo = currentaudiostream->getInfo ();
    currentAudioSource=nwsource;
    if(myname)
        currentAudioName=ADM_strdup(myname);
  }
  return 1;
}

//_____________________________________________________________
//    Save current stream (generally avi...)
//     in decoded mode (assuming MP3)
//_____________________________________________________________
void
A_saveAudioDecodedTest (char *name)
{

// debug audio seek
  uint32_t len, gauge = 0;
  uint32_t written = 0;
  FILE *out;
  AVDMGenericAudioStream *saveFilter;
  
  uint64_t sampleTarget,sampleCurrent;
  
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

  outbuffer = (uint8_t *) ADM_alloc (2 * OUTCHUNK);	// 1Meg cache;
  if (!outbuffer)
    {
      GUI_Alert ("Memory Error!");
      return;
    }



// re-ignite first filter...

  currentaudiostream->beginDecompress ();


  // Write Wav header

  /* Sat Nov 09 06:11:52 CET 2002 Fixes from Maik Broemme <mbroemme@plusserver.de> */
  /* If you set negative delay and save the audio stream, the saved stream was shorter than the video stream. */

  /* Example: video stream is 10 minutes long, audio stream perhaps 20 minutes, you need the audio stream from */
  /*          minute 1 until 11, so you setup an audio delay from -60 seconds, but this 60 seconds were removed */
  /*          from begin and end of the audio stream. That was not good :) Now it runs correctly also if you use */
  /*          audio stream with same length then video, therefore is premature ending :) */

 
	saveFilter =	buildAudioFilter (currentaudiostream,
			  video_body->getTime (frameStart), 0xffffffff);
   
    	DIA_working *work=new DIA_working("Saving audio");


//
//  Create First filter that is null filter
//
  saveFilter->writeHeader (out);
  uint32_t tstart,tend,samples;
  double duration;
  tstart=video_body->getTime(frameStart);
  tend=video_body->getTime(frameEnd+1);
  duration=(tend-tstart);
  duration*=saveFilter->getInfo()->frequency;
  duration/=1000.;
  
  sampleTarget=(uint64_t)floor(duration);
  sampleCurrent=0;
  gauge=0;
  
  while ((sampleCurrent<sampleTarget))
    {
      if(!saveFilter->getPacket(outbuffer + gauge,&len,&samples))
      {
        printf("Audio save:Read error\n");
      	break;
      }
      //      printf("Got : %lu\n",len2);
      gauge += len;
      sampleCurrent+=samples;
      // update GUI                   
	if (work->update (sampleCurrent>>10, sampleTarget>>10))	// abort request ?
	    break;;
      if (gauge > OUTCHUNK)	// either out buffer is full	
	{
	  fwrite (outbuffer, 1, gauge, out);	  
	  written += gauge;
	  gauge = 0;
	}   	
    };
// Clean up
	if(gauge)
	{
		fwrite (outbuffer,  gauge,1, out);	  
		written += gauge;
		gauge = 0;	
	}
  saveFilter->endWrite (out, written);
  fclose (out);
  ADM_dealloc (outbuffer);
  delete work;
  deleteAudioFilter ();
  currentaudiostream->endDecompress ();
  printf ("AudioSave: actually written %u\n", written);
  printf ("Audiosave: target sample:%llu, got :%llu\n",sampleTarget,sampleCurrent);


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




#warning fixme


extern void getCutPoints (int br);
void
ADM_cutWizard (void)
{
#if 0
  int br;
  if (currentaudiostream)
    {
      br = currentaudiostream->getInfo ()->byterate * 8 / 1000;
    }
  else
    br = 128;

  getCutPoints (br);
#endif
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

/*
	Save a raw video stream without any container
	Usefull to cut mpeg stream or extract raw h263/mpeg4 stream

*/
int ADM_saveRaw (char *name)
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
    return 0;
  work=new DIA_working("Saving raw video stream");

  // preamble
#if 0
  video_body->getRawStart (frameStart, buffer, &len);
  fwrite (buffer, len, 1, fd);
#endif
  for (uint32_t i = frameStart; i < frameEnd; i++)
    {
      work->update (i - frameStart, frameEnd - frameStart);
      if(!work->isAlive()) goto _abt;
      ADM_assert (video_body->getFlags (i, &flags));
      if (flags & AVI_B_FRAME)	// oops
	{
	  // se search for the next i /p
	  uint32_t found = 0;

	  for (uint32_t j = i + 1; j < frameEnd; j++)
	    {
	      ADM_assert (video_body->getFlags (j, &flags));
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
  return 1;

}

void
A_saveWorkbench (char *name)
{
#if 0
  video_body->saveWorbench (name);
#else
  video_body->saveAsScript(name);
#endif
  if( actual_workbench_file )
     ADM_dealloc(actual_workbench_file);
  actual_workbench_file = ADM_strdup(name);
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

#if 0
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
			file=ADM_strdup(outfile);
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
    if(param) ADM_dealloc(param);
    if(cmd) ADM_dealloc(cmd);
    if(file) ADM_dealloc(file);
    GUI_Info("Done!");

}
#endif
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
//uint32_t buf[720*576*2];
uint32_t error=0;
ADMImage *aImage;
DIA_working *work;

	nb = avifileinfo->nb_frames;
	work=new DIA_working("Checking video");
	aImage=new ADMImage(avifileinfo->width,avifileinfo->height);
  for(uint32_t i=0;i<nb;i++)
  {
	work->update(i, nb);
      	if(!work->isAlive()) break;
	if(!GUI_getFrame (i, aImage,NULL))
	{
		error ++;
		printf("Frame %u has error\n",i);
	}

    };
  delete work;
  delete aImage;
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
	if (audioProcessMode())
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
//_____________________________
int A_saveDVDPS(char *name)
{
// if we are in process mode
			if(videoProcessMode())
			{
    				oplug_mpeg_dvd_ps(name);
			}
			else // copy mode
			{
			// do some sanitu check first
				uint32_t fatal;
				uint32_t end;
				end=frameEnd;
				if(end==avifileinfo->nb_frames-1) end++;
				video_body->sanityCheckRef(frameStart,end,&fatal);
				if(fatal)
				{
					GUI_Alert("There is a lonely Bframe at start/end\nPlease remove it");
					return 0;
				}
				printf("Using pass through\n");
				if(!name)
					GUI_FileSelWrite ("Select Mpeg file...", (SELFILE_CB *)A_pass);
				else
					mpeg_passthrough(name,ADM_PS);
			}

	return 1;
}
uint8_t A_pass(char *name)
{
        mpeg_passthrough(name,ADM_PS);
        return 1;
}
int A_delete(uint32_t start, uint32_t end)
{
uint32_t count;

      aviInfo info;
      ADM_assert (video_body->getVideoInfo (&info));
      count = end - start;
     
      if( end < start ){
         GUI_Alert("Marker A < B: can't delete");
         return 0;
      }
      if (count >= info.nb_frames - 1)
	{
	  GUI_Error_HIG ("You can't remove all frames", NULL);
	  return 0;
	}

      video_body->dumpSeg ();
      if (!video_body->removeFrames (start, end))
	{
	  GUI_Alert ("Something bad happened...");
	  return 0;
	}
      video_body->dumpSeg ();
      //resync GUI and video
      if (!video_body->updateVideoInfo (avifileinfo))
	{
	  GUI_Alert ("Something bad happened (II)...");
	}
      


      frameEnd=avifileinfo->nb_frames-1;
      frameStart=0;
      rebuild_status_bar ();
      UI_setMarkers (frameStart, frameEnd);
      ReSync ();
     return 1;
      


}
extern int DIA_getMPParams( uint32_t *pplevel, uint32_t *ppstrength,uint32_t *swap);
//
uint8_t A_jumpToTime(uint32_t hh,uint32_t mm,uint32_t ss)
{
uint32_t frame;
        time2frame(&frame,avifileinfo->fps1000,hh,mm,ss,0);
        if(frame>=avifileinfo->nb_frames)
        {
                printf("Frame is out of bound\n");
                return 0;
        }
        return GUI_GoToFrame(frame);
        
}

//
void	A_setPostproc( void )
{
uint32_t type,strength,swap;
	if(!avifileinfo) return;
	
	video_body->getPostProc(&type,&strength,&swap);

 	if(DIA_getMPParams( &type, &strength,&swap))
 	{
		video_body->setPostProc(type,strength,swap);
 	}

}
extern uint8_t DIA_audioTrack(AudioSource *source, uint32_t *track,uint32_t nbTrack, uint32_t *infos);
void A_audioTrack( void )
{
        uint32_t nb,*infos;
        AudioSource old,nw;        
        uint8_t r=0;
        uint32_t oldtrack,newtrack;


        if(!video_body->getAudioStreamsInfo(0,&nb, &infos))
        {
                GUI_Alert("Get tracks info failed");
                return ;
        }
        newtrack=oldtrack=video_body->getCurrentAudioStreamNumber(0);
        nw=old=currentAudioSource;
        r=DIA_audioTrack(&nw, &newtrack,nb, infos);
        delete [] infos;
        // Change track here
        if(old==nw && (nw!=AudioAvi)) return;

        switch( nw)
        {
                case AudioMP3:
                        GUI_FileSelRead ("Select MP3 to load ", (SELFILE_CB *)A_loadMP3);
                        break;
                case AudioAC3:
                        GUI_FileSelRead ("Select AC3 to load ", (SELFILE_CB *)A_loadAC3);
                        break;
                case AudioWav:
                        GUI_FileSelRead ("Select WAV to load ",(SELFILE_CB *) A_loadWave);
                        break;
                case AudioNone:
                          A_changeAudioStream((AVDMGenericAudioStream *) NULL, AudioNone,NULL);
                        break;
                case AudioAvi:
                        //printf("New :%d old:%d\n",newtrack,oldtrack);
                        if(oldtrack!=newtrack)
                        {
                                video_body->changeAudioStream(0,newtrack);
                                //
                                if(aviaudiostream==currentaudiostream)
                                        currentaudiostream=NULL;
                                delete aviaudiostream;
                                aviaudiostream=NULL;
                                video_body->getAudioStream(&aviaudiostream);
                                A_changeAudioStream (aviaudiostream, AudioAvi,NULL);
                        }
                        break;
                default:
                        ADM_assert(0);
        }
}
void A_externalAudioTrack( void )
{
        uint32_t nb=0,*infos=NULL;
        AudioSource old,nw;        
        uint8_t r=0;
        uint32_t oldtrack,newtrack;

//secondaudiostream


        nw=secondAudioSource;
        r=DIA_audioTrack(&nw, NULL,0, NULL);
        if(!r) return;

        if(secondAudioSource!=AudioNone)
        {
                 delete secondaudiostream;
                 secondAudioSource=AudioNone;
                 secondaudiostream=NULL;
        }
        switch( nw)
        {
                case AudioNone:break;
                case AudioMP3:
                        {
                        AVDMMP3AudioStream *tmp;
                        char               *name=NULL;
                        GUI_FileSelRead("MP3 to use as 2nd track",&name);
                        if(!name) break;

                        tmp = new AVDMMP3AudioStream ();
                        if (!tmp->open (name))
                        {
                                delete tmp;
                                GUI_Alert("Error loading that mp3");
                        }
                        else
                        {
                                secondaudiostream = tmp;     
                                secondAudioSource=AudioMP3;
                                printf ("\n MP3 loaded\n");
                                GUI_Info_HIG("Second track loaded", NULL);
                        }
                        }
                        break;
                case AudioAC3:
                          {
                        AVDMAC3AudioStream *tmp;
                        char               *name=NULL;
                        GUI_FileSelRead("AC3 to use as 2nd track",&name);
                        if(!name) break;

                        tmp = new AVDMAC3AudioStream ();
                        if (!tmp->open (name))
                        {
                                delete tmp;
                                GUI_Alert("Error loading that ac3");
                        }
                        else
                        {
                                secondaudiostream = tmp;     
                                secondAudioSource=AudioAC3;
                                printf ("\n AC3 loaded\n");
                                GUI_Info_HIG("Second track loaded", NULL);
                        }
                        }
                        break;
                case AudioWav:
                         {
                        AVDMWavAudioStream *tmp;
                        char               *name=NULL;
                        GUI_FileSelRead("Wav to use as 2nd track",&name);
                        if(!name) break;

                        tmp = new AVDMWavAudioStream ();
                        if (!tmp->open (name))
                        {
                                delete tmp;
                                GUI_Alert("Error loading that Wav");
                        }
                        else
                        {
                                secondaudiostream = tmp;     
                                secondAudioSource=AudioAC3;
                                printf ("\n AC3 loaded\n");
                                GUI_Info_HIG("Second track loaded", NULL);
                        }}
                        break;
                default:
                ADM_assert(0);
        }
}


             

//****************
uint8_t A_TimeShift(void)
{
static int update=0;
// extern int audioShift;
// extern int audioDelay;
int onoff;
int value;
        if(update) return 1;
        update=1;

        // Read and update
        update=0;
        UI_getTimeShift(&onoff,&value);
        if(onoff && value) audioFilterDelay(value);
                else audioFilterDelay(0);
        //****

        update=0;

}
// EOF

