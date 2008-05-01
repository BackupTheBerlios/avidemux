INCLUDE(vf_plugin)


SET(ADM_vf_blendRemoval_SRCS ADM_vidBlendRemoval.cpp)

ADD_LIBRARY(ADM_vf_blendRemoval SHARED ${ADM_vf_blendRemoval_SRCS})

INIT_VIDEOFILTER_PLUGIN(ADM_vf_blendRemoval)
INSTALL_VIDEOFILTER(ADM_vf_blendRemoval)
