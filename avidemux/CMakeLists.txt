##########################################################
#
#  Main CMake makefile for avidemux2
#  It is not for the faint of heart and pretty ugly
#
##########################################################

INCLUDE(Po)

SET(adm_EXE_SRCS ADM_preview.cpp  gtk_gui.cpp  gui_autodrive.cpp  GUI_jobs.cpp  gui_navigate.cpp  guiplay.cpp  gui_savenew.cpp  main.cpp )
if(NOT WIN32)
COMPILE_PO_FILES(${CMAKE_SOURCE_DIR}/po adm_EXE_SRCS)
endif(NOT WIN32)

include_directories(ADM_userInterfaces/ADM_commonUI ADM_outputs)

# Small macro...
MACRO (add_libs_all_targets _libName)
  TARGET_LINK_LIBRARIES(avidemux2_cli ${_libName})
  if(GTK_FOUND)
  TARGET_LINK_LIBRARIES(avidemux2_gtk ${_libName})
  endif(GTK_FOUND)
  if(QT4_FOUND)
  TARGET_LINK_LIBRARIES(avidemux2_qt4 ${_libName})
  endif(QT4_FOUND)
ENDMACRO (add_libs_all_targets _libName)

MACRO (change_ldflags_all_targets _libName)
        SET_TARGET_PROPERTIES(avidemux2_cli
	PROPERTIES LINK_FLAGS ${_libName})
  if(GTK_FOUND)
        SET_TARGET_PROPERTIES(avidemux2_gtk
	PROPERTIES LINK_FLAGS ${_libName})
  endif(GTK_FOUND)
  if(QT4_FOUND)
        SET_TARGET_PROPERTIES(avidemux2_qt4
	PROPERTIES LINK_FLAGS ${_libName})
  endif(QT4_FOUND)
ENDMACRO (change_ldflags_all_targets _libName)
##/ Small macro...


#
# Normal subdirs, compile and link what is inside....
#
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
ADM_colorspace 
ADM_audiodevice
ADM_ocr  
)


########################################################
# Our exectuables
########################################################
ADD_EXECUTABLE(avidemux2_cli ${adm_EXE_SRCS} )
if(GTK_FOUND)
ADD_EXECUTABLE(avidemux2_gtk ${adm_EXE_SRCS} )
endif(GTK_FOUND)
if(QT4_FOUND)
ADD_EXECUTABLE(avidemux2_qt4 ${adm_EXE_SRCS} )
endif(QT4_FOUND)


add_definitions(${GLIB_CFLAGS})
###########################################
# Build Regular Subdirs
###########################################
FOREACH(_current ${ADM_SUBDIR})
subdirs (PREORDER ${_current})
add_libs_all_targets(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})
# build trail
subdirs (PREORDER ADM_osSupport)
subdirs (PREORDER ADM_encoder)
subdirs (PREORDER ADM_toolkit)
subdirs (PREORDER ADM_codecs)
subdirs (PREORDER ADM_requant)
subdirs (PREORDER ADM_infoExtractor)
###########################################
# Construct ADM_outputs
###########################################
subdirs (PREORDER ADM_outputs)
LINK_DIRECTORIES(ADM_outputs)
SET(ADM_outputs
oplug_avi
oplug_mp4
oplug_mpeg
oplug_mpegFF
oplug_ogm
)

FOREACH(_current ${ADM_outputs})
add_libs_all_targets(${_current})
ENDFOREACH(_current ${ADM_outputs})
###########################################
# Construct ADM_inputs
###########################################
subdirs (PREORDER ADM_inputs)
LINK_DIRECTORIES(ADM_inputs)
SET(ADM_inputs
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
)

FOREACH(_current ${ADM_inputs})
add_libs_all_targets(${_current})
ENDFOREACH(_current ${ADM_inputs})

#########################################
# Tails
#########################################


add_libs_all_targets(ADM_toolkit)
add_libs_all_targets(ADM_osSupport)
add_libs_all_targets(ADM_encoder)
add_libs_all_targets(ADM_codecs)
add_libs_all_targets(ADM_requant)
add_libs_all_targets(ADM_infoExtractor)

###########################################
# Construct ADM_libraries
###########################################
subdirs (PREORDER ADM_libraries)
LINK_DIRECTORIES(ADM_libraries)
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
)
if(USE_FREETYPE)
SET(ADM_LIBRARIES ${ADM_LIBRARIES}  ADM_libass)
ENDIF(USE_FREETYPE)

FOREACH(_current ${ADM_LIBRARIES})
add_libs_all_targets(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})

###########################################
# Construct ADM_userInterface
###########################################
subdirs (PREORDER ADM_userInterfaces)

LINK_DIRECTORIES(ADM_userInterfaces)
LINK_DIRECTORIES(ADM_userInterfaces/ADM_NONE)


TARGET_LINK_LIBRARIES(avidemux2_cli ADM_commonUI )
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogNone )
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogFactoryNone )
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogFiltersNone )
TARGET_LINK_LIBRARIES(avidemux2_cli ADM_dialogGuiNone )

if(GTK_FOUND)
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_ocrGtk )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_ocr )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_commonUI )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_dialogGtk )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_filtersGtk )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_dialogFactoryGtk )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_gui2Gtk )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_toolkitGtk )
TARGET_LINK_LIBRARIES(avidemux2_gtk ADM_commonUI )
endif(GTK_FOUND)

if(QT4_FOUND)
TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_ocr )
TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_commonUI )
TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_dialogQt4 )

TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_dialogFactoryQt4 )
TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_guiQt4 )
TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_filtersQt4 )
TARGET_LINK_LIBRARIES(avidemux2_qt4 ADM_commonUI )
endif(QT4_FOUND)

###########################################
# Add external libs
###########################################
TARGET_LINK_LIBRARIES(avidemux2_cli ${GLIB_LDFLAGS} )
LINK_DIRECTORIES(avidemux2_cli ${GLIB_LIB}  )
# ARTS
if(USE_ARTS)
add_libs_all_targets(artsc)
endif(USE_ARTS)
# libAMRNB
if(USE_AMR_NB)
add_libs_all_targets(amrnb)
endif(USE_AMR_NB)
#libdts
if(USE_LIBDCA)
add_libs_all_targets(dts)
endif(USE_LIBDCA)
# xvid
if(USE_XVID_4)
add_libs_all_targets(xvidcore)
endif(USE_XVID_4)

#alsa
if(ALSA_FOUND)
add_libs_all_targets( ${ASOUND_LIBRARY} )
endif(ALSA_FOUND)
#fontconfig
if(USE_FONTCONFIG)
add_libs_all_targets(fontconfig)
endif(USE_FONTCONFIG)
##########################
# XVideo 
##########################
if(USE_XV)
       TARGET_LINK_LIBRARIES(avidemux2_cli Xext)
       TARGET_LINK_LIBRARIES(avidemux2_cli X11)
       TARGET_LINK_LIBRARIES(avidemux2_cli Xv)
       TARGET_LINK_LIBRARIES(avidemux2_cli m)
       if(GTK_FOUND)
               TARGET_LINK_LIBRARIES(avidemux2_gtk Xext)
               TARGET_LINK_LIBRARIES(avidemux2_gtk X11)
               TARGET_LINK_LIBRARIES(avidemux2_gtk Xv)
               TARGET_LINK_LIBRARIES(avidemux2_gtk m)
       endif(GTK_FOUND)
       if(QT4_FOUND AND NOT CMAKE_OSX_SYSROOT)
               TARGET_LINK_LIBRARIES(avidemux2_qt4 Xext)
               TARGET_LINK_LIBRARIES(avidemux2_qt4 X11 )
               TARGET_LINK_LIBRARIES(avidemux2_qt4 Xv )
               TARGET_LINK_LIBRARIES(avidemux2_qt4 m)
       endif(QT4_FOUND AND NOT CMAKE_OSX_SYSROOT)
endif(USE_XV)

# Lame
if(HAVE_LIBMP3LAME)
add_libs_all_targets(mp3lame )
endif(HAVE_LIBMP3LAME)

# SDL
if(USE_SDL)
add_libs_all_targets(${SDL_LDFLAGS} )
endif(USE_SDL)

# ESD
if(USE_ESD)
add_libs_all_targets(esd )
endif(USE_ESD)


# x264
if(USE_X264)
add_libs_all_targets(x264 )
endif(USE_X264)

# png
if(USE_PNG)
add_libs_all_targets(png )
endif(USE_PNG)

# Vorbis
if(USE_VORBIS)
add_libs_all_targets(vorbis )
add_libs_all_targets(vorbisenc )
endif(USE_VORBIS)

#faac
if(USE_FAAC)
add_libs_all_targets(faac)
endif(USE_FAAC)

#aften
if(USE_AFTEN)
add_libs_all_targets(aften)
endif(USE_AFTEN)


#faad
if(USE_FAAD)
add_libs_all_targets(faad)
endif(USE_FAAD)

#freetype
if(USE_FREETYPE)
add_libs_all_targets(${FT_LDFLAGS})
endif(USE_FREETYPE)

#gettext
if(ADM_GETTEXT_LIB)
add_libs_all_targets(${ADM_GETTEXT_LIB})
endif(ADM_GETTEXT_LIB)

#iconv
if(NEEED_LIBICONV)
add_libs_all_targets(iconv)
endif(NEEED_LIBICONV)

# Xml2
add_libs_all_targets(xml2 )
###########################################
# Specific
###########################################
if(GTK_FOUND)
TARGET_LINK_LIBRARIES(avidemux2_gtk ${GLIB_LDFLAGS} )
LINK_DIRECTORIES(avidemux2_gtk ${GLIB_LIB}  )
TARGET_LINK_LIBRARIES(avidemux2_gtk ${GTK_LDFLAGS} )
TARGET_LINK_LIBRARIES(avidemux2_gtk ${GTHREAD_LDFLAGS} ) # FIXME
endif(GTK_FOUND)

if(QT4_FOUND)
TARGET_LINK_LIBRARIES(avidemux2_qt4 ${GLIB_LDFLAGS} )
LINK_DIRECTORIES(avidemux2_qt4 ${GLIB_LIB}  )
TARGET_LINK_LIBRARIES(avidemux2_qt4 ${QT_QTGUI_LIBRARY} ) # FIXME
TARGET_LINK_LIBRARIES(avidemux2_qt4 ${QT_QTCORE_LIBRARY} ) # FIXME
endif(QT4_FOUND)


########################
# Win32
########################
IF(WIN32)
add_libs_all_targets(z)
add_libs_all_targets(ADM_osSupport)
add_libs_all_targets(pthreadGC2 )
add_libs_all_targets(winmm )
add_libs_all_targets(wsock32 )

change_ldflags_all_targets( "-mms-bitfield -mno-cygwin -mwinmm -mwindows xpm/adm.o")
ENDIF(WIN32)
########################
# MacOsX
########################
if(CMAKE_OSX_SYSROOT)
add_libs_all_targets(z)
add_libs_all_targets( "-framework CoreServices  -framework CoreAudio   -framework AudioUnit")
if(GTK_FOUND)
LINK_DIRECTORIES(avidemux2_gtk /usr/X11R6/lib )
endif(GTK_FOUND)

endif(CMAKE_OSX_SYSROOT)
########################
# all
########################
SDLify(main.cpp )
#FIXME : Set correct prefix for linguas
if(HAVE_GETTEXT)
    SET_SOURCE_FILES_PROPERTIES(main.cpp
                                     PROPERTIES COMPILE_FLAGS -DADMLOCALE="\\"${CMAKE_INSTALL_PREFIX}/share/locale\\"") 
endif(HAVE_GETTEXT)
###############
# INSTALL
###############
SET(ADM_EXES avidemux2_cli)
if(GTK_FOUND)
  SET(ADM_EXES ${ADM_EXES} avidemux2_gtk)
endif(GTK_FOUND)
if(QT4_FOUND)
  SET(ADM_EXES ${ADM_EXES} avidemux2_qt4)
endif(QT4_FOUND)

INSTALL(TARGETS ${ADM_EXES}
            RUNTIME DESTINATION bin
            LIBRARY DESTINATION lib)
# /EOF


