################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogGuiNone)
#
SET(${ADM_LIB}_SRCS 
file_none.cpp  gui_none.cpp  preview_none.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
#include_directories(../..)
