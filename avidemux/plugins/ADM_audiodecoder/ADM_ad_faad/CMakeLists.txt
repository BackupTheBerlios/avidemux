SET(ADM_ad_faad_SRCS ADM_ad_faad.cpp )

ADD_LIBRARY(ADM_ad_faad SHARED ${ADM_ad_faad_SRCS})

# FIXME
TARGET_LINK_LIBRARIES(ADM_ad_faad  ${FAAD_LIBRARY_DIR})
INCLUDE(ad_plugin)
ADM_audio_plugin(ADM_ad_faad)