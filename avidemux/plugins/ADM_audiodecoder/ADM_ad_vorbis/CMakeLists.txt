SET(ADM_ad_vorbis_SRCS ADM_ad_vorbis.cpp )

ADD_LIBRARY(ADM_ad_vorbis SHARED ${ADM_ad_vorbis_SRCS})

# FIXME
TARGET_LINK_LIBRARIES(ADM_ad_vorbis  ${VORBIS_LIBRARY_DIR})
TARGET_LINK_LIBRARIES(ADM_ad_vorbis  ${VORBISENC_LIBRARY_DIR})
INCLUDE(ad_plugin)
ADM_audio_plugin(ADM_ad_vorbis)
