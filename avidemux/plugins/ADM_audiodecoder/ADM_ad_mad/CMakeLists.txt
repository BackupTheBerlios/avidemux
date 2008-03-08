##
##
##

subdirs(PREORDER ADM_libMad)



include_directories(../../)
include_directories(../../../ADM_audiocodec)
include_directories(ADM_libMad)

SET(ADM_ad_Mad_SRCS         ADM_ad_mad.cpp )

ADD_LIBRARY(ADM_ad_Mad SHARED ${ADM_ad_Mad_SRCS})
TARGET_LINK_LIBRARIES(ADM_ad_Mad ADM_libMad)

