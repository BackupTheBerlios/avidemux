SET(ADM_audiodevice_SRCS 
	ADM_audiodevice.cpp  
	)

ADD_ADM_LIB_ALL_TARGETS(ADM_audiodevice ${ADM_audiodevice_SRCS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_coreAudio/include)

#SDLify(ADM_deviceSDL.cpp)

#IF (USE_ARTS)
	#ADD_CFLAGS_ALL_TARGETS(ADM_audiodevice ${ARTS_CFLAGS})
#ENDIF (USE_ARTS)
