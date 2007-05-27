INCLUDE(Po)
MESSAGE("GTK:"${GTK_CFLAGS}"GLIB:"${GLIB_CFLAGS})

SET(adm_EXE_SRCS ADM_preview.cpp  gtk_gui.cpp  gui_autodrive.cpp  GUI_jobs.cpp  gui_navigate.cpp  guiplay.cpp  gui_savenew.cpp  main.cpp )
COMPILE_PO_FILES(${CMAKE_SOURCE_DIR}/po adm_EXE_SRCS)



SET(ADM_SUBDIR ADM_audiocodec ADM_audiodevice ADM_audiofilter ADM_codecs ADM_colorspace ADM_editor ADM_encoder ADM_filter   ADM_ocr ADM_osSupport  ADM_requant ADM_script ADM_toolkit  ADM_video ADM_videoFilter)
#ADM_inputs ADM_outputs

ADD_EXECUTABLE(avidemux2_cli ${adm_EXE_SRCS} )

########################################################
# Custom Rule to force evaluation if *INPUT* SUBDIR*
########################################################

# add_subdirectory(ADM_inputs)
# SET(ADM_INPUT_SUBDIR 
# ADM_asf
# ADM_avsproxy
# ADM_h263
# ADM_inpics
# ADM_matroska
# ADM_mp4
# ADM_mpegdemuxer
# ADM_nuv
# ADM_ogm
# ADM_openDML
# )
# FOREACH(_current ${ADM_INPUT_SUBDIR})
# 
# ADD_CUSTOM_TARGET(  ADM_buildInput ALL
#                     COMMAND make
#                     DEPENDS 
#                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/avidemux/ADM_inputs
#                     COMMENT "Building inputs")
# 
# ENDFOREACH(_current ${ADM_SUBDIR})
# 
# add_subdirectory(ADM_userInterfaces)
########################################################
# Custom Rule to force evaluation if *CommonUI*
########################################################
# add_subdirectory(ADM_userInterfaces/ADM_commonUI)
# 
# 
# add_subdirectory(ADM_userInterfaces/ADM_commonUI)
# ADD_CUSTOM_TARGET(  ADM_buildInput ALL
#                     COMMAND make
#                     DEPENDS 
#                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/avidemux/ADM_userInterfaces/ADM_commonUI
#                     COMMENT "Building commonUI")

########################################################
# Custom Rule to force evaluation if *None UI*
########################################################

# add_subdirectory(ADM_userInterfaces/ADM_NONE)
# SET(ADM_NONE_SUBDIR 
# ADM_dialog
# ADM_dialogFactory
# ADM_filters
# ADM_gui2
# )
# FOREACH(_current ${ADM_NONE_SUBDIR})
# 
# ADD_CUSTOM_TARGET(  ADM_buildInput ALL
#                     COMMAND make
#                     DEPENDS 
#                     WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/avidemux/ADM_userInterfaces/ADM_NONE/${_current}
#                     COMMENT "Building NONE")
# 
# ENDFOREACH(_current ${ADM_NONE_SUBDIR})


# ** User interfaces **

add_definitions(${GLIB_CFLAGS})

include_directories(ADM_userInterfaces/ADM_commonUI)
include_directories(ADM_inputs)
include_directories(ADM_outputs)

FOREACH(_current ${ADM_SUBDIR})
add_subdirectory(${_current})
ENDFOREACH(_current ${ADM_SUBDIR})



FOREACH(_current ${ADM_SUBDIR})
TARGET_LINK_LIBRARIES(avidemux2_cli ${_current})
ENDFOREACH(_current ${ADM_SUBDIR})




