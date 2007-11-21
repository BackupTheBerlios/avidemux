SET(ADM_LIB ADM_guiQt4)
SET(${ADM_LIB}_SRCS avidemux_rsc.cpp  file_qt4.cpp  gui_none.cpp  ADM_qslider.cpp)
SET(${ADM_LIB}_T preview)
SET(${ADM_LIB}_Q gui2)

Q_RCC(avidemux)

INCLUDE(adm_qt4)