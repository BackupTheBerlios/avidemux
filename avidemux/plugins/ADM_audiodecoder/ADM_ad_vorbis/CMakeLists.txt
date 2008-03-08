SET(ADM_ad_vorbis_SRCS ADM_ad_vorbis.cpp )

ADD_LIBRARY(ADM_ad_vorbis SHARED ${ADM_ad_vorbis_SRCS})

# FIXME
ADD_TARGET_CFLAGS(ADM_ad_vorbis "-I${CMAKE_BINARY_DIR}/config/cli")
ADD_TARGET_CFLAGS(ADM_ad_vorbis "-I${CMAKE_SOURCE_DIR}/avidemux/ADM_audiocodec")