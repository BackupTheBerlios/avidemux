/***************************************************************************
                          gui_action.hxx  -  description
                             -------------------
    begin                : Fri Jan 18 2002
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

#ifndef __GUI_ACTION
#define __GUI_ACTION
#define ADM_MAC_CUSTOM_SCRIPT 10
typedef enum
{
  ACT_OpenAvi = 1,
  ACT_AppendAvi,
  ACT_BrokenAvi ,
  ACT_AviInfo,
  ACT_Exit,
  ACT_PlayAvi,
  ACT_StopAvi,
  ACT_SaveImg,
  ACT_SaveOGM,
  ACT_SaveJPG,
  ACT_SaveBunchJPG,
  ACT_SaveDVDPS,
  ACT_SaveUnpackedMpeg4,
  ACT_RecentFiles,

  ACT_SaveDualAudio,
  ACT_SaveAvi,
  ACT_SaveWave,
  ACT_Pipe2Lame,
  ACT_PipeLame,
  ACT_Pipe2Other,

  
 ACT_SaveMpeg1vcd,
 ACT_SaveMpeg2svcd, 
 ACT_SaveMpeg2dvd,
 
 ACT_SaveRaw,
 
 ACT_XVCD,

  
  ACT_SetPostProcessing,
  ACT_NextFrame,
  ACT_Back50Frames,
  ACT_Back25Frames,
  ACT_Forward50Frames,
  ACT_Forward25Frames,
  ACT_NextKFrame,
  ACT_PreviousKFrame,
  ACT_PreviousFrame,
  ACT_PrevBlackFrame,
  ACT_NextBlackFrame,
  ACT_AllBlackFrames,
   
  ACT_Scale,
  
  ACT_Goto,
  ACT_GotoTime,
  

  ACT_Begin,
  ACT_End,
  
  ACT_AudioSourceAvi,
  ACT_AudioSourceMP3,
  ACT_AudioSourceWAV,
  ACT_AudioSourceAC3,
  ACT_AudioSourceNone,

 

  ACT_FrameChanged,
  ACT_TimeChanged,

  ACT_MarkA,
  ACT_MarkB,
  ACT_GotoMarkA,
  ACT_GotoMarkB,



 
  ACT_AudioModeProcess,
  ACT_AudioModeCopy,
  ACT_AudioModeToggle,
  ACT_AudioFilters,

  ACT_AudioMap      ,
  
   
   
ACT_VideoModeCopy,
ACT_VideoModeProcess,
ACT_VideoParameter,
ACT_VideoModeToggle,

ACT_Copy,
ACT_Cut,
ACT_Paste,
ACT_Delete                           ,
ACT_PreviewToggle    ,

ACT_RebuildKF  ,
ACT_BitRate,
ACT_ChangeFPS  ,
ACT_SetMuxParam   ,
ACT_DecoderOption   ,

ACT_SelectEncoder,
ACT_Fast,

ACT_SecondAudioTrack,

ACT_SelectDevOSS,
ACT_SelectDevDummy,
ACT_SelectDevArts,
ACT_SelectDevALSA,

ACT_CutWizard,

ACT_MpegIndex,
ACT_VideoCheck,


ACT_AudioConfigure,

ACT_OuputToggle,

ACT_VideoConfigure,

ACT_SaveWork,
ACT_SaveCurrentWork,
ACT_ResetSegments,

ACT_SetLogFile,
ACT_SavePref,

ACT_VideoCodec,
ACT_AudioCodec,
ACT_About,

ACT_Pref,

ACT_Requant,
ACT_JumpToFrame,
ACT_RunScript,
ACT_AudioCodecChanged,
ACT_VideoCodecChanged,
ACT_Bitrate,
ACT_Ocr,
ACT_SelectTrack1,
ACT_ViewMain,
ACT_ViewSide,
ACT_TimeShift,
ACT_RECENT0,
ACT_RECENT1,
ACT_RECENT2,
ACT_RECENT3,

ACT_AUTO_VCD,
ACT_AUTO_SVCD,
ACT_AUTO_DVD,
ACT_AUTO_PSP,

ACT_ADD_JOB,
ACT_HANDLE_JOB,
ACT_V2V,

ACT_ZOOM_1_4,
ACT_ZOOM_1_2,
ACT_ZOOM_1_1,
ACT_ZOOM_2_1,
ACT_ZOOM_4_1,
ACT_BUILT_IN,
ACT_CUSTOM_BASE,
ACT_CUSTOM_END=ACT_CUSTOM_BASE+ADM_MAC_CUSTOM_SCRIPT,
ACT_DUMMY


}
Action;

#endif
