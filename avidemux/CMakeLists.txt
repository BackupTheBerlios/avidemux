########################################
# Macros
########################################
MACRO (ADD_ADMLIB_ALL_TARGETS)
	FOREACH (_libName ${ARGV})
		TARGET_LINK_LIBRARIES(avidemux2_cli ${_libName}_cli)

		IF (ADM_UI_GTK)
			TARGET_LINK_LIBRARIES(avidemux2_gtk ${_libName}_gtk)
		ENDIF (ADM_UI_GTK)

		IF (ADM_UI_QT4)
			TARGET_LINK_LIBRARIES(avidemux2_qt4 ${_libName}_qt4)
		ENDIF (ADM_UI_QT4)
	ENDFOREACH (_libName ${ARGV})
ENDMACRO (ADD_ADMLIB_ALL_TARGETS)


MACRO (ADD_LIB_ALL_TARGETS)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${ARGV})

	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${ARGV})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${ARGV})
	ENDIF (ADM_UI_QT4)
ENDMACRO (ADD_LIB_ALL_TARGETS)


MACRO (ADD_LDFLAGS_ALL_TARGETS)
	ADD_TARGET_LDFLAGS(avidemux2_cli ${ARGV})

	IF (ADM_UI_GTK)
		ADD_TARGET_LDFLAGS(avidemux2_gtk ${ARGV})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		ADD_TARGET_LDFLAGS(avidemux2_qt4 ${ARGV})
	ENDIF (ADM_UI_QT4)
ENDMACRO (ADD_LDFLAGS_ALL_TARGETS)

########################################
# Definitions and Includes
########################################
ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64 -D_LARGE_FILES)
ADD_DEFINITIONS(-DHAVE_CONFIG_H)

IF (ADM_DEBUG)
	ADD_DEFINITIONS(-DADM_DEBUG)
ENDIF (ADM_DEBUG)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}")
INCLUDE_DIRECTORIES("${CMAKE_BINARY_DIR}/config")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/ADM_core/include")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/ADM_libraries/ADM_utilities")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/ADM_libraries/ADM_lavutil")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/ADM_libraries/ADM_lavcodec")

IF (GETTEXT_FOUND)
	INCLUDE_DIRECTORIES(${GETTEXT_INCLUDE_DIR})
ENDIF (GETTEXT_FOUND)

########################################
# Embed resources for executables
########################################
IF (APPLE)
	# add icon
	SET(ADM_EXE_SRCS ${ADM_EXE_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/xpm/avidemux.icns)
	SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/xpm/avidemux.icns PROPERTIES MACOSX_PACKAGE_LOCATION Resources)

	# bundle version info
	SET(ADM_OSX_VERSION "${VERSION}")

	IF (Subversion_FOUND)
		SET(ADM_OSX_VERSION "${ADM_OSX_VERSION} (r${ADM_SUBVERSION})")
	ENDIF (Subversion_FOUND)

	SET(MACOSX_BUNDLE_ICON_FILE avidemux.icns)
	SET(MACOSX_BUNDLE_INFO_STRING "Avidemux ${ADM_OSX_VERSION}")
	SET(MACOSX_BUNDLE_LONG_VERSION_STRING "${ADM_OSX_VERSION}") 
	SET(MACOSX_BUNDLE_SHORT_VERSION_STRING "${ADM_OSX_VERSION}")
	SET(MACOSX_BUNDLE_VERSION "${ADM_OSX_VERSION}")

	# include translation files in bundle
	# this isn't the cleanest approach but there's not much we can do until the po directory is under the avidemux directory
	FILE(GLOB PO_FILES ${CMAKE_SOURCE_DIR}/po/*.po)

	FOREACH(_file ${PO_FILES})
		GET_FILENAME_COMPONENT(_filename ${_file} NAME_WE)
		SET(_mofile "${CMAKE_BINARY_DIR}/po/${_filename}/avidemux.mo")

		SET(MO_FILES ${MO_FILES} ${_mofile})
		SET_SOURCE_FILES_PROPERTIES(${_mofile} PROPERTIES GENERATED TRUE MACOSX_PACKAGE_LOCATION "Resources/locale/${_filename}/LC_MESSAGES")
	ENDFOREACH(_file)

	FILE(GLOB TS_FILES ${CMAKE_SOURCE_DIR}/po/*.ts)

	FOREACH(_file ${TS_FILES})
		GET_FILENAME_COMPONENT(_filename ${_file} NAME_WE)
		SET(_qmfile "${CMAKE_BINARY_DIR}/po/${_filename}.qm")

		SET(QM_FILES ${QM_FILES} ${_qmfile})
		SET_SOURCE_FILES_PROPERTIES(${_qmfile} PROPERTIES GENERATED TRUE MACOSX_PACKAGE_LOCATION "Resources/locale")
	ENDFOREACH(_file)
ENDIF (APPLE)

IF (WIN32)
	# add icon and version info
	SET(FILEVERSION_STRING "${VERSION}.0.${ADM_SUBVERSION}")
	SET(PRODUCTVERSION_STRING "${VERSION}")
	STRING(REPLACE "." "," FILEVERSION ${FILEVERSION_STRING})
	STRING(REPLACE "." "," PRODUCTVERSION ${PRODUCTVERSION_STRING})
	SET(PRODUCTVERSION "${PRODUCTVERSION},0,0")

	CONFIGURE_FILE("${CMAKE_SOURCE_DIR}/cmake/admWin32.rc.in" ${CMAKE_CURRENT_BINARY_DIR}/admWin32.rc IMMEDIATE)
	ADD_CUSTOM_COMMAND(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/admWin32.obj COMMAND windres.exe -i ${CMAKE_CURRENT_BINARY_DIR}/admWin32.rc -o ${CMAKE_CURRENT_BINARY_DIR}/admWin32.obj -O coff --define VS_VERSION_INFO=1)

	SET(ADM_EXE_SRCS ${ADM_EXE_SRCS} "${CMAKE_CURRENT_BINARY_DIR}/admWin32.obj")
ENDIF (WIN32)

########################################
# Our exectuables
########################################
SET(ADM_EXE_SRCS ${ADM_EXE_SRCS} ADM_preview.cpp  gtk_gui.cpp  gui_autodrive.cpp  GUI_jobs.cpp  gui_navigate.cpp  guiplay.cpp  gui_savenew.cpp  main.cpp  gui_action.cpp)

ADD_EXECUTABLE(avidemux2_cli MACOSX_BUNDLE ${ADM_EXE_SRCS} ${MO_FILES})
ADD_TARGET_CFLAGS(avidemux2_cli "-I${CMAKE_BINARY_DIR}/config/cli")
ADD_DEPENDENCIES(avidemux2_cli mofiles)

IF (ADM_UI_GTK)
	ADD_EXECUTABLE(avidemux2_gtk MACOSX_BUNDLE ${ADM_EXE_SRCS} ${MO_FILES})
	ADD_TARGET_CFLAGS(avidemux2_gtk "-I${CMAKE_BINARY_DIR}/config/gtk")
	ADD_DEPENDENCIES(avidemux2_gtk mofiles)
ENDIF (ADM_UI_GTK)

IF (ADM_UI_QT4)
	ADD_EXECUTABLE(avidemux2_qt4 MACOSX_BUNDLE ${ADM_EXE_SRCS} ${QM_FILES})
	ADD_TARGET_CFLAGS(avidemux2_qt4 "-I${CMAKE_BINARY_DIR}/config/qt4")
	ADD_DEPENDENCIES(avidemux2_qt4 qmfiles)
ENDIF (ADM_UI_QT4)

SDLify(main.cpp)
ADD_SOURCE_CFLAGS(main.cpp "-DADM_SUBVERSION=${ADM_SUBVERSION}")

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

ADD_ADMLIB_ALL_TARGETS(${ADM_OUTPUTS})

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

ADD_ADMLIB_ALL_TARGETS(${ADM_INPUTS})

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

ADD_ADMLIB_ALL_TARGETS(${ADM_LIBRARIES})

IF (USE_FREETYPE)
	ADD_ADMLIB_ALL_TARGETS(ADM_libass)
ENDIF (USE_FREETYPE)

IF (ADM_DEBUG AND FIND_LEAKS)
	ADD_ADMLIB_ALL_TARGETS(ADM_nvwa)
ENDIF (ADM_DEBUG AND FIND_LEAKS)

IF (APPLE AND USE_SDL)
	TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_libsdl_gtk)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_libsdl_qt4)
ENDIF (APPLE AND USE_SDL)

###########################################
# Construct ADM_userInterface
###########################################
ADD_SUBDIRECTORY(ADM_userInterfaces)

TARGET_LINK_LIBRARIES(avidemux2_cli
	ADM_UI_None_cli
	ADM_commonUI_cli
	ADM_ocr_cli
	ADM_dialogNone_cli
	ADM_dialogFactoryNone_cli
	ADM_dialogFiltersNone_cli
	ADM_dialogGuiNone_cli)

IF (ADM_UI_GTK)
	TARGET_LINK_LIBRARIES(avidemux2_gtk
		ADM_UI_GTK_gtk
		ADM_commonUI_gtk
		ADM_ocr_gtk
		ADM_ocrGtk_gtk
		ADM_dialogGtk_gtk
		ADM_filtersGtk_gtk
		ADM_dialogFactoryGtk_gtk
		ADM_gui2Gtk_gtk
		ADM_toolkitGtk_gtk
		ADM_commonUI_gtk)
ENDIF (ADM_UI_GTK)

IF (ADM_UI_QT4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4
		ADM_UI_QT4_qt4
		ADM_commonUI_qt4
		ADM_ocr_qt4
		ADM_ocrQt4_qt4
		ADM_dialogQt4_qt4
		ADM_dialogFactoryQt4_qt4
		ADM_guiQt4_qt4
		ADM_filtersQt4_qt4
		ADM_commonUI_qt4)
ENDIF (ADM_UI_QT4)

ADD_ADMLIB_ALL_TARGETS(ADM_utilities)

###########################################
# ADM_core
###########################################
ADD_SUBDIRECTORY(ADM_core)
ADD_LIB_ALL_TARGETS(ADM_core)

###########################################
# External libs
###########################################
# gettext
IF (GETTEXT_FOUND)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${GETTEXT_LIBRARY_DIR})

	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${GETTEXT_LIBRARY_DIR})
	ENDIF (ADM_UI_GTK)
ENDIF (GETTEXT_FOUND)

# Freetype2
IF (USE_FREETYPE)
	ADD_LDFLAGS_ALL_TARGETS(${FREETYPE2_LDFLAGS})

	# libiconv
	ADD_LIB_ALL_TARGETS("${LIBICONV_LIBRARY_DIR}")
ENDIF (USE_FREETYPE)

# FontConfig
IF (USE_FONTCONFIG)
	ADD_LDFLAGS_ALL_TARGETS(${FONTCONFIG_LDFLAGS})
ENDIF (USE_FONTCONFIG)

# PNG
IF (USE_PNG)
	TARGET_LINK_LIBRARIES(avidemux2_cli ${PNG_LIBRARIES})
	
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${PNG_LIBRARIES})
	ENDIF (ADM_UI_GTK)
	
	IF (ADM_UI_QT4)
		IF (APPLE AND NOT QT_USE_FRAMEWORKS)
			TARGET_LINK_LIBRARIES(avidemux2_qt4 ${PNG_LIBRARIES})
		ELSEIF (NOT APPLE)
			TARGET_LINK_LIBRARIES(avidemux2_qt4 ${PNG_LIBRARIES})
		ENDIF (APPLE AND NOT QT_USE_FRAMEWORKS)
	ENDIF (ADM_UI_QT4)
ENDIF (USE_PNG)

# Libxml2
ADD_LIB_ALL_TARGETS("${LIBXML2_LIBRARIES}")

# XVideo
IF (USE_XV)
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${XVIDEO_LIBRARY_DIR})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${XVIDEO_LIBRARY_DIR})
	ENDIF (ADM_UI_QT4)
ENDIF (USE_XV)

# SDL
IF (USE_SDL)
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${SDL_LIBRARY})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${SDL_LIBRARY})
	ENDIF (ADM_UI_QT4)
ENDIF (USE_SDL)

# aRts
IF (USE_ARTS)
	IF (ADM_UI_GTK)
		ADD_TARGET_LDFLAGS(avidemux2_gtk ${ARTS_LDFLAGS})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		ADD_TARGET_LDFLAGS(avidemux2_qt4 ${ARTS_LDFLAGS})
	ENDIF (ADM_UI_QT4)
ENDIF (USE_ARTS)

# ALSA
IF (ALSA_FOUND)
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${ASOUND_LIBRARY})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${ASOUND_LIBRARY})
	ENDIF (ADM_UI_QT4)
ENDIF (ALSA_FOUND)

# ESD
IF (USE_ESD)
	IF (ADM_UI_GTK)
		TARGET_LINK_LIBRARIES(avidemux2_gtk ${ESD_LIBRARY_DIR})
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		TARGET_LINK_LIBRARIES(avidemux2_qt4 ${ESD_LIBRARY_DIR})
	ENDIF (ADM_UI_QT4)
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
IF (ADM_UI_GTK)
	ADD_TARGET_LDFLAGS(avidemux2_gtk ${GTK_LDFLAGS} ${GTHREAD_LDFLAGS})
ENDIF (ADM_UI_GTK)

IF (ADM_UI_QT4)
	TARGET_LINK_LIBRARIES(avidemux2_qt4 ${QT_QTGUI_LIBRARY} ${QT_QTCORE_LIBRARY})
ENDIF (ADM_UI_QT4)

###########################################
# OS Specific
###########################################
IF (WIN32)
	ADD_ADMLIB_ALL_TARGETS(ADM_osSupport)

	IF (MINGW)
		ADD_LIB_ALL_TARGETS(-mno-cygwin)
	ENDIF (MINGW)

	ADD_LIB_ALL_TARGETS(winmm)

	ADD_TARGET_LDFLAGS(avidemux2_cli "-Wl,-subsystem,console")

	IF (ADM_UI_GTK)
		ADD_TARGET_LDFLAGS(avidemux2_gtk "-mwindows")
	ENDIF (ADM_UI_GTK)

	IF (ADM_UI_QT4)
		ADD_TARGET_LDFLAGS(avidemux2_qt4 "-mwindows")
	ENDIF (ADM_UI_QT4)
ENDIF (WIN32)

IF (APPLE)
	ADD_LIB_ALL_TARGETS("-framework CoreServices -framework CoreAudio -framework AudioUnit -framework Carbon -framework Cocoa")

	# for Leopard but it doesn't hurt Tiger
	ADD_LIB_ALL_TARGETS("-Wl,-dylib_file,/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib:/System/Library/Frameworks/OpenGL.framework/Versions/A/Libraries/libGL.dylib")
ENDIF (APPLE)

###########################################
# Install
###########################################
SET(ADM_EXES avidemux2_cli)

IF (ADM_UI_GTK)
	SET(ADM_EXES ${ADM_EXES} avidemux2_gtk)
ENDIF (ADM_UI_GTK)

IF (ADM_UI_QT4)
	SET(ADM_EXES ${ADM_EXES} avidemux2_qt4)
ENDIF (ADM_UI_QT4)

INSTALL(TARGETS ${ADM_EXES} RUNTIME DESTINATION ${BIN_DIR} LIBRARY DESTINATION lib)
