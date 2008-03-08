##
##
##

include_directories(../../)
include_directories(../../../ADM_audiocodec)

SET(ADM_ad_faad_SRCS         ADM_ad_faad.cpp )

ADD_LIBRARY(ADM_ad_faad SHARED ${ADM_ad_faad_SRCS})

