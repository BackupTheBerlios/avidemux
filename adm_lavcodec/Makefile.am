libavcodec_a_METASOURCES = AUTO
noinst_LIBRARIES = libavcodec.a
SUBDIRS=libpostproc i386 ppc 
if BUILD_MMX_COMPATIBLE
EXTEND_MMX = i386/cputest.c           \
			i386/mpegvideo_mmx.c           i386/vp3dsp_mmx.c \
			i386/dsputil_mmx.c  i386/idct_mmx.c      \
			i386/fdct_mmx.c     i386/motion_est_mmx.c  i386/simple_idct_mmx.c \
			i386/fft_sse.c i386/vp3dsp_sse2.c 
X_PPC_FLAGS=
else
EXTEND_MMX = 
endif

if BUILD_A64
X_MMX_FLAGS=-DHAVE_MMX 
else
X_MMX_FLAGS=
endif

if BUILD_ALTIVEC
EXTEND_PPC=ppc/dsputil_ppc.c ppc/mpegvideo_ppc.c ppc/dsputil_altivec.c \
	ppc/fft_altivec.c \
	ppc/gmc_altivec.c \
	ppc/idct_altivec.c \
	ppc/fdct_altivec.c \
	ppc/mpegvideo_altivec.c

else

if BUILD_PPC
EXTEND_PPC=ppc/dsputil_ppc.c ppc/mpegvideo_ppc.c 
endif

endif
EXTEND_AMR=	amr_float/interf_dec.c amr_float/sp_dec.c \
		amr_float/interf_enc.c amr_float/sp_enc.c
# for SSE we need special compil flags
fft_sse.o:i386/fft_sse.c
	$(CC) -o $@ $< -c $(CFLAGS) $(all_includes)   -I.. -DHAVE_AV_CONFIG_H $(X_MMX_FLAGS) -O3 -msse
vp3dsp_sse2.o:i386/vp3dsp_sse2.c
	$(CC) -o $@ $< -c  $(CFLAGS) $(all_includes)   -I.. -DHAVE_AV_CONFIG_H $(X_MMX_FLAGS) -O3 -msse

libavcodec_a_SOURCES = $(EXTEND_PPC) $(EXTEND_MMX) $(EXTEND_A64) ac3enc.c $(EXTEND_AMR) \
		adpcm.c 	allcodecs.c 	cyuv.c 	dsputil.c 	dv.c \
		error_resilience.c 	eval.c 	fdctref.c 	fft.c 	h263.c 	h263dec.c 	huffyuv.c 	imgconvert.c \
		imgresample.c 	jfdctfst.c 	jfdctint.c 	jrevdct.c 	mace.c 	mdct.c 	mem.c 	mjpeg.c \
		motion_est.c 	mpeg12.c 	mpegaudio.c 	mpegaudiodec.c 	mpegvideo.c 	msmpeg4.c 	opts.c \
		pcm.c 	ratecontrol.c 	raw.c 	resample.c 	rv10.c 	simple_idct.c 	svq1.c \
		utils.c 	wmadec.c 	indeo3.c 	vp3.c 	vp3dsp.c 	h264.c 	ffv1.c 	ra144.c 	ra288.c \
		golomb.c 	4xm.c 	cabac.c 	asv1.c vcr1.c cljr.c 	faandct.c 	rational.c 	parser.c \
		roqvideo.c 	xan.c 	rpza.c 	msrle.c 	msvideo1.c 	cinepak.c 	vqavideo.c \
		idcinvideo.c 	smc.c 	flicvideo.c 	interplayvideo.c 	dpcm.c 	8bps.c vmdav.c \
	truemotion1.c flac.c g726.c 	qtrle.c lcl.c 	snow.c integer.c \
		rangecoder.c bitstream.c h264idct.c h261.c amr.c
	

EXTRA_DIST = 
	
	

####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes)   -I.. -DHAVE_AV_CONFIG_H $(X_MMX_FLAGS) -O3 -Iamr_float
noinst_HEADERS = 

