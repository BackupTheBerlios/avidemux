########################################
# Macros
########################################
MACRO (ADD_ADMLIB_ALL_TARGETS _libName)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${_libName}_cli)
  
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${_libName}_gtk)
	ENDIF (ADM_UI_GTK)
	
	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${_libName}_qt4)
	ENDIF (ADM_UI_QT4)
ENDMACRO (ADD_ADMLIB_ALL_TARGETS)

MACRO (ADD_LIB_ALL_TARGETS _libName)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${ARGV})
  
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${ARGV})
	ENDIF (ADM_UI_GTK)
	
	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${ARGV})
	ENDIF (ADM_UI_QT4)
ENDMACRO (ADD_LIB_ALL_TARGETS)

########################################
# Definitions and Includes
########################################
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

IF (ADM_DEBUG)
	ADD_DEFINITIONS(-DADM_DEBUG)
ENDIF (ADM_DEBUG)

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux)		# ADM_assert.h, prefs.h
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries/ADM_utilities)		# default.h, fourcc.h
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries/ADM_lavutil)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries/ADM_lavcodec)

########################################
# Our exectuables
########################################
SET(ADM_EXE_SRCS ADM_preview.cpp  gtk_gui.cpp  gui_autodrive.cpp  GUI_jobs.cpp  gui_navigate.cpp  guiplay.cpp  gui_savenew.cpp  main.cpp  gui_action.cpp)

ADD_EXECUTABLE(avidemux2_cli ${ADM_EXE_SRCS})
ADD_TARGET_CFLAGS(avidemux2_cli "-I${CMAKE_BINARY_DIR}/config/cli")

IF (ADM_UI_GTK)
	ADD_EXECUTABLE(avidemux2_gtk ${ADM_EXE_SRCS})
	ADD_TARGET_CFLAGS(avidemux2_gtk "-I${CMAKE_BINARY_DIR}/config/gtk")
ENDIF (ADM_UI_GTK)

IF (ADM_UI_QT4)
	ADD_EXECUTABLE(avidemux2_qt4 ${ADM_EXE_SRCS})
	ADD_TARGET_CFLAGS(avidemux2_qt4 "-I${CMAKE_BINARY_DIR}/config/qt4")
ENDIF (ADM_UI_QT4)

SDLify(main.cpp)
ADD_SOURCE_CFLAGS(main.cpp "-DADM_SUBVERSION=${ADM_SUBVERSION}")
ADD_SOURCE_CFLAGS(gtk_gui.cpp ${GLIB_CFLAGS})

IF (USE_AMR_NB)
	ADD_SOURCE_CFLAGS(main.cpp -I${AMRNB_INCLUDE_DIR})
ENDIF (USE_AMR_NB)

IF (USE_LIBDCA)
	ADD_SOURCE_CFLAGS(main.cpp -I${LIBDCA_INCLUDE_DIR})
ENDIF (USE_LIBDCA)

########################################
# Compile and link general subdirs
########################################
SET(ADM_SUBDIR 
	ADM_filter
	ADM_audiofilter
	ADM_editor
	ADM_audiocodec
	ADM_audio
	ADM_script
	ADM_videoFilter
	ADM_filter
	ADM_video
	ADM_audiodevice
	ADM_ocr
	ADM_colorspace)

FOREACH(_current ${ADM_SUBDIR})
	ADD_SUBDIRECTORY(${_current})
	ADD_ADMLIB_ALL_TARGETS(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})

###########################################
# Construct ADM_outputs
###########################################
ADD_SUBDIRECTORY(ADM_outputs)

SET(ADM_OUTPUTS
	oplug_avi
	oplug_mp4
	oplug_mpeg
	oplug_mpegFF
	oplug_ogm
	oplug_flv
	oplug_dummy)

FOREACH(_current ${ADM_OUTPUTS})
	ADD_ADMLIB_ALL_TARGETS(${_current})
ENDFOREACH(_current ${ADM_OUTPUTS})

###########################################
# Construct ADM_inputs
###########################################
ADD_SUBDIRECTORY(ADM_inputs)

SET(ADM_INPUTS
	ADM_asf
	ADM_avsproxy
	ADM_h263
	ADM_inpics
	ADM_matroska
	ADM_mp4
	ADM_mpegdemuxer
	ADM_nuv
	ADM_ogm
	ADM_openDML
	ADM_flv
	ADM_amv)

FOREACH(_current ${ADM_INPUTS})
	ADD_ADMLIB_ALL_TARGETS(${_current})
ENDFOREACH(_current ${ADM_INPUTS})

#########################################
# Tails
#########################################
SET(ADM_TAILS
	ADM_osSupport
	ADM_encoder
	ADM_toolkit
	ADM_codecs
	ADM_requant
	ADM_infoExtractor)

FOREACH(_current ${ADM_TAILS})
	ADD_SUBDIRECTORY(${_current})
	ADD_ADMLIB_ALL_TARGETS(${_current})
ENDFOREACH(_current ${ADM_TAILS})

###########################################
# Construct ADM_libraries
###########################################
ADD_SUBDIRECTORY(ADM_libraries)

SET(ADM_LIBRARIES
	ADM_lavformat
	ADM_lavcodec
	ADM_lavutil
	ADM_liba52
	ADM_libMad
	ADM_mplex
	ADM_libMpeg2Dec
	ADM_libmpeg2enc
	ADM_libswscale
	ADM_libtwolame
	ADM_libwrapper
	ADM_lvemux
	ADM_smjs
	ADM_utilities
	ADM_xvidratectl
	ADM_libpostproc
	ADM_colorspace)

IF (USE_FREETYPE)
	SET(ADM_LIBRARIES ${ADM_LIBRARIES} ADM_libass)
ENDIF (USE_FREETYPE)

IF (ADM_DEBUG AND FIND_LEAKS)
	SET(ADM_LIBRARIES ${ADM_LIBRARIES} ADM_nvwa)
ENDIF (ADM_DEBUG AND FIND_LEAKS)

IF (ADM_OS_DARWIN AND USE_SDL)
	SET(ADM_LIBRARIES ${ADM_LIBRARIES} ADM_libsdl)
ENDIF (ADM_OS_DARWIN AND USE_SDL)

FOREACH(_current ${ADM_LIBRARIES})
	ADD_ADMLIB_ALL_TARGETS(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})

###########################################
# Construct ADM_userInterface
###########################################
ADD_SUBDIRECTORY(ADM_userInterfaces)

TARGET_LINK_LIBRARIES(avidemux2_cli ADM_UI_None_cli)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_commonUI_cli)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_ocr_cli)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogNone_cli)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogFactoryNone_cli)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogFiltersNone_cli)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogGuiNone_cli)

IF (GTK_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_UI_GTK_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_commonUI_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_ocr_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_ocrGtk_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_dialogGtk_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_filtersGtk_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_dialogFactoryGtk_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_gui2Gtk_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_toolkitGtk_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_commonUI_gtk)
ENDIF (GTK_FOUND)

IF (QT4_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_UI_QT4_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_commonUI_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_ocr_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_ocrQt4_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_dialogQt4_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_dialogFactoryQt4_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_guiQt4_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_filtersQt4_qt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_commonUI_qt4)
ENDIF (QT4_FOUND)

ADD_ADMLIB_ALL_TARGETS(ADM_utilities)

###########################################
# External libs
###########################################
# libiconv
ADD_LIB_ALL_TARGETS("${LIBICONV_LIBRARY_DIR}")

# gettext
IF (GETTEXT_FOUND)
	INCLUDE_DIRECTORIES(${GETTEXT_INCLUDE_DIR})
	TARGET_LINK_LIBRARIES(avidemux2_cli "${GETTEXT_LIBRARY_DIR}")

	IF (GTK_FOUND)
		TARGET_LINK_LIBRARIES(avidemux2_gtk "${GETTEXT_LIBRARY_DIR}")
	ENDIF (GTK_FOUND)
ENDIF (GETTEXT_FOUND)

# Freetype2
IF (USE_FREETYPE)
	ADD_LIB_ALL_TARGETS("${FREETYPE2_LDFLAGS}")
ENDIF (USE_FREETYPE)

# FontConfig
IF (USE_FONTCONFIG)
	ADD_LIB_ALL_TARGETS("${FONTCONFIG_LIBRARY_DIR}")
ENDIF (USE_FONTCONFIG)

# PNG
IF (USE_PNG)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${PNG_LIBRARIES})
	
	IF (GTK_FOUND)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${PNG_LIBRARIES})
	ENDIF (GTK_FOUND)
	
	IF (QT4_FOUND)
		IF (ADM_OS_DARWIN AND NOT QT_USE_FRAMEWORKS)
			TARGET_LINK_LIBRARIES(avidemux2_qt4 ${PNG_LIBRARIES})
		ELSEIF (NOT ADM_OS_DARWIN)
			TARGET_LINK_LIBRARIES(avidemux2_qt4 ${PNG_LIBRARIES})
		ENDIF (ADM_OS_DARWIN AND NOT QT_USE_FRAMEWORKS)
	ENDIF (QT4_FOUND)
ENDIF (USE_PNG)

# Libxml2
ADD_LIB_ALL_TARGETS("${LIBXML2_LIBRARIES}")

# XVideo 
IF (USE_XV)
	ADD_LIB_ALL_TARGETS("${X11_LIBRARIES}")
	ADD_LIB_ALL_TARGETS("${XVIDEO_LIBRARY_DIR}")
ENDIF (USE_XV)

# SDL
IF (USE_SDL)
	ADD_LIB_ALL_TARGETS("${SDL_LIBRARY}")
ENDIF (USE_SDL)

# aRts
IF (USE_ARTS)
	ADD_LIB_ALL_TARGETS("${ARTS_LDFLAGS}")
ENDIF (USE_ARTS)

# ALSA
IF (ALSA_FOUND)
	ADD_LIB_ALL_TARGETS("${ASOUND_LIBRARY}")
ENDIF (ALSA_FOUND)

# ESD
IF (USE_ESD)
	ADD_LIB_ALL_TARGETS("${ESD_LIBRARY_DIR}")
ENDIF (USE_ESD)

# JACK
IF (USE_JACK)
	ADD_LIB_ALL_TARGETS("${JACK_LIBRARY_DIR}")
ENDIF (USE_JACK)

# amrnb
IF (USE_AMR_NB AND NOT USE_LATE_BINDING)
	ADD_LIB_ALL_TARGETS("${AMRNB_LIBRARY_DIR}")
ENDIF (USE_AMR_NB AND NOT USE_LATE_BINDING)

# libdca
IF (USE_LIBDCA AND NOT USE_LATE_BINDING)
	ADD_LIB_ALL_TARGETS("${LIBDCA_LIBRARY_DIR}")
ENDIF (USE_LIBDCA AND NOT USE_LATE_BINDING)

# LAME
IF (HAVE_LIBMP3LAME)
	ADD_LIB_ALL_TARGETS("${LAME_LIBRARY_DIR}")
ENDIF (HAVE_LIBMP3LAME)

# Vorbis
IF (USE_VORBIS)
	ADD_LIB_ALL_TARGETS("${VORBIS_LIBRARY_DIR}")
	ADD_LIB_ALL_TARGETS("${VORBISENC_LIBRARY_DIR}")
ENDIF (USE_VORBIS)

# FAAC
IF (USE_FAAC)
	ADD_LIB_ALL_TARGETS("${FAAC_LIBRARY_DIR}")
ENDIF (USE_FAAC)

# Aften
IF (USE_AFTEN)
	ADD_LIB_ALL_TARGETS("${AFTEN_LIBRARY_DIR}")
ENDIF (USE_AFTEN)

# FAAD
IF (USE_FAAD)
	ADD_LIB_ALL_TARGETS("${FAAD_LIBRARY_DIR}")
ENDIF (USE_FAAD)

# Secret Rabbit Code
IF (USE_SRC)
	ADD_LIB_ALL_TARGETS("${SRC_LIBRARY_DIR}")
ENDIF (USE_SRC)

# Xvid
IF (USE_XVID_4)
	ADD_LIB_ALL_TARGETS("${XVID_LIBRARY_DIR}")
ENDIF (USE_XVID_4)

# x264
IF (USE_X264)
	ADD_LIB_ALL_TARGETS("${X264_LIBRARY_DIR}")
ENDIF (USE_X264)

###########################################
# UI Specific
###########################################
TARGET_LINK_LIBRARIES(avidemux2_cli ${GLIB_LDFLAGS})

IF (GTK_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ${GTK_LDFLAGS} ${GTHREAD_LDFLAGS})
ENDIF (GTK_FOUND)

IF (QT4_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ${GLIB_LDFLAGS})
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
ENDIF (QT4_FOUND)

###########################################
# OS Specific
###########################################
IF (ADM_OS_WINDOWS)
	ADD_ADMLIB_ALL_TARGETS(ADM_osSupport)
	ADD_LIB_ALL_TARGETS(pthreadGC2 winmm wsock32 imagehlp)
	ADD_LIB_ALL_TARGETS("-mno-cygwin -mwinmm -mwindows ${CMAKE_SOURCE_DIR}/avidemux/xpm/adm.o -Wl,--export-all-symbols")

	IF (CMAKE_BUILD_TYPE STREQUAL "Release")
		ADD_LIB_ALL_TARGETS("-Wl,-s")
	ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")
ENDIF (ADM_OS_WINDOWS)

IF (ADM_OS_DARWIN)
	ADD_LIB_ALL_TARGETS("-framework CoreServices -framework CoreAudio -framework AudioUnit")
ENDIF (ADM_OS_DARWIN)

###########################################
# Install
###########################################
SET(ADM_EXES avidemux2_cli)

IF (GTK_FOUND)
	SET(ADM_EXES ${ADM_EXES} avidemux2_gtk)
ENDIF (GTK_FOUND)

IF (QT4_FOUND)
	SET(ADM_EXES ${ADM_EXES} avidemux2_qt4)
ENDIF (QT4_FOUND)

INSTALL(TARGETS ${ADM_EXES} RUNTIME DESTINATION bin LIBRARY DESTINATION lib)

# EOF
