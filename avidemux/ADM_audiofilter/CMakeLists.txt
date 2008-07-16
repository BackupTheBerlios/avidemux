SET(ADMaudiofilter_SRCS 
	audiodeng_buildfilters.cpp    audioeng_process.cpp           audiofilter_film2pal.cpp   audiolib_sox.cpp
	audiofilter_bridge.cpp         audiofilter_limiter.cpp    audio_raw.cpp
	audiofilter_buildchain.cpp     audiofilter_mixer.cpp
	audiofilter_normalize.cpp
	audioeng_buff.cpp          audiofilter_dolby.cpp          audiofilter_sox.cpp)
	
ADD_ADM_LIB_ALL_TARGETS(ADM_audiofilter ${ADMaudiofilter_SRCS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_coreAudio/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_audioEncoder/include)

