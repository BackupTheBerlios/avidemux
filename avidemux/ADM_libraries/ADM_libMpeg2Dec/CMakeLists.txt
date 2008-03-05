SET(ADM_LIB ADM_libMpeg2Dec)

SET(${ADM_LIB}_SRCS 
	alloc.c      cpu_state.c  header.c      idct_altivec.c  idct_mlib.c  motion_comp_alpha.c    motion_comp.c       motion_comp_mmx.c
	cpu_accel.c  decode.c     idct_alpha.c  idct.c          idct_mmx.c   motion_comp_altivec.c  motion_comp_mlib.c  slice.c)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DACCEL_DETECT")

IF (ADM_CPU_X86)
	ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DARCH_X86")
ELSEIF (ADM_CPU_ALTIVEC)
	ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DARCH_PPC")

	IF (NOT APPLE)
		ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-DHAVE_ALTIVEC_H")
	ENDIF (NOT APPLE)
ENDIF (ADM_CPU_X86)