include(mx_plugin)
SET(ADM_flv_SRCS 
	muxerFlv.cpp
	muxerFlvPlugin.cpp
        muxerFlvConfig.cpp
)

ADD_LIBRARY(ADM_mx_flv SHARED ${ADM_flv_SRCS})

INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_libraries/ADM_ffmpeg")

INIT_MUXER(ADM_mx_flv)
INSTALL_MUXER(ADM_mx_flv)

