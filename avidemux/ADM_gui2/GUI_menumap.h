
/**
	Defines the binding between menu entries and core
	Since we use a common callback we don't use the
	glade generated one


*/
CALLBACK( open_video1         			,ACT_OpenAvi); 
CALLBACK( append_video1     			,ACT_AppendAvi);
CALLBACK( save_audio1         			,ACT_SaveWave);
CALLBACK( save_as_avi1       			,ACT_SaveAvi);
CALLBACK(save_as_dual_audio_avi1 		,ACT_SaveDualAudio);
CALLBACK(avi_muxer_options1 			,ACT_SetMuxParam);
CALLBACK(save_raw_video_stream1			,ACT_SaveRaw);
CALLBACK(save_as_dvd_ps1			,ACT_SaveDVDPS);
CALLBACK(unpacked				,ACT_SaveUnpackedMpeg4);

CALLBACK(load_project         			,ACT_OpenAvi); 
CALLBACK(save_project_as1              		,ACT_SaveWork);
CALLBACK(save_current_project                	,ACT_SaveCurrentWork);
 
 
CALLBACK(to_toolame1 				,ACT_Pipe2Lame);
CALLBACK(to_lame1 				,ACT_PipeLame);
CALLBACK(to_other1 				,ACT_Pipe2Other);
#if 0
 CALLBACK(video_cd1                		,ACT_SaveMpeg1vcd);
 CALLBACK(svcd1                          		,ACT_SaveMpeg2svcd);
 CALLBACK(dvd1                                       ,ACT_SaveMpeg2dvd);
 CALLBACK(xvcd                                       ,ACT_XVCD);
 #endif
 CALLBACK(video_informations1	                 ,ACT_AviInfo);
 CALLBACK(save_image1               		 ,ACT_SaveImg);
 CALLBACK(save_jpg_image1              		 ,ACT_SaveJPG);
 CALLBACK(save_selection_as_jpegs1		  ,ACT_SaveBunchJPG);
 CALLBACK(save_as_ogm1				  ,ACT_SaveOGM);

 CALLBACK( play_video1         			,ACT_PlayAvi);
 CALLBACK( decoder_options1         		,ACT_DecoderOption);
 CALLBACK( set_postprocessing1			,ACT_SetPostProcessing);
 CALLBACK( next_frame1            		,ACT_NextFrame);
 CALLBACK( previous_frame1            		,ACT_PreviousFrame);
 CALLBACK( next_intra_frame1            	,ACT_NextKFrame);
 CALLBACK( previous_intra_frame1         	,ACT_PreviousKFrame);
 CALLBACK( jum_to_frame1               		,ACT_Goto);

 CALLBACK( copy1      				,ACT_Copy);
 CALLBACK( paste1      				,ACT_Paste);
 CALLBACK( delete1      			,ACT_Delete);
 CALLBACK( cut1					,ACT_Cut);
 CALLBACK( set_marker_a1      			,ACT_MarkA);
 CALLBACK( set_marker_b1      			,ACT_MarkB);
 CALLBACK( go_to_marker_a1  			,ACT_GotoMarkA);
 CALLBACK( go_to_marker_b1 			,ACT_GotoMarkB);

 CALLBACK( search_previous_black_frame1  			,ACT_PrevBlackFrame);
 CALLBACK( search_next_black_frame1 			,ACT_NextBlackFrame);

 
CALLBACK(reset_edits1                		,ACT_ResetSegments);


CALLBACK(current_video1             	   	,ACT_AudioSourceAvi);
CALLBACK(external_mpeg                		,ACT_AudioSourceMP3);
CALLBACK(external_ac1                		,ACT_AudioSourceAC3);
CALLBACK(none1                			,ACT_AudioSourceNone);
CALLBACK(external_wav1                		,ACT_AudioSourceWAV);
CALLBACK(build_vbr_time_map1      		,ACT_AudioMap);


 //CALLBACK(index_mpeg1                		,ACT_MpegIndex);
 //CALLBACK(cut_wizard1                		,ACT_CutWizard);
// CALLBACK(select_log_file1           		,ACT_SetLogFile);

CALLBACK(videoencoder				,ACT_VideoConfigure);
CALLBACK(audio_encoder1           		,ACT_AudioConfigure);

CALLBACK(bitrate_histogram1		      	,ACT_BitRate);

 CALLBACK(rebuild_frames           		,ACT_RebuildKF);
 CALLBACK(change_fps           			,ACT_ChangeFPS);
 CALLBACK(about1	           			,ACT_About);

CALLBACK(mp3_file1				,ACT_SecondAudioMP3);
CALLBACK(ac3_file1					,ACT_SecondAudioAC3);

 //CALLBACK(save_preference1   		,ACT_SavePref);
 CALLBACK(preferences1   			,ACT_Pref);
CALLBACK(	check_frames			,ACT_VideoCheck);
 CALLBACK(quit1		   			,ACT_Exit);
 
 CALLBACK(requantize_mpeg1		   	,ACT_Requant);
  CALLBACK(run_script1		   	,ACT_RunScript);


   
   
  

  
