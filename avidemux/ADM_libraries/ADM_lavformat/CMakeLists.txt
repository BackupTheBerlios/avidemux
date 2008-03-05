SET(ADM_LIB ADM_lavformat)

SET(${ADM_LIB}_SRCS 
	mpeg.c utils.c avio.c ADM_lavformat.cpp 
	aviobuf.c cutils.c img2.c file.c mpegtsenc.c ADM_muxts.cpp movenc.c 
	avienc.c wav.c mov.c matroska.c isom.c isom.h riff.c
	flvenc.c matroskaenc.c)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-I../ADM_lavcodec -I../ADM_lavutil -DHAVE_AV_CONFIG_H")

INCLUDE(admLibavcodec)
GET_LAVC_CFLAGS(LAVC_CFLAGS)
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "${LAVC_CFLAGS}")