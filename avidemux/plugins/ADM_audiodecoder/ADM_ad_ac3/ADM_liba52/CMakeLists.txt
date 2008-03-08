SET(ADM_LIB ADM_liba52)

SET(${ADM_LIB}_SRCS 
	bit_allocate.c  bitstream.c  downmix.c  imdct.c  parse.c)

ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
add_definitions(-fPIC)