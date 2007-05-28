################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libpostproc)
#
SET(${ADM_LIB}_SRCS 
postprocess.c
)

ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
add_definitions(-I.. -DHAVE_AV_CONFIG_H  -O3 -I../.. -I../ADM_lavutil -Drestrict=__restrict__ -fomit-frame-pointer -msse )

