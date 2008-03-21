SET(ADM_LIB ADM_libavutil)

SET(${ADM_LIB}_SRCS 
adler32.c
aes.c
base64.c
crc.c
des.c
fifo.c
integer.c
intfloat_readwrite.c
lls.c
log.c
lzo.c
mathematics.c
md5.c
mem.c
random.c
rational.c
rc4.c
sha1.c
softfloat.c
string.c
tree.c
)
ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})
ADD_TARGET_CFLAGS(${ADM_LIB} "-DHAVE_AV_CONFIG_H")

IF (UNIX)
	ADD_TARGET_CFLAGS(${ADM_LIB} -fPIC)
ENDIF (UNIX)


INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR} LIBRARY DESTINATION lib)
