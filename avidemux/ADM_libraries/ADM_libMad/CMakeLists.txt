
SET(ADM_libMad_SRCS 
bit.c  decoder.c  fixed.c  frame.c  huffman.c  layer12.c  layer3.c  
#minimad.c  i
stream.c  synth.c  timer.c  version.c
)
ADD_LIBRARY(ADM_libMad STATIC ${ADM_libMad_SRCS})

################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libMad)
#
SET(${ADM_LIB}_SRCS 
bit.c  decoder.c  fixed.c  frame.c  huffman.c  layer12.c  layer3.c  
#minimad.c  i
stream.c  synth.c  timer.c  version.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
