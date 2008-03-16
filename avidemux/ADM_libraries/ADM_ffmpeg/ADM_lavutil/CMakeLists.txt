SET(ADM_LIB ADM_libavutil)

SET(${ADM_LIB}_SRCS 
	integer.c  intfloat_readwrite.c  mathematics.c  rational.c  md5.c  crc.c
	log.c  adler32.c  lls.c  random.c  string.c)

ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})
ADD_TARGET_CFLAGS(${ADM_LIB} "-DHAVE_AV_CONFIG_H")

IF (UNIX)
	ADD_TARGET_CFLAGS(${ADM_LIB} -fPIC)
ENDIF (UNIX)


INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR} LIBRARY DESTINATION lib)
