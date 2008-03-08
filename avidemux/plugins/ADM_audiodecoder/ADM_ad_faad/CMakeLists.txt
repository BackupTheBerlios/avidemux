SET(ADM_ad_faad_SRCS ADM_ad_faad.cpp )

ADD_LIBRARY(ADM_ad_faad SHARED ${ADM_ad_faad_SRCS})

# FIXME
ADD_TARGET_CFLAGS(ADM_ad_faad "-I${CMAKE_BINARY_DIR}/config/cli")
ADD_TARGET_CFLAGS(ADM_ad_faad "-I${CMAKE_SOURCE_DIR}/avidemux/ADM_audiocodec")