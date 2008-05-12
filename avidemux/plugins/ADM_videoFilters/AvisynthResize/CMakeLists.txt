INCLUDE(vf_plugin)


SET(ADM_vf_avsResize_SRCS 
ADM_resizebis.cpp    
ADM_resizeter.cpp    
ADM_vidResize.cpp
)

ADD_LIBRARY(ADM_vf_avsResize SHARED ${ADM_vf_avsResize_SRCS})

INIT_VIDEOFILTER_PLUGIN(ADM_vf_avsResize)
INSTALL_VIDEOFILTER(ADM_vf_avsResize)
