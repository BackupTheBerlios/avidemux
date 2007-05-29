################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_filtersQt4)
#
SET(${ADM_LIB}_SRCS
AQ_mainfilter.cpp  filter_none.cpp  filter_rsc.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. ../../../ADM_osSupport ../../../ADM_inputs)
include_directories(${QT_INCLUDES})

