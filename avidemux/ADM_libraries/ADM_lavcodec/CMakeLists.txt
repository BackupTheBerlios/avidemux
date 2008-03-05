SET(ADM_LIB ADM_lavcodec)

SET(${ADM_LIB}_SRCS 
	adpcm.c 	allcodecs.c 	cyuv.c 	dsputil.c 	dv.c 
	error_resilience.c 		fft.c 	h263.c 	h263dec.c 	huffyuv.c 	imgconvert.c 
	jfdctfst.c 	jfdctint.c 	jrevdct.c 	mace.c 	mdct.c 	 	mjpeg.c 
	motion_est.c 	mpeg12.c 	mpegaudio.c 	mpegaudiodec.c 	mpegvideo.c 	msmpeg4.c 	 
	pcm.c 	ratecontrol.c 	raw.c 	resample.c 	rv10.c 	simple_idct.c 	svq1.c 
	utils.c 	wmadec.c 	indeo3.c 	vp3.c 	vp3dsp.c 	h264.c 	ffv1.c 	ra144.c 	ra288.c 
	golomb.c 	4xm.c 	cabac.c 	asv1.c vcr1.c cljr.c 	faandct.c 	 	parser.c 
	roqvideo.c 	xan.c 	rpza.c 	msrle.c 	msvideo1.c 	cinepak.c 	vqavideo.c 
	idcinvideo.c 	smc.c 	flicvideo.c 	interplayvideo.c 	dpcm.c 	8bps.c vmdav.c 
	truemotion1.c flac.c g726.c 	qtrle.c lcl.c 	snow.c  
	rangecoder.c bitstream.c h264idct.c h261.c amr.c tscc.c 
	resample2.c pthread.c qdm2.c bitstream_filter.c eval.c 
	vc1.c vc1dsp.c vp56.c  vp6.c vp5.c indeo2.c smacker.c  opt.c
	fraps.c dnxhddata.h  dnxhddec.c
	mpeg12data.c  mpegaudiodata.c  msmpeg4data.c   vc1data.c   vp56data.c mjpegbdec.c
	h263_parser.c
	h264_parser.c
	mpeg4video_parser.c
	mpegvideo_enc.c
	mpeg12enc.c mpegaudioenc.c
	ac3enc.c ac3.c ac3tab.c
	mjpegenc.c mjpegdec.c
	wma.c
	dvbsub_parser.c
	dvbsubdec.c
	dvbsub.c xiph.c
	sp5xdec.c nellymoserdec.c)

IF(ADM_CPU_X86)
	SET(${ADM_LIB}_SRCS  ${${ADM_LIB}_SRCS}
		i386/cputest.c
		i386/mpegvideo_mmx.c     i386/vp3dsp_mmx.c 
		i386/dsputil_mmx.c   
		i386/fdct_mmx.c   i386/motion_est_mmx.c  i386/simple_idct_mmx.c 
		i386/fft_sse.c i386/vp3dsp_sse2.c  
		i386/idct_mmx_xvid.c i386/fft_3dn2.c 
		i386/snowdsp_mmx.c i386/fft_3dn.c)
ENDIF(ADM_CPU_X86)

IF (ADM_CPU_ALTIVEC)
	SET(${ADM_LIB}_SRCS  ${${ADM_LIB}_SRCS}
		ppc/dsputil_altivec.c  ppc/dsputil_ppc.c  ppc/fdct_altivec.c  ppc/fft_altivec.c  ppc/float_altivec.c  ppc/gmc_altivec.c
		ppc/h264_altivec.c  ppc/idct_altivec.c  ppc/int_altivec.c  ppc/mpegvideo_altivec.c  ppc/mpegvideo_ppc.c
		ppc/snow_altivec.c  ppc/vc1dsp_altivec.c)
ENDIF (ADM_CPU_ALTIVEC)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-I../ADM_lavutil -DHAVE_AV_CONFIG_H -Drestrict=__restrict__ -fomit-frame-pointer -include ffconf.h")

IF (ADM_DEBUG)
	ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-O2")
ENDIF (ADM_DEBUG)

IF (USE_AMR_NB)
	ADD_SOURCE_CFLAGS(amr.c -I${AMRNB_INCLUDE_DIR})
ENDIF (USE_AMR_NB)

INCLUDE(admLibFfmpeg)
GET_FFMPEG_CFLAGS(FFMPEG_CFLAGS)
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "${FFMPEG_CFLAGS}")