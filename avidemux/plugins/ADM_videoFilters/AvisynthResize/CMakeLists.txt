INCLUDE(vf_plugin)


SET(avisynthResizeCommon_SRCS 
ADM_resizebis.cpp  ADM_resizeter.cpp  ADM_vidResize.cpp
)

INCLUDE(vf_plugin_gtk)
INCLUDE(vf_plugin_qt4)

SET(resizeCommonGtk_SRCS gtk/gtk_resize.cpp)
SET(resizeCommonQT_SRCS Q_resize.cpp)
SET(resizeCommonQT_UI resizing)

INIT_VIDEOFILTER_PLUGIN_QT4(ADM_vf_avisynthResize_qt4 
                ${resizeCommonQT_SRCS} ${resizeCommonQT_UI} ${avisynthResizeCommon_SRCS})
INIT_VIDEOFILTER_PLUGIN_GTK(  ADM_vf__avisynthResize_gtk 
                ${resizeCommonGtk_SRCS} ${avisynthResizeCommon_SRCS})

