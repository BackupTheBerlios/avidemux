
SET(ADM_libMpeg2dec_SRCS 
alloc.c      cpu_state.c  header.c      idct_altivec.c  idct_mlib.c  motion_comp_alpha.c    motion_comp.c       motion_comp_mmx.c
cpu_accel.c  decode.c     idct_alpha.c  idct.c          idct_mmx.c   motion_comp_altivec.c  motion_comp_mlib.c  slice.c
)
ADD_LIBRARY(ADM_libMpeg2Dec STATIC ${ADM_libMpeg2dec_SRCS})
################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_libMpeg2Dec)
#
SET(${ADM_LIB}_SRCS 
alloc.c      cpu_state.c  header.c      idct_altivec.c  idct_mlib.c  motion_comp_alpha.c    motion_comp.c       motion_comp_mmx.c
cpu_accel.c  decode.c     idct_alpha.c  idct.c          idct_mmx.c   motion_comp_altivec.c  motion_comp_mlib.c  slice.c
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)

################################
