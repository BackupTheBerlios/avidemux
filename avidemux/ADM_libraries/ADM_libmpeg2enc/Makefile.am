####### kdevelop will overwrite this part!!! (begin)##########
noinst_LIBRARIES = libmpeg2enc.a


## INCLUDES were found outside kdevelop specific part

#quantize.cc 
#quantize_x86.cc 
#quantize.cc 
#quant_mmx.s 
#quantize_x86.cc 
#quantize.cc 

if BUILD_ALTIVEC
SUBDIRS = altivec
endif

libmpeg2enc_a_METASOURCES = AUTO
libmpeg2enc_a_SOURCES = ADM_mpe2enc.cpp ADM_mpeg2enc.h ad_layer.cc conform.cc fdctdata.cc fdctref.cc \
feedframe.cc idct.cc idctdata.cc macroblock.cc motionest.cc motionsearch.cc mpeg2enc.cc mpeg2param.cc \
mpeg2setup.cc mpegconsts.cc predict.cc predict_ref.cc putbits.cc puthdr.cc putmpg.cc putpic.cc putseq.cc \
putvlc.cc \
quantize_ref.cc ratectl.cc readpic.cc stats.cc synchrolib.cc transfrm.cc transfrm_ref.cc \
writepic.cc yuv4mpeg.cc yuv4mpeg_ratio.cc mpeg2enc_if.cc ioio.c cpu_accel.cc \
predcomp_mmx.cc predcomp_mmxe.cc \
mblock_bsad_mmx.cc \
mblock_sad_mmx.cc mblock_sad_mmxe.cc \
mblock_sumsq_mmx.cc \
predict_mmx.cc \
quantize.cc \
quant_mmx2.cc \
mblock_bsumsq_mmx.cc \
predict_x86.cc build_sub22_mests.cc build_sub44_mests.cc \
find_best_one_pel.cc \
motion.cc \
mblock_sub44_sads_x86.cc fdct_mmx.cc fdct_x86.cc idct_mmx.cc transfrm_x86.cc \
quantize_x86.cc


####### kdevelop will overwrite this part!!! (end)############

INCLUDES = $(all_includes)
#
# From mpeg2enc 's Makefile.am
%.o: %.s ; 	$(AS) $(ASFLAGS) -o $@ $<
#
EXTRA_DIST = ADM_mpe2enc.cpp       global.h        \
mmxsse_motion.h   predict_ref.cc   stats.cc \
ADM_mpeg2Param.h      idct.cc                     motion.cc         predict_ref.h    synchrolib.cc \
ADM_mpeg2enc.h        idct_mmx.cc                 motionest.cc      predict_x86.cc   synchrolib.h \
ad_layer.cc           idctdata.cc                 motionsearch.cc   putbits.cc       syntaxparams.h \
attributes.h          ioio.c                      motionsearch.h    puthdr.cc        tables.h \
build_sub22_mests.cc  macroblock.cc               mpeg2enc.cc       putmpg.cc        transfrm.cc \
build_sub44_mests.cc  macroblock.hh               mpeg2enc.h        putpic.cc        transfrm_ref.cc \
conform.cc            mblock_bsad_mmx.cc          mpeg2enc_if.cc    putseq.cc        transfrm_ref.h \
cpu_accel.cc          mblock_bsumsq_mmx.cc        mpeg2param.cc     putvlc.cc        transfrm_x86.cc \
cpu_accel.h           mblock_sad_mmx.cc           mpeg2parm.h       quant_mmx2.cc    vlc.h \
fastintfns.h          mblock_sad_mmxe.cc          mpeg2setup.cc     quantize.cc      writepic.cc \
fdct_mmx.cc           mblock_sub44_sads_x86.cc    mpegconsts.cc     quantize_ref.cc  xx.c \
fdct_x86.cc           mblock_sub44_sads_x86.h     mpegconsts.h      quantize_ref.h   yuv4mpeg.cc \
fdctdata.cc           mblock_sub44_sads_x86_h.cc  picture.hh        quantize_x86.cc  yuv4mpeg.h \
fdctref.cc            mblock_sumsq_mmx.cc         predcomp_mmx.cc   ratectl.cc       yuv4mpeg_intern.h \
feedframe.cc          mjpeg_logging.h             predcomp_mmxe.cc  ratectl.hh       yuv4mpeg_ratio.cc \
find_best_one_pel.cc  mjpeg_types.h               predict.cc        readpic.cc \
format_codes.h        mmx.h                       predict_mmx.cc    simd.h

noinst_HEADERS = ADM_mpeg2Param.h
