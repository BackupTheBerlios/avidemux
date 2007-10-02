################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libswscale)
#
SET(${ADM_LIB}_SRCS 
swscale.c yuv2rgb.c rgb2rgb.c yuv2rgb_altivec.c ADM_mp.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
ADD_DEFINITIONS(-DCONFIG_GPL -fomit-frame-pointer)

IF (ADM_OS_APPLE)
	ADD_DEFINITIONS(-DPIC)
ENDIF (ADM_OS_APPLE)

################################
