SET(ADM_LIB ADM_libsamplerate)

SET(${ADM_LIB}_SRCS 
src_linear.c
src_sinc.c
src_zoh.c
)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-I${CMAKE_CURRENT_SOURCE_DIR} ")

