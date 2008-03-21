SET(ADM_LIB ADM_libpostproc)

SET(${ADM_LIB}_SRCS postprocess.c)

ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})
ADD_TARGET_CFLAGS(${ADM_LIB} "-I.. -DHAVE_AV_CONFIG_H  -O3 -I../.. -I../ADM_lavutil -Drestrict=__restrict__ -fomit-frame-pointer")

IF(ADM_CPU_X86_64)
	ADD_TARGET_CFLAGS(${ADM_LIB} "-msse")
ENDIF(ADM_CPU_X86_64)
IF (UNIX)
	ADD_TARGET_CFLAGS(${ADM_LIB} -fPIC -DPIC)
        ADD_TARGET_LDFLAGS(${ADM_LIB} "-rdynamic -export-dynamic -Wl,-Bsymbolic")
ENDIF (UNIX)


TARGET_LINK_LIBRARIES(${ADM_LIB} ADM_core ADM_libavutil)

INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR} LIBRARY DESTINATION lib)
