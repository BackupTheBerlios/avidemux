################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_lavutil)
#
SET(${ADM_LIB}_SRCS 
integer.c  intfloat_readwrite.c  mathematics.c  rational.c md5.c crc.c log.c  adler32.c lls.c random.c
string.c

)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)

################################
add_definitions(-DHAVE_AV_CONFIG_H)
