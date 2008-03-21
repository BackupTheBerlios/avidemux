SET(ADM_LIB ADM_libavformat)

SET(${ADM_LIB}_SRCS 
	mpeg.c  utils.c  avio.c  aviobuf.c  cutils.c  img2.c  file.c  mpegtsenc.c
	movenc.c  avienc.c  wav.c  mov.c  matroska.c  isom.c  isom.h  riff.c
	flvenc.c  matroskaenc.c
	avc.c raw.c  riff.c)

ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})
ADD_TARGET_CFLAGS(${ADM_LIB} "-I../ADM_lavcodec -I../ADM_lavutil -DHAVE_AV_CONFIG_H")

IF (UNIX)
	ADD_TARGET_CFLAGS(${ADM_LIB} -fPIC -DPIC)
        ADD_TARGET_LDFLAGS(${ADM_LIB} "-rdynamic -export-dynamic -Wl,-Bsymbolic")
ENDIF (UNIX)

TARGET_LINK_LIBRARIES(${ADM_LIB} ADM_core ADM_libavutil ADM_libavcodec)

INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR} LIBRARY DESTINATION lib)
