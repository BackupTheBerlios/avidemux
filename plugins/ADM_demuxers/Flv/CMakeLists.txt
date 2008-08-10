include(dm_plugin)
SET(ADM_flv_SRCS 
	ADM_flv.cpp
	ADM_flvPlugin.cpp
	ADM_flvIndex.cpp
	ADM_flvAudio.cpp)

ADD_LIBRARY(ADM_dm_flv SHARED ${ADM_flv_SRCS})

INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_libraries/ADM_ffmpeg")

INIT_DEMUXER(ADM_dm_flv)
INSTALL_DEMUXER(ADM_dm_flv)
