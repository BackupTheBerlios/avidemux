################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_guiQt4)
#
SET(${ADM_LIB}_SRCS
avidemux_rsc.cpp  file_qt4.cpp  gui_none.cpp
)
#
#
SET(${ADM_LIB}_T
preview
qslider
)
#
SET(${ADM_LIB}_Q
gui2
)

#########################################"
include(adm_qt4)
################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. .)
include_directories(${QT_INCLUDES})

