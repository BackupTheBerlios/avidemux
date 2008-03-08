SET(ADM_LIB ADM_libMad)

SET(${ADM_LIB}_SRCS 
	bit.c  decoder.c  fixed.c  frame.c  huffman.c  layer12.c  layer3.c  
	stream.c  synth.c  timer.c  version.c)

ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
add_definitions(-fPIC)
