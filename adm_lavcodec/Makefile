#
# libavcodec Makefile
# (c) 2000, 2001, 2002 Fabrice Bellard
#
include ./config.mak
include ./config.cpu

VPATH=$(SRC_PATH)/libavcodec

# NOTE: -I.. is needed to include config.h
CFLAGS= $(OPTFLAGS) -g -Wall -O2 -DHAVE_AV_CONFIG_H -I.. -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE -D_GNU_SOURCE
# used by darwin, useless for others
CFLAGS+=-I/usr/include/malloc
LDFLAGS= -g

OBJS= common.o utils.o mem.o allcodecs.o \
      mpegvideo.o h263.o jrevdct.o jfdctfst.o jfdctint.o\
      mpegaudio.o ac3enc.o mjpeg.o resample.o dsputil.o \
      motion_est.o imgconvert.o imgresample.o msmpeg4.o \
      mpeg12.o h263dec.o svq1.o rv10.o mpegaudiodec.o pcm.o simple_idct.o \
      ratecontrol.o adpcm.o eval.o dv.o error_resilience.o \
      wmadec.o fft.o mdct.o mace.o huffyuv.o opts.o cyuv.o \
      raw.o  vp3.o h264.o indeo3.o  golomb.o ffv1.o ra144.o  ra288.o \
      4xm.o cabac.o asv1.o vcr1.o cljr.o faandct.o rational.o parser.o \
      roqvideo.o xan.o rpza.o  msrle.o  msvideo1.o \
      cinepak.o vqavideo.o idcinvideo.o smc.o flicvideo.o interplayvideo.o dpcm.o \
      8bps.o vmdav.o truemotion1.o flac.o g726.o qtrle.o lcl.o

ASM_OBJS=

	
# i386 mmx specific stuff
ifeq ($(TARGET_MMX),yes)
OBJS += i386/fdct_mmx.o i386/cputest.o \
	i386/dsputil_mmx.o i386/mpegvideo_mmx.o \
	i386/idct_mmx.o i386/motion_est_mmx.o \
	i386/simple_idct_mmx.o
endif
ifeq ($(TARGET_PPC),yes)
OBJS += \
	ppc/dsputil_ppc.o \
	ppc/mpegvideo_ppc.o
endif
ifeq ($(TARGET_ALTIVEC),yes)
OBJS += \
	ppc/dsputil_altivec.o \
	ppc/fft_altivec.o \
	ppc/gmc_altivec.o \
	ppc/idct_altivec.o \
	ppc/fdct_altivec.o \
	ppc/mpegvideo_altivec.o 

CFLAGS+=-maltivec -mabi=altivec	 -faltivec
endif



# post proc files
# uncomplete ? 
#OBJS += 	libpostproc/postprocess.o

SRCS := $(OBJS:.o=.c) $(ASM_OBJS:.o=.S)
OBJS := $(OBJS) $(ASM_OBJS)

LIB= libavcodec.a
TESTS= imgresample-test dct-test motion-test fft-test

all: $(LIB) $(SLIB) libpostproc/libpostproc.a
tags:

tests: apiexample cpuid_test $(TESTS)

libpostproc/libpostproc.a:
		cd libpostproc && make && cd ..

$(LIB): $(OBJS)
	rm -f $@
	$(AR) rc $@ $(OBJS)
	$(RANLIB) $@

$(SLIB): $(OBJS)
	$(CC) $(SHFLAGS) -o $@ $(OBJS) $(EXTRALIBS)

dsputil.o: dsputil.c dsputil.h

# specific sse code
%_sse.o : %_sse.c
	$(CC) $(CFLAGS) -msse -c -o $@ $< 

# specific 3dnow code
%_3dnow.o : %_3dnow.c
	$(CC) $(CFLAGS) -m3dnow -c -o $@ $< 

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $< 

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<


# depend only used by mplayer now
dep:	depend

depend:
	$(CC) -MM $(CFLAGS) $(SRCS) 1>.depend

clean: 
	rm -f *.o *~ .depend $(LIB) $(SLIB) *.so i386/*.o i386/*~ \
	   armv4l/*.o armv4l/*~ \
	   mlib/*.o mlib/*~ \
	   alpha/*.o alpha/*~ \
	   ppc/*.o ppc/*~ \
	   ps2/*.o ps2/*~ \
	   liba52/*.o liba52/*~ \
	   apiexample $(TESTS)

distclean: clean
	rm -f Makefile.bak .depend
top_distdir = ../

DISTPPC= \
	ppc/dsputil_altivec.c \
	ppc/dsputil_altivec.h \
	ppc/gcc_fixes.h \
	ppc/dsputil_ppc.c \
	ppc/dsputil_ppc.h \
	ppc/fft_altivec.c \
	ppc/gmc_altivec.c \
	ppc/idct_altivec.c \
	ppc/fdct_altivec.c \
	ppc/mpegvideo_altivec.c \
	ppc/mpegvideo_ppc.c \


DIST_PP= \
	libpostproc/Makefile  \
	libpostproc/mangle.h  \
	libpostproc/postprocess.c  \
	libpostproc/postprocess.h  \
	libpostproc/postprocess_internal.h  \
	libpostproc/postprocess_template.c

DIST386= \
	i386/cputest.c \
	i386/dsputil_mmx.c \
	i386/dsputil_mmx_avg.h \
	i386/dsputil_mmx_rnd.h \
	i386/fdct_mmx.c \
	i386/fft_sse.c \
	i386/idct_mmx.c \
	i386/mmx.h \
	i386/motion_est_mmx.c \
	i386/mpegvideo_mmx.c \
	i386/mpegvideo_mmx_template.c \
	i386/simple_idct_mmx.c
DISTFILES= $(DIST386) $(DISTPPC) $(DIST_PP) \
	Makefile \
	a52dec.c \
	ac3.h \
	ac3dec.c \
	ac3enc.c \
	ac3tab.h \
	adpcm.c \
	allcodecs.c \
	apiexample.c \
	avcodec.c \
	avcodec.h \
	bswap.h \
	common.c \
	common.h \
	config.h \
	config.h \
	config.mak \
	cyuv.c \
	dct-test.c \
	dsputil.c \
	dsputil.h \
	dv.c \
	dvdata.h \
	error_resilience.c \
	eval.c \
	fastmemcpy.h \
	fdctref.c \
	fft-test.c \
	fft.c \
	h263.c \
	h263data.h \
	h263dec.c \
	huffyuv.c \
	imgconvert.c \
	imgresample.c \
	jfdctfst.c \
	jfdctint.c \
	jrevdct.c \
	mace.c \
	mdct.c \
	mem.c \
	mjpeg.c \
	sp5x.h \
	motion_est.c \
	motion_est_template.c \
	motion_test.c \
	mp3lameaudio.c \
	mpeg12.c \
	mpeg12data.h \
	mpeg4data.h \
	mpegaudio.c \
	mpegaudio.h \
	mpegaudiodec.c \
	mpegaudiodectab.h \
	mpegaudiotab.h \
	mpegvideo.c \
	mpegvideo.h \
	msmpeg4.c \
	msmpeg4data.h \
	oggvorbis.c \
	oggvorbis.h \
	opts.c \
	os_support.h \
	pcm.c \
	ratecontrol.c \
	raw.c \
	resample.c \
	rv10.c \
	simple_idct.c \
	simple_idct.h \
	svq1.c \
	svq1_cb.h \
	utils.c \
	wmadata.h \
	wmadec.c \
	wmv2.c \
	golomb.c \
	indeo3.c \
	svq3.c \
	indeo3data.h \
	golomb.h \
	vp3.c \
	vp3data.h \
	h264.c \
	h264data.h \
	imgconvert_template.h \
	ffv1.c \
	ra144.h \
	ra288.h \
	ra144.c \
	ra288.c  \
	golomb.c \
	4xm.c \
	cabac.c \
	cabac.h \
	golomb.h \
	svq1_vlc.h \
	asv1.c vcr1.c cljr.c \
	mdec.c \
	faandct.c \
	faandct.h \
	rational.c \
	rational.h \
	parser.c \
      	roqvideo.c \
	xan.c \
	rpza.c  \
	msrle.c  \
	msvideo1.c \
	cinepak.c  \
	vqavideo.c  \
	idcinvideo.c  \
	smc.c  \
	flicvideo.c  \
	interplayvideo.c \
	dpcm.c \
	8bps.c vmdav.c truemotion1.c  truemotion1data.h flac.c g726.c \
	qtrle.c lcl.c

srcdir= .
distdir: $(DISTFILES)
	mkdir $(distdir)/i386
	mkdir $(distdir)/ppc
	mkdir $(distdir)/libpostproc
	@for file in $(DISTFILES); do \
	if test -f $$file; then d=.; else d=$(srcdir); fi; \
	dir=`echo "$$file" | sed -e 's,/[^/]*$$,,'`; \
	if test "$$dir" != "$$file" && test "$$dir" != "."; then \
	$(mkinstalldirs) "$(distdir)/$$dir"; \
	fi; \
	if test -d $$d/$$file; then \
	cp -pR $$d/$$file $(distdir) \
	|| exit 1; \
	else \
	test -f $(distdir)/$$file \
	|| cp -p $$d/$$file $(distdir)/$$file \
	|| exit 1; \
	fi; \
	done

# api example program
apiexample: apiexample.c $(LIB)
	$(CC) $(CFLAGS) -o $@ $< $(LIB) $(EXTRALIBS) -lm

# cpuid test
cpuid_test: i386/cputest.c
	$(CC) $(CFLAGS) -D__TEST__ -o $@ $<

# testing progs

imgresample-test: imgresample.c
	$(CC) $(CFLAGS) -DTEST -o $@ $^ -lm

dct-test: dct-test.o jfdctfst.o jfdctint.o i386/fdct_mmx.o\
          fdctref.o jrevdct.o i386/idct_mmx.o simple_idct.o i386/simple_idct_mmx.o
	$(CC) -o $@ $^ -lm

motion-test: motion_test.o $(LIB)
	$(CC) -o $@ $^ -lm

fft-test: fft-test.o fft.o mdct.o
	$(CC) -o $@ $^ -lm

install: all

installlib: all
	echo "nothing to install"
#
# include dependency files if they exist
#

#ifneq ($(wildcard .depend),)
#include .depend
#endif
