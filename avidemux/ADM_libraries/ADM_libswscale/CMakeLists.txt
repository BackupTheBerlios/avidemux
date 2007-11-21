SET(ADM_LIB ADM_libswscale)

SET(${ADM_LIB}_SRCS 
	swscale.c yuv2rgb.c rgb2rgb.c yuv2rgb_altivec.c ADM_mp.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DCONFIG_GPL -fomit-frame-pointer")

IF (ADM_OS_DARWIN)
	ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DPIC")
	
	IF (ADM_DEBUG)
		ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-O2")
	ENDIF (ADM_DEBUG)
ENDIF (ADM_OS_DARWIN)