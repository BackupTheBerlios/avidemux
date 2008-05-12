INCLUDE(vf_plugin)


SET(ADM_vf_lavDeinterlace_SRCS ADM_lavpp_deint.cpp)
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../")
ADD_LIBRARY(ADM_vf_lavDeinterlace SHARED ${ADM_vf_lavDeinterlace_SRCS})
TARGET_LINK_LIBRARIES(ADM_vf_lavDeinterlace ADM_libswscale)

INIT_VIDEOFILTER_PLUGIN(ADM_vf_lavDeinterlace)
INSTALL_VIDEOFILTER(ADM_vf_lavDeinterlace)
