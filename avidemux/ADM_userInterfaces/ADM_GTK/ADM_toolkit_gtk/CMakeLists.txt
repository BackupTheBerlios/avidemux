################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_toolkitGtk)
#
SET(${ADM_LIB}_SRCS
ADM_gladeSupport.cpp  ADM_tray_gtk.cpp  choice.cpp  TLK_filesel.cpp  toolkit.cpp  toolkit_dialog.cpp 
                                        eggtrayicon.c eggtray_win32.c gtkmarkscale.c jogshuttle.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(${GTK_CFLAGS})
include_directories(.. ../../../ADM_osSupport)
