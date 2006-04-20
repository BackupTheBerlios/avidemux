####### kdevelop will overwrite this part!!! (begin)##########
noinst_PROGRAMS =liblibMpeg2Dec.o 
liblibMpeg2Dec_o_LDFLAGS =  -r
LIBS =  
LDFLAGS = 
	
## INCLUDES were found outside kdevelop specific part

liblibMpeg2Dec_o_METASOURCES = AUTO

liblibMpeg2Dec_o_SOURCES =  alloc.c attributes.h convert.h cpu_accel.c cpu_state.c decode.c header.c idct.c idct_alpha.c idct_altivec.c idct_mlib.c idct_mmx.c mmx.h motion_comp.c motion_comp_alpha.c motion_comp_altivec.c motion_comp_mlib.c motion_comp_mmx.c mpeg2.h mpeg2_internal.h slice.c  video_out.h vlc.h


EXTRA_DIST = cpu_state.c decode.c video_out.h header.c idct.c idct_altivec.c idct_mmx.c  mmx.h motion_comp.c motion_comp_altivec.c motion_comp_mlib.c motion_comp_mmx.c mpeg2.h mpeg2_internal.h slice.c 

####### kdevelop will overwrite this part!!! (end)############

#ugly trick to enable MMX
#INCLUDES = $(all_includes) $(LMPG_OPT)
INCLUDES = $(all_includes)  $(ALTIVEC_SWITCH)
