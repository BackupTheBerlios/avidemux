################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_lavcodec)
#
SET(${ADM_LIB}_SRCS 
ac3enc.c  
		adpcm.c 	allcodecs.c 	cyuv.c 	dsputil.c 	dv.c 
		error_resilience.c 		fdctref.c 	fft.c 	h263.c 	h263dec.c 	huffyuv.c 	imgconvert.c 
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
		vc1.c vc1dsp.c vp56.c vp56data.c vp6.c vp5.c indeo2.c smacker.c  opt.c
)
# ADD x86 ones

IF(ARCH_X86)
SET(${ADM_LIB}_SRCS  ${${ADM_LIB}_SRCS}
                        i386/cputest.c     
			i386/mpegvideo_mmx.c     i386/vp3dsp_mmx.c 
			i386/dsputil_mmx.c  i386/idct_mmx.c  
			i386/fdct_mmx.c   i386/motion_est_mmx.c  i386/simple_idct_mmx.c 
			i386/fft_sse.c i386/vp3dsp_sse2.c  
			i386/idct_mmx_xvid.c i386/fft_3dn2.c 
			i386/snowdsp_mmx.c i386/fft_3dn.c 
)
ENDIF(ARCH_X86)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
add_definitions(-I.. -DHAVE_AV_CONFIG_H  -O3 -I../.. -I../ADM_lavutil -Drestrict=__restrict__ -fomit-frame-pointer)

