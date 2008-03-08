##
##
##

subdirs(PREORDER ADM_liba52)



include_directories(../../)
include_directories(../../../ADM_audiocodec)
include_directories(ADM_liba52)

SET(ADM_ad_a52_SRCS         ADM_ad_a52.cpp )

ADD_LIBRARY(ADM_ad_a52 SHARED ${ADM_ad_a52_SRCS})
TARGET_LINK_LIBRARIES(ADM_ad_a52 ADM_liba52)

