####### kdevelop will overwrite this part!!! (begin)##########
bin_PROGRAMS = avidemux2

## INCLUDES were found outside kdevelop specific part

if BUILD_ALTIVEC
libmpeg2enc_Altivec = ./mpeg2enc/altivec/libaltivec.la
endif

avidemux2_SOURCES =   guiplay.cpp  \
	gui_savenew.cpp  gui_navigate.cpp gtk_gui.cpp callbacks.cpp avilist.cpp \
	main.cpp prefs.cpp ADM_guiBitrate.cpp ADM_requantize.cpp ADM_pp.cpp \
	ADM_memsupport.cpp gui_autodrive.cpp GUI_jobs.cpp
#avidemux_LDADD   = ./ADM_library/libADM_library.a ./ADM_audio/libADM_audio.a ./ADM_audiocodec/libADM_audiocodec.a ./ADM_audiofilter/libADM_audiofilter.a ./ADM_codecs/libADM_codecs.a ./ADM_colorspace/libADM_colorspace.a ./ADM_editor/libADM_editor.a ./ADM_encoder/libADM_encoder.a ./ADM_filter/libADM_filter.a ../ADM_gui2/libADM_gui2.a /ADM_gui/libADM_gui.a ./ADM_inpics/libADM_inpics.a ./ADM_nuv/libADM_nuv.a ./ADM_toolkit/libADM_toolkit.a ./oplug_mpeg/liboplug_mpeg.a ./ADM_video/libADM_video.a ./oplug_avi/liboplug_avi.a ./ADM_audiodevice/libADM_audiodevice.a ./ADM_vp32/libADM_vp32.a ./ADM_mpeg2dec/libADM_mpeg2dec.a ./ADM_mpegindexer/libADM_mpegindexer.a ./libMpeg2Dec/liblibMpeg2Dec.a ./oplug_mpegFF/liboplug_mpegFF.a  -L/usr/X11R6/lib -lm -lXext -lX11

SUBDIRS = ADM_library ADM_audio ADM_audiocodec ADM_audiofilter ADM_codecs \
	ADM_colorspace ADM_editor ADM_encoder ADM_filter  ADM_inpics ADM_nuv \
	ADM_toolkit oplug_mpeg xpm ADM_video oplug_avi ADM_audiodevice ADM_vp32 \
	libMpeg2Dec oplug_mpegFF ADM_h263 ADM_gui2 ADM_dialog \
	ADM_ogm ADM_3gp mpeg2enc ADM_openDML ADM_lvemux libtoolame ADM_requant \
	ADM_script oplug_ogm ADM_xvidratectl ADM_mplex ADM_ocr MPlayer_pp \
	ADM_mpegdemuxer ADM_tray ADM_lavutil ADM_lavformat ADM_lavcodec oplug_mp4 \
	ADM_avsproxy  ADM_liba52

EXTRA_DIST = main.cpp avi_vars.h avilist.cpp avilist.h callbacks.cpp callbacks.h gtk_gui.cpp gtkgui.h gui_action.hxx gui_navigate.cpp   gui_savenew.cpp  guiplay.cpp interface.h  support.h  prototype.h ADM_lavcodec.h  prefs.h prefs.cpp prefs.in mmx_macros.h ADM_assert.h

####### kdevelop will overwrite this part!!! (end)############
# set the include path found by configure
#INCLUDES= $(all_includes)

#avidemux2_LDADD   =    ./ADM_editor/libADM_editor.a  ./ADM_audiocodec/libADM_audiocodec.a ./ADM_audio/libADM_audio.a ./oplug_avi/liboplug_avi.a ./ADM_filter/libADM_filter.a ./ADM_video/libADM_video.a  ./ADM_encoder/libADM_encoder.a  ./ADM_codecs/libADM_codecs.a ./ADM_vp32/libADM_vp32.a   ./ADM_audiofilter/libADM_audiofilter.a ./oplug_mpegFF/liboplug_mpegFF.a  ./oplug_mpeg/liboplug_mpeg.a ./ADM_gui2/libADM_gui2.a  ./ADM_gui/libADM_gui.a ./ADM_inpics/libADM_inpics.a  ./ADM_h263/libADM_h263.a   ./ADM_nuv/libADM_nuv.a  ./ADM_colorspace/libADM_colorspace.a ./ADM_audiodevice/libADM_audiodevice.a ./ADM_toolkit/libADM_toolkit.a ./ADM_mpeg2dec/libADM_mpeg2dec.a ./ADM_mpegindexer/libADM_mpegindexer.a   ./libMpeg2Dec/liblibMpeg2Dec.a    ./ADM_dialog/libADM_dialog.a


avidemux2_LDADD   =  \
  ADM_lavcodec/ADMlibavcodec.o \
./ADM_lavutil/libADM_lavutil.o \
  ADM_lavcodec/libpostproc/libpostproc.o \
  ./ADM_script/libADM_script.o \
./ADM_editor/libADM_editor.o \
./ADM_library/libADM_library.o \
./ADM_openDML/libADM_openDML.o \
./oplug_ogm/liboplug_ogm.o \
./ADM_audiocodec/libADM_audiocodec.o \
./ADM_audio/libADM_audio.o \
./oplug_avi/liboplug_avi.o \
./oplug_mp4/liboplug_mp4.o \
./oplug_mpeg/liboplug_mpeg.o \
./oplug_mpegFF/liboplug_mpegFF.o  \
./ADM_requant/libADM_requant.o \
./ADM_mplex/libADM_mplex.o \
./ADM_lavformat/libADM_lavformat.o \
./ADM_lvemux/libADM_lvemux.o \
./ADM_filter/libADM_filter.o \
./ADM_video/libADM_video.o \
./ADM_encoder/libADM_encoder.o  \
./ADM_codecs/libADM_codecs.o \
./ADM_vp32/libADM_vp32.o   \
./ADM_audiofilter/libADM_audiofilter.o \
./libtoolame/liblibtoolame.o \
./ADM_gui2/libADM_gui2.o  \
./MPlayer_pp/libMPlayer_pp.o \
./ADM_colorspace/libADM_colorspace.o \
./ADM_toolkit/libADM_toolkit.o  \
./ADM_3gp/libADM_3gp.o \
./ADM_avsproxy/libADM_avsproxy.o \
./ADM_audiodevice/libADM_audiodevice.o \
./ADM_ocr/libADM_ocr.o \
./ADM_tray/libADM_tray.o \
./ADM_nuv/libADM_nuv.o  \
./ADM_ogm/libADM_ogm.o \
./ADM_h263/libADM_h263.o   \
./ADM_inpics/libADM_inpics.o  \
./ADM_dialog/libADM_dialog.o  \
./mpeg2enc/libmpeg2enc.o \
./ADM_xvidratectl/libADM_xvidratectl.o  \
./ADM_mpegdemuxer/libADM_mpegdemuxer.o   \
./ADM_liba52/libADM_liba52.o \
./libMpeg2Dec/liblibMpeg2Dec.o   \
$(libmpeg2enc_Altivec) 


INCLUDES = $(all_includes) $(GTK_CFLAGS) $(XML_CFLAGS) \
	$(SMONKEY_CFLAGS) \
	-DADMLOCALE='"'$(ADM_LOCALEDIR)'"' \
	-IADM_lavutil

# the library search path.
avidemux2_LDFLAGS = $(all_libraries)  $(SDL_LIBS) $(GTK_LIBS) \
	$(FREETYPE_LIBS) $(ARTS_LIBS) $(XML_LIBS) 

noinst_HEADERS = ADM_pp.h admmangle.h
AM_CFLAGS = -g
AM_CXXFLAGS = -g
