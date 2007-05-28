################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libwrapper)
#
SET(${ADM_LIB}_SRCS 
libwrapper_amrnb.cpp  libwrapper.cpp  libwrapper_dca.cpp  libwrapper_global.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)

################################
