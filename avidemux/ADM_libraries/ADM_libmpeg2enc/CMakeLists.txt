SET(ADM_LIB ADM_libmpeg2enc)

SET(${ADM_LIB}_SRCS 
	ad_layer.cc           fdct_x86.cc           mblock_bsad_mmx.cc          motionest.cc     predcomp_mmxe.cc  putpic.cc        readpic.cc       yuv4mpeg.cc
	build_sub22_mests.cc  feedframe.cc          mblock_bsumsq_mmx.cc        motionsearch.cc  predict.cc        putseq.cc        stats.cc         yuv4mpeg_ratio.cc
	build_sub44_mests.cc  find_best_one_pel.cc  mblock_sad_mmx.cc           mpeg2enc.cc      predict_mmx.cc    putvlc.cc        synchrolib.cc
	conform.cc            idct.cc               mblock_sad_mmxe.cc          mpeg2enc_if.cc   predict_ref.cc    quantize.cc      transfrm.cc
	cpu_accel.cc          idctdata.cc           mblock_sub44_sads_x86.cc    mpeg2param.cc    predict_x86.cc    quantize_ref.cc  transfrm_ref.cc
	fdctdata.cc           idct_mmx.cc           mpeg2setup.cc               putbits.cc        quantize_x86.cc  transfrm_x86.cc
	fdct_mmx.cc           ioio.c                mblock_sumsq_mmx.cc         mpegconsts.cc    puthdr.cc         quant_mmx2.cc    writepic.cc
	fdctref.cc            macroblock.cc         motion.cc                   predcomp_mmx.cc  putmpg.cc         ratectl.cc       xx.c 
	ADM_mpe2enc.cpp)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})
ADD_CFLAGS_ALL_TARGETS(${ADM_LIB} "-I${CMAKE_CURRENT_SOURCE_DIR}")