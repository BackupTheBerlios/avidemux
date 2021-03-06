SET(ADM_LIB ADM_libavcodec)

SET(${ADM_LIB}_SRCS 
	adpcm.c  allcodecs.c  cyuv.c  dsputil.c  dv.c  error_resilience.c  fft.c  h263.c
	h263dec.c  huffyuv.c  imgconvert.c  jfdctfst.c  jfdctint.c  jrevdct.c  mace.c
	mdct.c  mjpeg.c  motion_est.c  mpeg12.c  mpegaudio.c  mpegaudiodec.c  mpegvideo.c
	msmpeg4.c  pcm.c  ratecontrol.c  raw.c  resample.c  rv10.c  simple_idct.c  svq1.c
	utils.c  wmadec.c  indeo3.c  vp3.c  vp3dsp.c  h264.c  ffv1.c  ra144.c  ra288.c
	golomb.c  4xm.c  cabac.c  asv1.c  vcr1.c  cljr.c  faandct.c  parser.c  roqvideo.c
	xan.c  rpza.c  msrle.c  msrledec.c msvideo1.c  cinepak.c  vqavideo.c  idcinvideo.c  smc.c
	flicvideo.c  interplayvideo.c  dpcm.c  8bps.c  vmdav.c  truemotion1.c  flac.c
	g726.c  qtrle.c  lcl.c  snow.c  rangecoder.c  bitstream.c  h264idct.c  h261.c
	amr.c  tscc.c  resample2.c    qdm2.c  bitstream_filter.c  eval.c  vc1.c
	vc1dsp.c  vp56.c  vp6.c  vp5.c  indeo2.c  smacker.c  opt.c  fraps.c  dnxhddec.c
	mpeg12data.c  mpegaudiodata.c  msmpeg4data.c  vc1data.c  vp56data.c  mjpegbdec.c
	h263_parser.c  h264_parser.c  mpeg4video_parser.c  mpegvideo_enc.c  mpeg12enc.c
	mpegaudioenc.c  ac3enc.c  ac3.c  ac3tab.c  mjpegenc.c  mjpegdec.c  wma.c  
	dvbsub_parser.c  dvbsubdec.c  dvbsub.c  xiph.c  sp5xdec.c  
        nellymoserdec.c nellymoser.c
	ADM_lavcodec.cpp  pthread.c  huffman.c  wmv2dec.c  wmv2.c  h264pred.c  intrax8.c
	intrax8dsp.c  dnxhddata.c  faanidct.c  ac3_parser.c  aac_ac3_parser.c
        celp_math.c options.c aandcttab.c audioconvert.c mpeg4audio.c svq1dec.c celp_filters.c
	aac_parser.c  mpegaudiodecheader.c)

IF(ADM_CPU_X86)
	SET(${ADM_LIB}_SRCS  ${${ADM_LIB}_SRCS}
		x86/cpuid.c  x86/mpegvideo_mmx.c  x86/vp3dsp_mmx.c  x86/dsputil_mmx.c
		x86/fdct_mmx.c  x86/motion_est_mmx.c  x86/simple_idct_mmx.c  
		x86/vp3dsp_sse2.c  x86/idct_mmx_xvid.c    x86/snowdsp_mmx.c
                x86/idct_sse2_xvid.c 
                #x86/fft_sse.c
                #x86/fft_3dn2.c
		#x86/fft_3dn.c  
                x86/dsputilenc_mmx.c  x86/vc1dsp_mmx.c  x86/idct_mmx.c)
ENDIF(ADM_CPU_X86)

IF (ADM_CPU_ALTIVEC)
	SET(${ADM_LIB}_SRCS  ${${ADM_LIB}_SRCS}
		ppc/dsputil_altivec.c  ppc/dsputil_ppc.c  ppc/fdct_altivec.c  ppc/fft_altivec.c
		ppc/float_altivec.c  ppc/gmc_altivec.c  ppc/h264_altivec.c  ppc/idct_altivec.c
		ppc/int_altivec.c  ppc/mpegvideo_altivec.c  ppc/mpegvideo_ppc.c
		ppc/snow_altivec.c  ppc/vc1dsp_altivec.c  ppc/check_altivec.c)
ENDIF (ADM_CPU_ALTIVEC)

ADD_LIBRARY(${ADM_LIB} SHARED ${${ADM_LIB}_SRCS})

IF (ADM_DEBUG AND ADM_CPU_X86_32)
	ADD_SOURCE_CFLAGS(cabac.c -O2)
	ADD_SOURCE_CFLAGS(h264.c -O2)
	ADD_SOURCE_CFLAGS(h264_parser.c -O2)
	ADD_SOURCE_CFLAGS(x86/dsputil_mmx.c -O2)
	ADD_SOURCE_CFLAGS(x86/snowdsp_mmx.c -O2)
ENDIF (ADM_DEBUG AND ADM_CPU_X86_32)

TARGET_LINK_LIBRARIES(${ADM_LIB} ADM_core ADM_libavutil ${ZLIB_LIBRARY})

INSTALL(TARGETS ${ADM_LIB} RUNTIME DESTINATION ${BIN_DIR}  LIBRARY DESTINATION lib  ARCHIVE DESTINATION lib)
