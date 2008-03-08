##
##
##

include_directories(../../)
include_directories(../../../ADM_audiocodec)

SET(ADM_ad_vorbis_SRCS         ADM_ad_vorbis.cpp )

ADD_LIBRARY(ADM_ad_vorbis SHARED ${ADM_ad_vorbis_SRCS})

