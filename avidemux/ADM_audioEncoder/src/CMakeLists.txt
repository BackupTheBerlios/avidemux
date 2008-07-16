SET(ADMaudioEncoder_SRCS
audioencoder_aften.cpp
audioencoder_faac.cpp
audioencoder_lame.cpp
audioencoder_lavcodec.cpp
audioencoder_pcm.cpp
audioencoder_twolame.cpp
audioencoder_vorbis.cpp 
)	
ADD_ADM_LIB_ALL_TARGETS(ADM_audioEncoder ${ADMaudioEncoder_SRCS})
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_coreAudio/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_audioEncoder/include)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_audiofilter/)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/avidemux/ADM_audioEncoder/include)
IF (USE_AFTEN)
	ADD_SOURCE_CFLAGS(audioencoder_aften.cpp -I${AFTEN_INCLUDE_DIR})
ENDIF (USE_AFTEN)

IF (USE_FAAC)
	ADD_SOURCE_CFLAGS(audioencoder_faac.cpp -I${FAAC_INCLUDE_DIR})
ENDIF (USE_FAAC)

IF (HAVE_LIBMP3LAME)
	ADD_SOURCE_CFLAGS(audioencoder_lame.cpp -I${LAME_INCLUDE_DIR})
ENDIF (HAVE_LIBMP3LAME)

IF (USE_VORBIS)
	ADD_SOURCE_CFLAGS(audioencoder_vorbis.cpp -I${VORBIS_INCLUDE_DIR})
ENDIF (USE_VORBIS)

