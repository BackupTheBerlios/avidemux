
################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogFiltersNone)
#
SET(${ADM_LIB}_SRCS 
filter_none
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
include_directories(../..)
include_directories(../ADM_commonUI)
