INCLUDE(vf_plugin)


SET(ADM_vf_mplayerResize_SRCS ADM_vidMPLResize.cpp)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../")



ADD_LIBRARY(ADM_vf_mplayerResize SHARED ${ADM_vf_mplayerResize_SRCS})
TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize ADM_libswscale)
TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize ADM_libavcodec)
TARGET_LINK_LIBRARIES(ADM_vf_mplayerResize ADM_libavutil)

INIT_VIDEOFILTER_PLUGIN(ADM_vf_mplayerResize)
INSTALL_VIDEOFILTER(ADM_vf_mplayerResize)
