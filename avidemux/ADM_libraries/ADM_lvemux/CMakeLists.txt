################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_lvemux)
#
SET(${ADM_LIB}_SRCS 
ADM_muxer.cpp
mux_out.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################

