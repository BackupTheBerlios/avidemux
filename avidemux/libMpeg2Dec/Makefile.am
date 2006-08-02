####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = liblibMpeg2Dec.a

## INCLUDES were found outside kdevelop specific part

liblibMpeg2Dec_a_METASOURCES = AUTO

liblibMpeg2Dec_a_SOURCES =  alloc.c attributes.h convert.h cpu_accel.c cpu_state.c decode.c header.c idct.c idct_alpha.c idct_altivec.c idct_mlib.c idct_mmx.c mmx.h motion_comp.c motion_comp_alpha.c motion_comp_altivec.c motion_comp_mlib.c motion_comp_mmx.c mpeg2.h mpeg2_internal.h slice.c  video_out.h vlc.h


EXTRA_DIST = alloc.c       cpu_state.c  idct_alpha.c    mmx.h     \
motion_comp_mlib.c  slice.c \
attributes.h  decode.c     idct_altivec.c  motion_comp.c          motion_comp_mmx.c   video_out.h \
convert.h     header.c     idct_mlib.c     motion_comp_alpha.c    mpeg2.h             vlc.h \
cpu_accel.c   idct.c       idct_mmx.c      motion_comp_altivec.c  mpeg2_internal.h 

####### kdevelop will overwrite this part!!! (end)############

#ugly trick to enable MMX
#INCLUDES = $(all_includes) $(LMPG_OPT)
INCLUDES = $(all_includes)  $(ALTIVEC_SWITCH)
