####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libmpeg2enc.a

## INCLUDES were found outside kdevelop specific part

libmpeg2enc_a_METASOURCES = AUTO
if    HAVE_NASM
libmpeg2enc_a_SOURCES = ADM_mpe2enc.cpp ADM_mpeg2enc.h ad_layer.cc conform.cc fdctdata.cc fdctref.cc \
feedframe.cc idct.cc idctdata.cc macroblock.cc motionest.cc motionsearch.cc mpeg2enc.cc mpeg2param.cc \
mpeg2setup.cc mpegconsts.cc predict.cc predict_ref.cc putbits.cc puthdr.cc putmpg.cc putpic.cc putseq.cc \
putvlc.cc quantize.cc quantize_ref.cc ratectl.cc readpic.cc stats.cc synchrolib.cc transfrm.cc transfrm_ref.cc \
writepic.cc yuv4mpeg.cc yuv4mpeg_ratio.cc mpeg2enc_if.cc ioio.c cpu_accel.cc predcomp_mmx.s predcomp_mmxe.s \
predict_mmx.s quant_mmx.s quantize_x86.cc predict_x86.cc build_sub22_mests.cc build_sub44_mests.cc \
find_best_one_pel.cc mblock_bsad_mmx.s mblock_bsumsq_mmx.s mblock_sad_mmx.s mblock_sad_mmxe.s motion.cc \
mblock_sumsq_mmx.s mblock_sub44_sads_x86.cc fdct_mmx.s idct_mmx.s transfrm_x86.cc
else
libmpeg2enc_a_SOURCES = ADM_mpe2enc.cpp ADM_mpeg2enc.h ad_layer.cc conform.cc \
								cpu_accel.cc fdctdata.cc fdctref.cc feedframe.cc idct.cc idctdata.cc \
								macroblock.cc motionest.cc motionsearch.cc mpeg2enc.cc \
								mpeg2param.cc mpeg2setup.cc mpegconsts.cc predict.cc \
								predict_ref.cc putbits.cc puthdr.cc putmpg.cc putpic.cc putseq.cc \
								putvlc.cc quantize.cc quantize_ref.cc ratectl.cc readpic.cc stats.cc \
								synchrolib.cc transfrm.cc transfrm_ref.cc writepic.cc yuv4mpeg.cc \
								yuv4mpeg_ratio.cc mpeg2enc_if.cc ioio.c


endif

EXTRA_DIST =

####### kdevelop will overwrite this part!!! (end)############

INCLUDES = $(all_includes)
#
#if    HAVE_NASM
#libmpeg2enc_a_SOURCES = ADM_mpe2enc.cpp ADM_mpeg2enc.h ad_layer.cc conform.cc \
#								cpu_accel.c fdctdata.cc fdctref.cc feedframe.cc idct.cc idctdata.cc \
#								macroblock.cc motionest.cc motionsearch.cc mpeg2enc.cc \
#								mpeg2param.cc mpeg2setup.cc mpegconsts.cc predict.cc \
#								predict_ref.cc putbits.cc puthdr.cc putmpg.cc putpic.cc putseq.cc \
#								putvlc.cc quantize.cc quantize_ref.cc ratectl.cc readpic.cc stats.cc \
#								synchrolib.cc transfrm.cc transfrm_ref.cc writepic.cc yuv4mpeg.cc \
#								yuv4mpeg_ratio.cc mpeg2enc_if.cc ioio.c fdct_mmx.s fdctdata.c \
#								idct_mmx.s idctdata.c \
#  								quant_mmx.s quantize_x86.c predict_mmx.s \
#   								predcomp_mmxe.s predcomp_mmx.s transfrm_x86.c \
#								predict_x86.c
#else
#libmpeg2enc_a_SOURCES = ADM_mpe2enc.cpp ADM_mpeg2enc.h ad_layer.cc conform.cc \
#								cpu_accel.c fdctdata.cc fdctref.cc feedframe.cc idct.cc idctdata.cc \
#								macroblock.cc motionest.cc motionsearch.cc mpeg2enc.cc \
#								mpeg2param.cc mpeg2setup.cc mpegconsts.cc predict.cc \
#								predict_ref.cc putbits.cc puthdr.cc putmpg.cc putpic.cc putseq.cc \
#								putvlc.cc quantize.cc quantize_ref.cc ratectl.cc readpic.cc stats.cc \
#								synchrolib.cc transfrm.cc transfrm_ref.cc writepic.cc yuv4mpeg.cc \
#								yuv4mpeg_ratio.cc mpeg2enc_if.cc ioio.c
#
#
#endif
#
# From mpeg2enc 's Makefile.am
%.o: %.s ; 	$(AS) $(ASFLAGS) -o $@ $<
#
EXTRA_DIST = mpeg2parm.h attributes.h mjpeg_types.h global.h transfrm_ref.h mpeg2enc.h  picture.hh simd.h fastintfns.h motionsearch.h mmxsse_motion.h quantize_ref.h format_codes.h ratectl.hh vlc.h syntaxparams.h tables.h predict_ref.h mpegconsts.h synchrolib.h mjpeg_logging.h mblock_sub44_sads_x86.h mblock_sub44_sads_x86_h.cc macroblock.hh cpu_accel.h yuv4mpeg.h  yuv4mpeg_intern.h mmx.h
