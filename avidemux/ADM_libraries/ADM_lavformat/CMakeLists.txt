################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_lavformat)
#
SET(${ADM_LIB}_SRCS 
 mpeg.c utils.c avio.c ADM_lavformat.cpp 
aviobuf.c cutils.c img2.c file.c mpegtsenc.c ADM_muxts.cpp movenc.c 
avienc.c wav.c mov.c matroska.c isom.c isom.h riff.c
flvenc.c matroskaenc.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)
INCLUDE_DIRECTORIES(../ADM_lavcodec .. ../ADM_lavutil)

################################
add_definitions(-DHAVE_AV_CONFIG_H)
