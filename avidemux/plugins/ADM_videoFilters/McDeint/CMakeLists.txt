INCLUDE(vf_plugin)

#INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../ADM_libraries/ADM_ffmpeg/ADM_libpostproc")
INCLUDE_DIRECTORIES("${CMAKE_CURRENT_SOURCE_DIR}/../../../")
SET(ADM_vf_mcdeint_SRCS ADM_vidMcDeint.cpp)

ADD_LIBRARY(ADM_vf_mcdeint SHARED ${ADM_vf_mcdeint_SRCS})

INIT_VIDEOFILTER_PLUGIN(ADM_vf_mcdeint)
INSTALL_VIDEOFILTER(ADM_vf_mcdeint)
