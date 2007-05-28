################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_gui2Gtk)
#
SET(${ADM_LIB}_SRCS
GUI_bindings.cpp  GUI_gtkRender.cpp  GUI_keymap.cpp  GUI_main2.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(${GTK_CFLAGS} -DADM_SUBVERSION=0 )
include_directories(.. ../..)
#/../ADM_osSupport ../../../ADM_inputs)
