/bin/sh ../libtool --silent --tag=CXX --mode=link i586-mingw32msvc-g++  -I.. -IADM_library -I../ADM_library  -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -O -Wnon-virtual-dtor -Wno-long-long -Wundef -Wall -W -Wpointer-arith -Wmissing-prototypes -Wwrite-strings -O2 -mno-cygwin -mms-bitfields -fno-exceptions -fno-check-new -O2   \
-L/rot2/lib \
 -mwindows \
/rot2/lib/libpthreadGC1.a \
ADM_osSupport/libADM_ossupport.a \
./ADM_libraries/ADM_lavutil/libADM_lavutil.a \
./ADM_libraries/ADM_utilities/libADM_library.a \
ADM_userInterfaces/ADM_GTK/ADM_dialog/libADM_dialog.a \
ADM_userInterfaces/ADM_GTK/ADM_gui2/libADM_gui2.a \
ADM_userInterfaces/ADM_GTK/ADM_filters/libADM_filters.a \
ADM_userInterfaces/ADM_GTK/ADM_toolkit_gtk/libADM_toolkit_gtk.a \
ADM_userInterfaces/ADM_GTK/libADM_GTK.a \
 ./ADM_libraries/ADM_lavcodec/libavcodec.a  \
 ./ADM_libraries/ADM_lavcodec/libpostproc/libpostproc.a  \
./ADM_script/libADM_script.a \
./ADM_editor/libADM_editor.a \
./ADM_audiocodec/libADM_audiocodec.a \
./ADM_audio/libADM_audio.a \
 ./ADM_outputs/oplug_mp4/liboplug_mp4.a \
 ./ADM_outputs/oplug_mpeg/liboplug_mpeg.a \
 ./ADM_outputs/oplug_mpegFF/liboplug_mpegFF.a \
 ./ADM_outputs/oplug_ogm/liboplug_ogm.a \
 ./ADM_outputs/oplug_avi/liboplug_avi.a \
./ADM_requant/libADM_requant.a \
./ADM_libraries/ADM_lvemux/libADM_lvemux.a \
./ADM_libraries/ADM_lavformat/libADM_lavformat.a \
./ADM_libraries/ADM_mplex/libADM_mplex.a \
./ADM_filter/libADM_filter.a \
./ADM_video/libADM_video.a \
  ./ADM_encoder/libADM_encoder.a \
./ADM_requant/libADM_requant.a \
./ADM_codecs/libADM_codecs.a \
 ./ADM_libraries/ADM_libMad/libADM_mad.a \
 ./ADM_libraries/ADM_liba52/libADM_liba52.a \
  ./ADM_audiofilter/libADM_audiofilter.a \
 ./ADM_libraries/ADM_libtwolame/liblibtoolame.a \
 ./ADM_libraries/ADM_xvidratectl/libADM_xvidratectl.a \
 ./ADM_libraries/ADM_libmpeg2enc/libmpeg2enc.a  \
 ./ADM_inputs/ADM_3gp/libADM_3gp.a \
 ./ADM_inputs/ADM_asf/libADM_asf.a \
 ./ADM_inputs/ADM_h263/libADM_h263.a \
 ./ADM_inputs/ADM_inpics/libADM_inpics.a \
 ./ADM_inputs/ADM_mpegdemuxer/libADM_mpegdemuxer.a \
 ./ADM_inputs/ADM_nuv/libADM_nuv.a \
 ./ADM_inputs/ADM_ogm/libADM_ogm.a \
 ./ADM_inputs/ADM_openDML/libADM_openDML.a \
 ./ADM_inputs/ADM_avsproxy/libADM_avsproxy.a \
 ./ADM_inputs/ADM_matroska/libADM_matroska.a \
 ./ADM_inputs/ADM_mpegdemuxer/libADM_mpegdemuxer.a \
 ./ADM_libraries/ADM_libMpeg2Dec/liblibMpeg2Dec.a  \
./ADM_ocr/libADM_ocr.a \
./ADM_toolkit/libADM_toolkit.a \
./ADM_colorspace/libADM_colorspace.a \
 ./ADM_outputs/oplug_ogm/liboplug_ogm.a \
 ./ADM_libraries/ADM_lavcodec/libavcodec.a  \
 ./ADM_libraries/ADM_lavcodec/libpostproc/libpostproc.a  \
/rot2/lib/libx264.dll.a \
 ./ADM_libraries/ADM_lavutil/libADM_lavutil.a \
 ./ADM_libraries/ADM_utilities/libADM_library.a \
ADM_osSupport/libADM_ossupport.a \
ADM_audiodevice/libADM_audiodevice.a \
./ADM_libraries/ADM_libass/libass.a \
ADM_userInterfaces/ADM_GTK/ADM_gui2/libADM_gui2.a \
ADM_userInterfaces/ADM_GTK/ADM_filters/libADM_filters.a \
ADM_userInterfaces/ADM_GTK/ADM_toolkit_gtk/libADM_toolkit_gtk.a \
ADM_userInterfaces/ADM_GTK/libADM_GTK.a \
ADM_userInterfaces/ADM_commonUI/libADM_CommonUI.a \
 ADM_userInterfaces/ADM_GTK/ADM_dialog/libADM_dialog.a \
ADM_osSupport/libADM_ossupport.a \
 ./ADM_libraries/ADM_libswscale/libswscale.a \
./ADM_ocr/libADM_ocr.a \
ADM_userInterfaces/ADM_GTK/ADM_toolkit_gtk/libADM_toolkit_gtk.a \
 \
-lSDLmain  \
 -lfaac \
 -lmp3lame \
 -lxvidcore \
 -lxml2 \
 -lfreetype \
 -lfaad \
 -liconv \
 -lgettextpo \
 -lintl \
 -logg -lvorbis -lvorbisenc \
 -laften \
 -ljs \
 gui_savenew.o \
 gui_navigate.o \
 gtk_gui.o \
 main.o \
 guiplay.o \
 gui_autodrive.o \
 GUI_jobs.o \
 xpm/adm.o \
-lgtk-win32-2.0 -lgdk-win32-2.0 -latk-1.0 -lgdk_pixbuf-2.0 -lpangowin32-1.0 -lgdi32 -lpango-1.0 -lgobject-2.0 -lgmodule-2.0 -lglib-2.0 -lgthread-2.0 \
-lpng13 \
  -lzdll \
 -lpthreadGC1 \
  -lnspr4 \
  -lfontconfig \
  -lmingw32 \
 -lwinmm -lwsock32  \
 -lSDLmain -lSDL \
-o avidemux2_gtk.exe  \
-Wl,--export-all-symbols \
-Wl,--out-implib=libavidemux2.dll.a \
    -Wl,--enable-auto-import \
    -mno-cygwin 



 #/rot/lib/libfaac.a \

