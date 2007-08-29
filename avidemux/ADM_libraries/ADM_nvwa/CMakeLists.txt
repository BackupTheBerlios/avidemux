################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_nvwa)
#
SET(${ADM_LIB}_SRCS 
debug_new.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
ADD_DEFINITIONS(-D_DEBUG_NEW_ERROR_ACTION=NULL)

################################
