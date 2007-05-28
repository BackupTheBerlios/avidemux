################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogFactoryNone)
#
SET(${ADM_LIB}_SRCS 
DIA_dialogFactory.cpp  FAC_toggle.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
include_directories(../..)
