INCLUDE(vf_plugin)


SET(resizeCommon_SRCS ADM_vidMPLResize.cpp)

SET(resizeCommonGtk_SRCS gtk/gtk_resize.cpp)
SET(resizeCommonQT_SRCS Q_resize.cpp)
SET(resizeCommonQT_UI resizing)

INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../")


INCLUDE(vf_plugin_gtk)
INCLUDE(vf_plugin_qt4)


INIT_VIDEOFILTER_PLUGIN_GTK(  ADM_vf_mplayerResize_gtk ${resizeCommonGtk_SRCS} ${resizeCommon_SRCS} )
# We need some additionnal libs
IF (GTK_FOUND AND GTHREAD_FOUND)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libswscale)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libavcodec)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libavutil)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_UIGtk)
ENDIF (GTK_FOUND AND GTHREAD_FOUND)        

INIT_VIDEOFILTER_PLUGIN_QT4(ADM_vf_mplayerResize_qt4  ${resizeCommonQT_SRCS} ${resizeCommonQT_UI} ${resizeCommon_SRCS} )
IF (QT4_FOUND )
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libswscale)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libavcodec)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_libavutil)
        TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize_gtk ADM_UIGtk)
ENDIF (QT4_FOUND )
