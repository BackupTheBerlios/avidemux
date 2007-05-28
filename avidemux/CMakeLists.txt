##########################################################
#
#  Main CMake makefile for avidemux2
#  It is not for the faint of heart and pretty ugly
#
##########################################################

INCLUDE(Po)
MESSAGE("GTK:"${GTK_CFLAGS}"GLIB:"${GLIB_CFLAGS})

SET(adm_EXE_SRCS ADM_preview.cpp  gtk_gui.cpp  gui_autodrive.cpp  GUI_jobs.cpp  gui_navigate.cpp  guiplay.cpp  gui_savenew.cpp  main.cpp )
COMPILE_PO_FILES(${CMAKE_SOURCE_DIR}/po adm_EXE_SRCS)
include_directories(ADM_userInterfaces/ADM_commonUI ADM_outputs)

# Small macro...
MACRO (add_libs_all_targets _libName)
  TARGET_LINK_LIBRARIES(avidemux2_cli ${_libName})
  if(GTK_FOUND)
  TARGET_LINK_LIBRARIES(avidemux2_gtk ${_libName})
  endif(GTK_FOUND)
ENDMACRO (add_libs_all_targets _libName)
#/ Small macro...


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

ADM_outputs
ADM_inputs
ADM_encoder
ADM_codecs
ADM_requant 
ADM_toolkit  
ADM_osSupport  
)


########################################################
# Our exectuables
########################################################
ADD_EXECUTABLE(avidemux2_cli ${adm_EXE_SRCS} )
if(GTK_FOUND)
ADD_EXECUTABLE(avidemux2_gtk ${adm_EXE_SRCS} )
endif(GTK_FOUND)

add_definitions(${GLIB_CFLAGS})
###########################################
# Build Regular Subdirs
###########################################
FOREACH(_current ${ADM_SUBDIR})
subdirs (PREORDER ${_current})
add_libs_all_targets(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})

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
if(FT_FOUND)
SET(ADM_LIBRARIES ${ADM_LIBRARIES}  ADM_libass)
ENDIF(FT_FOUND)

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

###########################################
# Add external libs
###########################################
TARGET_LINK_LIBRARIES(avidemux2_cli ${GLIB_LDFLAGS} )
LINK_DIRECTORIES(avidemux2_cli ${GLIB_LIB}  )

#fontconfig
if(USE_FONTCONFIG)
add_libs_all_targets(fontconfig)
endif(USE_FONTCONFIG)

# Xv
if(USE_XV)
add_libs_all_targets( Xext)
add_libs_all_targets( X11 )
add_libs_all_targets( Xv )
add_libs_all_targets( m)
endif(USE_XV)

# Lame
if(HAVE_LIBMP3LAME)
add_libs_all_targets(mp3lame )
endif(HAVE_LIBMP3LAME)

# SDL
if(USE_SDL)
add_libs_all_targets(${SDL_LIBRARY} )
endif(USE_SDL)


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

#faad
if(USE_FAAD)
add_libs_all_targets(faad)
endif(USE_FAAD)

#freetype
if(USE_FREETYPE)
add_libs_all_targets(${FT_LDFLAGS})
endif(USE_FREETYPE)

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


# /EOF


