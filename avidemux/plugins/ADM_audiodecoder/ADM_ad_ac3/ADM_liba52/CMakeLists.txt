SET(ADM_LIB ADM_liba52)

SET(${ADM_LIB}_SRCS 
	bit_allocate.c  bitstream.c  downmix.c  imdct.c  parse.c)

ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})

IF (UNIX)
	ADD_TARGET_CFLAGS(${ADM_LIB} -fPIC)
ENDIF (UNIX)

# FIXME
ADD_TARGET_CFLAGS(${ADM_LIB} "-I${CMAKE_BINARY_DIR}/config/cli")
ADD_TARGET_CFLAGS(${ADM_LIB} "-I${CMAKE_SOURCE_DIR}/avidemux/ADM_audiocodec")
ADD_TARGET_CFLAGS(${ADM_LIB} "-I${CMAKE_SOURCE_DIR}/avidemux/ADM_core/include")