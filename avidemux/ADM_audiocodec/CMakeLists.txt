SET(ADM_audiocodec_SRCS ADM_8bits.cpp  ADM_codecac3.cpp   
	ADM_codecimaadpcm.cpp  
	ADM_codecmsadpcm.cpp  
	ADM_codeculaw.cpp  ADM_codecwma.cpp  ADM_lpcm.cpp
	ADM_audiocodec.cpp  ADM_codecfaad.cpp  ADM_codecmp3.cpp  ADM_codecwav.cpp   ADM_dca.cpp   audiocodec_ogg.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_audiocodec ${ADM_audiocodec_SRCS})

IF (USE_AMR_NB)
	ADD_SOURCE_CFLAGS(ADM_audiocodec.cpp -I${AMRNB_INCLUDE_DIR})
ENDIF (USE_AMR_NB)

IF (USE_FAAD)
	ADD_SOURCE_CFLAGS(ADM_codecfaad.cpp -I${FAAD_INCLUDE_DIR})
ENDIF (USE_FAAD)

IF (USE_LIBDCA)
	ADD_SOURCE_CFLAGS(ADM_audiocodec.cpp -I${LIBDCA_INCLUDE_DIR})
	ADD_SOURCE_CFLAGS(ADM_dca.cpp -I${LIBDCA_INCLUDE_DIR})
ENDIF (USE_LIBDCA)

IF (USE_VORBIS)
	ADD_SOURCE_CFLAGS(audiocodec_ogg.cpp -I${VORBIS_INCLUDE_DIR})
ENDIF (USE_VORBIS)