SET(ADMaudiofilter_SRCS 
	audiodeng_buildfilters.cpp    
        audiofilter_film2pal.cpp   
	audiofilter_bridge.cpp         
        audiofilter_limiter.cpp    
	audiofilter_buildchain.cpp     
        audiofilter_mixer.cpp
	audiofilter_normalize.cpp
        audiofilter_dolby.cpp          
        audiofilter_sox.cpp
        audio_raw.cpp
        audiolib_sox.cpp
	audioeng_buff.cpp          
        audio_encoderWrapper.cpp
)
	
ADD_ADM_LIB_ALL_TARGETS(ADM_audiofilter ${ADMaudiofilter_SRCS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_audioEncoder/include)

