libavcodec_a_METASOURCES = AUTO
noinst_LIBRARIES = libavcodec.a
SUBDIRS=libpostproc i386 ppc 
LAVUTIL=../ADM_lavutil 
if BUILD_MMX_COMPATIBLE
EXTEND_MMX = i386/cputest.c     \
			i386/mpegvideo_mmx.c     i386/vp3dsp_mmx.c \
			i386/dsputil_mmx.c  i386/idct_mmx.c  \
			i386/fdct_mmx.c   i386/motion_est_mmx.c  i386/simple_idct_mmx.c \
			i386/fft_sse.c i386/vp3dsp_sse2.c  \
			i386/idct_mmx_xvid.c i386/fft_3dn2.c \
			i386/snowdsp_mmx.c i386/fft_3dn.c 
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
	ppc/dsputil_h264_altivec.c \
	ppc/mpegvideo_altivec.c

else

if BUILD_PPC
EXTEND_PPC=ppc/dsputil_ppc.c ppc/mpegvideo_ppc.c 
else
EXTEND_PPC=
endif

endif
EXTEND_AMR=	amr_float/interf_dec.c amr_float/sp_dec.c \
		amr_float/interf_enc.c amr_float/sp_enc.c
# for SSE we need special compil flags
fft_sse.o:i386/fft_sse.c
	$(CC) -o $@ $< -c $(CFLAGS) $(all_includes) -I.. -DHAVE_AV_CONFIG_H $(X_MMX_FLAGS) -O3 -msse -I$(LAVUTIL) -I../..
vp3dsp_sse2.o:i386/vp3dsp_sse2.c
	$(CC) -o $@ $< -c  $(CFLAGS) $(all_includes) -I.. -DHAVE_AV_CONFIG_H $(X_MMX_FLAGS) -O3 -msse -I$(LAVUTIL) -I../..

libavcodec_a_SOURCES = $(EXTEND_PPC) $(EXTEND_MMX) ac3enc.c $(EXTEND_AMR) \
		adpcm.c 	allcodecs.c 	cyuv.c 	dsputil.c 	dv.c \
		error_resilience.c 		fdctref.c 	fft.c 	h263.c 	h263dec.c 	huffyuv.c 	imgconvert.c \
		imgresample.c 	jfdctfst.c 	jfdctint.c 	jrevdct.c 	mace.c 	mdct.c 	 	mjpeg.c \
		motion_est.c 	mpeg12.c 	mpegaudio.c 	mpegaudiodec.c 	mpegvideo.c 	msmpeg4.c 	 \
		pcm.c 	ratecontrol.c 	raw.c 	resample.c 	rv10.c 	simple_idct.c 	svq1.c \
		utils.c 	wmadec.c 	indeo3.c 	vp3.c 	vp3dsp.c 	h264.c 	ffv1.c 	ra144.c 	ra288.c \
		golomb.c 	4xm.c 	cabac.c 	asv1.c vcr1.c cljr.c 	faandct.c 	 	parser.c \
		roqvideo.c 	xan.c 	rpza.c 	msrle.c 	msvideo1.c 	cinepak.c 	vqavideo.c \
		idcinvideo.c 	smc.c 	flicvideo.c 	interplayvideo.c 	dpcm.c 	8bps.c vmdav.c \
		truemotion1.c flac.c g726.c 	qtrle.c lcl.c 	snow.c  \
		rangecoder.c bitstream.c h264idct.c h261.c amr.c tscc.c \
		resample2.c imgresample.c pthread.c qdm2.c bitstream_filter.c eval.c \
		vc1.c vc1dsp.c
	

EXTRA_DIST = ac3.h    faandct.h      internal.h   \
oggvorbis.h  snow.h     vc1data.h \
ac3tab.h   fastmemcpy.h     lzo.h      opt.h    sp5x.h     vorbis.h \
avcodec.h  golomb.h     mpeg12data.h   os_support.h svq1_cb.h    vp3data.h \
bitstream.h  h261data.h     mpeg4data.h    qdm2data.h   svq1_vlc.h   wmadata.h \
cabac.h  h263data.h     mpegaudio.h    ra144.h    swscale.h \
cavsdata.h h264data.h     mpegaudiodectab.h  ra288.h    truemotion1data.h \
cookdata.h imgconvert_template.h  mpegaudiotab.h   rangecoder.h truespeech_data.h \
dsputil.h  indeo2data.h     mpegvideo.h    rtjpeg.h   ulti_cb.h \
dvdata.h   indeo3data.h     msmpeg4data.h  simple_idct.h  vc1acdata.h \
4xm.c     dtsdec.c    indeo2.c     oggvorbis.c  svq3.c \
8bps.c      dv.c      indeo3.c     opt.c    truemotion1.c \
a52dec.c    dvbsub.c    interplayvideo.c   opts.c   truemotion2.c \
aasc.c      dvbsubdec.c   jfdctfst.c     os2thread.c  truespeech.c \
ac3dec.c    dvdsub.c    jfdctint.c     parser.c   tscc.c \
ac3enc.c    dvdsubenc.c   jpeg_ls.c      pcm.c    tta.c \
adpcm.c     error_resilience.c  jrevdct.c      png.c    ulti.c \
adx.c     eval.c      kmvc.c       pnm.c    utils.c \
alac.c      faac.c      lcl.c      pthread.c  vc1.c \
allcodecs.c   faad.c      libgsm.c     qdm2.c   vcr1.c \
amr.c     faandct.c     loco.c       qdrw.c   vmdav.c \
apiexample.c    fdctref.c     lzo.c      qpeg.c   vorbis.c \
asv1.c      fft-test.c    mace.c       qtrle.c    vp3.c \
avcodec.c     fft.c     mdct.c       ra144.c    vp3dsp.c \
avs.c     ffv1.c      mdec.c       ra288.c    vqavideo.c \
beosthread.c    flac.c      mem.c      rangecoder.c w32thread.c \
bitstream.c   flacenc.c     mjpeg.c      ratecontrol.c  wmadec.c \
bitstream_filter.c  flashsv.c     mmvideo.c      raw.c    wmv2.c \
bmp.c     flicvideo.c   motion_est.c     resample.c   wnv1.c \
cabac.c     fraps.c     motion_est_template.c  resample2.c  ws-snd1.c \
cavs.c      g726.c      motion_test.c    roqvideo.c   x264.c \
cavsdsp.c     golomb.c    mp3lameaudio.c   rpza.c   xan.c \
cinepak.c     h261.c      mpeg12.c     rtjpeg.c   xl.c \
cljr.c      h263.c      mpegaudio.c    rv10.c   xvid_rc.c \
common.c    h263dec.c     mpegaudiodec.c   shorten.c  xvidff.c \
cook.c      h264.c      mpegvideo.c    simple_idct.c  xvmcvideo.c \
cscd.c      h264idct.c    msmpeg4.c      smacker.c  zmbv.c \
cyuv.c      huffyuv.c     msrle.c      smc.c \
dct-test.c    idcinvideo.c    msvideo1.c     snow.c \
dpcm.c      imgconvert.c    nuv.c      sonic.c \
dsputil.c     imgresample.c   oggtheora.c    svq1.c \
vc1dsp.c \
amr_float i386 libpostproc ppc 

	
	

####### kdevelop will overwrite this part!!! (end)############
INCLUDES = $(all_includes) -I.. -DHAVE_AV_CONFIG_H $(X_MMX_FLAGS) -O3 -Iamr_float -I../avidemux/ADM_lavutil  -I$(LAVUTIL)
noinst_HEADERS = 

