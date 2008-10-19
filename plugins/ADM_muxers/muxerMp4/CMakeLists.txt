include(mx_plugin)
SET(ADM_mp4_SRCS 
	muxerMP4.cpp
	muxerMP4Plugin.cpp
)

ADD_LIBRARY(ADM_mx_mp4 SHARED ${ADM_mp4_SRCS})

#TARGET_LINK_LIBRARIES(ADM_dm_flv ADM_libavutil)

INCLUDE_DIRECTORIES("${AVIDEMUX_SOURCE_DIR}/avidemux/ADM_libraries/ADM_ffmpeg")

INIT_MUXER(ADM_mx_mp4)
INSTALL_MUXER(ADM_mx_mp4)

