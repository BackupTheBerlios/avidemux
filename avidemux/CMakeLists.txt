########################################
# Macros
########################################
MACRO (add_libs_all_targets _libName)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${ARGV})
  
	IF (GTK_FOUND)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${ARGV})
	ENDIF (GTK_FOUND)
	
	IF (QT4_FOUND)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${ARGV})
	ENDIF (QT4_FOUND)
ENDMACRO (add_libs_all_targets)

MACRO (add_ldflags_all_targets _ldflags)
	add_target_link_flags(avidemux2_cli ${_ldflags})

	IF (GTK_FOUND)
		add_target_link_flags(avidemux2_gtk ${_ldflags})
	ENDIF (GTK_FOUND)

	IF (QT4_FOUND)
		add_target_link_flags(avidemux2_qt4 ${_ldflags})
	ENDIF (QT4_FOUND)
ENDMACRO (add_ldflags_all_targets)

########################################
# Definitions and Includes
########################################
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

IF (ADM_DEBUG)
	ADD_DEFINITIONS(-DADM_DEBUG)
ENDIF (ADM_DEBUG)

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR}
					${CMAKE_SOURCE_DIR}/avidemux
					${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries
					${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries/ADM_utilities
					${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries/ADM_lavutil
					${CMAKE_SOURCE_DIR}/avidemux/ADM_libraries/ADM_lavcodec
					${CMAKE_SOURCE_DIR}/avidemux/ADM_userInterfaces/ADM_commonUI
					${CMAKE_SOURCE_DIR}/avidemux/ADM_outputs)

########################################
# Our exectuables
########################################
SET(ADM_EXE_SRCS ADM_preview.cpp  gtk_gui.cpp  gui_autodrive.cpp  GUI_jobs.cpp  gui_navigate.cpp  guiplay.cpp  gui_savenew.cpp  main.cpp  gui_action.cpp)
ADD_DEFINITIONS(${GLIB_CFLAGS})

ADD_EXECUTABLE(avidemux2_cli ${ADM_EXE_SRCS})

IF (GTK_FOUND)
	ADD_EXECUTABLE(avidemux2_gtk ${ADM_EXE_SRCS})
ENDIF (GTK_FOUND)

IF (QT4_FOUND)
	ADD_EXECUTABLE(avidemux2_qt4 ${ADM_EXE_SRCS})
ENDIF (QT4_FOUND)

SDLify(main.cpp)
add_source_compile_flags(main.cpp "-DADM_SUBVERSION=${ADM_SUBVERSION}")

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
	add_libs_all_targets(${_current})
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
	add_libs_all_targets(${_current})
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
	add_libs_all_targets(${_current})
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
	add_libs_all_targets(${_current})
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
	add_libs_all_targets(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})

###########################################
# Construct ADM_userInterface
###########################################
ADD_SUBDIRECTORY(ADM_userInterfaces)

TARGET_LINK_LIBRARIES(avidemux2_cli ADM_UI_None)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_commonUI)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_ocr)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogNone)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogFactoryNone)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogFiltersNone)
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogGuiNone)

IF (GTK_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_UI_GTK)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_commonUI)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_ocr)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_ocrGtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_dialogGtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_filtersGtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_dialogFactoryGtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_gui2Gtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_toolkitGtk)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_commonUI)
ENDIF (GTK_FOUND)

IF (QT4_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_UI_QT4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_commonUI)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_ocr)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_ocrQt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_dialogQt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_dialogFactoryQt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_guiQt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_filtersQt4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_commonUI)
ENDIF (QT4_FOUND)

add_libs_all_targets(ADM_utilities)

###########################################
# External libs
###########################################
# libiconv
add_libs_all_targets("${LIBICONV_LIBRARY_DIR}")

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
	add_libs_all_targets("${FREETYPE2_LDFLAGS}")
ENDIF (USE_FREETYPE)

# FontConfig
IF (USE_FONTCONFIG)
	add_libs_all_targets("${FONTCONFIG_LIBRARY_DIR}")
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
add_libs_all_targets("${LIBXML2_LIBRARIES}")

# XVideo 
IF (USE_XV)
	add_libs_all_targets("${X11_LIBRARIES}")
	add_libs_all_targets("${XVIDEO_LIBRARY_DIR}")
ENDIF (USE_XV)

# SDL
IF (USE_SDL)
	add_libs_all_targets("${SDL_LIBRARY}")
ENDIF (USE_SDL)

# aRts
IF (USE_ARTS)
	add_libs_all_targets("${ARTS_LDFLAGS}")
ENDIF (USE_ARTS)

# ALSA
IF (ALSA_FOUND)
	add_libs_all_targets("${ASOUND_LIBRARY}")
ENDIF (ALSA_FOUND)

# ESD
IF (USE_ESD)
	add_libs_all_targets("${ESD_LIBRARY_DIR}")
ENDIF (USE_ESD)

# JACK
IF (USE_JACK)
	add_libs_all_targets("${JACK_LIBRARY_DIR}")
ENDIF (USE_JACK)

# amrnb
IF (USE_AMR_NB AND NOT USE_LATE_BINDING)
	add_libs_all_targets("${AMRNB_LIBRARY_DIR}")
ENDIF (USE_AMR_NB AND NOT USE_LATE_BINDING)

# libdca
IF (USE_LIBDCA AND NOT USE_LATE_BINDING)
	add_libs_all_targets("${LIBDCA_LIBRARY_DIR}")
ENDIF (USE_LIBDCA AND NOT USE_LATE_BINDING)

# LAME
IF (HAVE_LIBMP3LAME)
	add_libs_all_targets("${LAME_LIBRARY_DIR}")
ENDIF (HAVE_LIBMP3LAME)

# Vorbis
IF (USE_VORBIS)
	add_libs_all_targets("${VORBIS_LIBRARY_DIR}")
	add_libs_all_targets("${VORBISENC_LIBRARY_DIR}")
ENDIF (USE_VORBIS)

# FAAC
IF (USE_FAAC)
	add_libs_all_targets("${FAAC_LIBRARY_DIR}")
ENDIF (USE_FAAC)

# Aften
IF (USE_AFTEN)
	add_libs_all_targets("${AFTEN_LIBRARY_DIR}")
ENDIF (USE_AFTEN)

# FAAD
IF (USE_FAAD)
	add_libs_all_targets("${FAAD_LIBRARY_DIR}")
ENDIF (USE_FAAD)

# Secret Rabbit Code
IF (USE_SRC)
	add_libs_all_targets("${SRC_LIBRARY_DIR}")
ENDIF (USE_SRC)

# Xvid
IF (USE_XVID_4)
	add_libs_all_targets("${XVID_LIBRARY_DIR}")
ENDIF (USE_XVID_4)

# x264
IF (USE_X264)
	add_libs_all_targets("${X264_LIBRARY_DIR}")
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
	add_libs_all_targets(ADM_osSupport pthreadGC2 winmm wsock32 imagehlp)
	add_ldflags_all_targets("-mms-bitfield -mno-cygwin -mwinmm -mwindows ${CMAKE_SOURCE_DIR}/avidemux/xpm/adm.o -Wl,--export-all-symbols")

	IF (CMAKE_BUILD_TYPE STREQUAL "Release")
		add_ldflags_all_targets("-Wl,-s")
	ENDIF (CMAKE_BUILD_TYPE STREQUAL "Release")
ENDIF (ADM_OS_WINDOWS)

IF (ADM_OS_DARWIN)
	add_libs_all_targets("-framework CoreServices -framework CoreAudio -framework AudioUnit")
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
