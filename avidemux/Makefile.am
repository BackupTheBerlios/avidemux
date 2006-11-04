####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = avidemux2_gtk avidemux2_cli

## INCLUDES were found outside kdevelop specific part

if BUILD_ALTIVEC
libmpeg2enc_Altivec = ./mpeg2enc/altivec/libaltivec.la
endif

user_interface_gtk = ./ADM_userInterfaces/ADM_GTK/ADM_gui2/libADM_gui2.a	\
		./ADM_userInterfaces/ADM_GTK/libADM_GTK.a \
		./ADM_userInterfaces/ADM_GTK/ADM_dialog/libADM_dialog.a	\
		./ADM_userInterfaces/ADM_GTK/ADM_filters/libADM_filters.a \
		./ADM_userInterfaces/ADM_GTK/ADM_toolkit_gtk/libADM_toolkit_gtk.a	
user_interface_cli = ./ADM_userInterfaces/ADM_NONE/ADM_gui2/libADM_gui2.a	\
		./ADM_userInterfaces/ADM_NONE/ADM_dialog/libADM_dialog.a	\
		./ADM_userInterfaces/ADM_NONE/ADM_filters/libADM_filters.a 

adm_src =   guiplay.cpp  \
	gui_savenew.cpp  gui_navigate.cpp gtk_gui.cpp   \
	main.cpp   \
	gui_autodrive.cpp GUI_jobs.cpp

avidemux2_gtk_SOURCES =   $(adm_src)
avidemux2_cli_SOURCES =   $(adm_src)

SUBDIRS = ADM_libraries ADM_audio ADM_audiocodec ADM_audiofilter ADM_codecs \
	ADM_colorspace ADM_editor ADM_encoder ADM_filter  \
	ADM_toolkit xpm ADM_video ADM_audiodevice \
	\
	ADM_requant \
	ADM_script ADM_ocr \
	ADM_osSupport \
	ADM_inputs ADM_outputs \
	ADM_userInterfaces
	


EXTRA_DIST = ADM_assert.h       gtkgui.h         \
h  gui_autodrive.cpp  main.cpp   \
ADM_lavcodec.h  GUI_jobs.cpp      gui_navigate.cpp   \
gui_savenew.cpp     \
     avi_vars.h      gtk_gui.cpp    guiplay.cpp    prefs.h \
gui_action.hxx prefs.in

####### kdevelop will overwrite this part!!! (end)############

adm_lib= \
ADM_filter/filter_saveload.o 				\
./ADM_audiofilter/libADM_audiofilter.a			\
./ADM_filter/libADM_filter.a				\
./ADM_editor/libADM_editor.a				\
./ADM_audiocodec/libADM_audiocodec.a			\
./ADM_audio/libADM_audio.a				\
./ADM_script/libADM_script.a				\
./ADM_video/libADM_video.a				\
./ADM_colorspace/libADM_colorspace.a			\
./ADM_audiodevice/libADM_audiodevice.a			\
./ADM_ocr/libADM_ocr.a					\
./ADM_outputs/oplug_mp4/liboplug_mp4.a			\
./ADM_outputs/oplug_mpeg/liboplug_mpeg.a		\
./ADM_outputs/oplug_mpegFF/liboplug_mpegFF.a		\
./ADM_outputs/oplug_ogm/liboplug_ogm.a			\
./ADM_outputs/oplug_avi/liboplug_avi.a			\
./ADM_encoder/libADM_encoder.a				\
./ADM_codecs/libADM_codecs.a				\
./ADM_requant/libADM_requant.a				\
./ADM_inputs/ADM_3gp/libADM_3gp.a			\
./ADM_inputs/ADM_asf/libADM_asf.a			\
./ADM_inputs/ADM_h263/libADM_h263.a			\
./ADM_inputs/ADM_inpics/libADM_inpics.a			\
./ADM_inputs/ADM_mpegdemuxer/libADM_mpegdemuxer.a	\
./ADM_inputs/ADM_nuv/libADM_nuv.a			\
./ADM_inputs/ADM_ogm/libADM_ogm.a			\
./ADM_inputs/ADM_openDML/libADM_openDML.a		\
./ADM_inputs/ADM_avsproxy/libADM_avsproxy.a		\
./ADM_inputs/ADM_matroska/libADM_matroska.a		\
./ADM_libraries/ADM_lavcodec/libpostproc/libpostproc.a	\
./ADM_libraries/ADM_mplex/libADM_mplex.a		\
./ADM_libraries/ADM_lvemux/libADM_lvemux.a		\
./ADM_libraries/ADM_lavformat/libADM_lavformat.a	\
./ADM_libraries/ADM_lavcodec/libavcodec.a		\
./ADM_libraries/ADM_lavutil/libADM_lavutil.a		\
./ADM_libraries/ADM_liba52/libADM_liba52.a		\
./ADM_libraries/ADM_libMad/libADM_mad.a			\
./ADM_libraries/ADM_libass/libass.a			\
./ADM_libraries/ADM_libtwolame/liblibtoolame.a		\
./ADM_libraries/ADM_libMpeg2Dec/liblibMpeg2Dec.a	\
./ADM_libraries/ADM_libmpeg2enc/libmpeg2enc.a		\
./ADM_libraries/ADM_xvidratectl/libADM_xvidratectl.a	

adm_postlib= \
./ADM_toolkit/libADM_toolkit.a				\
./ADM_libraries/ADM_utilities/libADM_library.a		\
./ADM_osSupport/libADM_ossupport.a			\
./ADM_libraries/ADM_libswscale/libswscale.a		\
./ADM_ocr/libADM_ocr.a					\
$(libmpeg2enc_Altivec) 

avidemux2_gtk_LDADD   =  \
	$(adm_lib) 					\
	$(user_interface_gtk)				\
	$(adm_postlib) 
	
avidemux2_cli_LDADD   =  \
	$(adm_lib) 					\
	$(user_interface_cli)				\
	$(adm_postlib) 
	
INCLUDES = $(all_includes) $(GTK_CFLAGS) $(XML_CFLAGS) \
	$(SMONKEY_CFLAGS) \
	-DADMLOCALE='"'$(ADM_LOCALEDIR)'"' \
	-IADM_inputs -IADM_outputs \
	-IADM_lavutil  -IADM_libraries -IADM_libraries/ADM_utilities -IADM_libraries/ADM_lavutil \
	-IADM_userInterfaces/ADM_commonUI \
	-IADM_userInterfaces/ADM_GTK

# the library search path.
avidemux2_gtk_LDFLAGS = $(all_libraries)  $(SDL_LIBS) $(GTK_LIBS) \
	$(FREETYPE_LIBS) $(FC_LIBS) $(ARTS_LIBS) $(XML_LIBS) 

avidemux2_cli_LDFLAGS = $(all_libraries)  $(SDL_LIBS)  \
	$(FREETYPE_LIBS) $(FC_LIBS) $(ARTS_LIBS) $(XML_LIBS) 

noinst_HEADERS = 
AM_CFLAGS = -g
AM_CXXFLAGS = -g
