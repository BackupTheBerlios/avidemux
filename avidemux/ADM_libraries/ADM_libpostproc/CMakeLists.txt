SET(ADM_LIB ADM_libpostproc)

SET(${ADM_LIB}_SRCS 
	postprocess.c)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-I.. -DHAVE_AV_CONFIG_H  -O3 -I../.. -I../ADM_lavutil -Drestrict=__restrict__ -fomit-frame-pointer")

IF(ARCH_X86_64)
	ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-msse")
ENDIF(ARCH_X86_64)