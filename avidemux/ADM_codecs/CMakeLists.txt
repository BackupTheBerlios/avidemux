SET(ADM_codecs_SRCS 
	ADM_codecs.cpp  ADM_ffmpeg.cpp  ADM_mjpegEncode.cpp  ADM_png.cpp    ADM_theora_dec.cpp  ADM_x264.cpp   ADM_xvid.cpp     ADM_xvideco.cpp
	ADM_ffmp43.cpp  ADM_mjpeg.cpp   ADM_mpeg.cpp         ADM_rgb16.cpp  ADM_uyvy.cpp        ADM_xvid4.cpp  ADM_xvidcvs.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_codecs ${ADM_codecs_SRCS})

IF (USE_PNG)
	 ADD_SOURCE_CFLAGS(ADM_png.cpp ${PNG_CFLAGS})
ENDIF (USE_PNG)

IF (USE_X264)
	ADD_SOURCE_CFLAGS(ADM_x264.cpp -I${X264_INCLUDE_DIR})
ENDIF (USE_X264)

IF (USE_XVID_4)
	ADD_SOURCE_CFLAGS(ADM_xvid4.cpp -I${XVID_INCLUDE_DIR})
ENDIF (USE_XVID_4)