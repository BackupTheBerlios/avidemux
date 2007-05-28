
SET(ADM_nuv_SRCS 
ADM_nuvAudio.cpp  ADM_nuv.cpp  minilzo.c  RTjpeg.c  RTjpegN.cpp  RToldformat.cpp)

ADD_LIBRARY(ADM_nuv STATIC ${ADM_nuv_SRCS})
include_directories(..)
include_directories(../../ADM_userInterfaces/ADM_commonUI)
