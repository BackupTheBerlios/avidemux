################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_guiQt4)
#
SET(${ADM_LIB}_SRCS
AQ_gui2.cpp  AT_preview.cpp  AT_qslider.cpp  avidemux_rsc.cpp  file_qt4.cpp  gui_none.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. .)
include_directories(${QT_INCLUDES})

