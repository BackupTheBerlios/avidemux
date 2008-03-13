SET(ADM_videoFilter_SRCS 
	ADM_lavpp_deint.cpp      ADM_vidDecTelecide.cpp      ADM_vidFields.cpp           ADM_vidMcDeint.cpp
	ADM_vidPulldown.cpp       ADM_vidSwapSmart.cpp
	ADM_vidAddBorder.cpp     ADM_vidDeintASM.cpp         ADM_vidFlipV.cpp            ADM_vidMosaic.cpp
	ADM_vidResampleFPS.cpp    ADM_vidUVSwap.cpp
	ADM_vidASS.cpp           ADM_vidDeinterlace.cpp      ADM_vidFlux.cpp             ADM_vidMPLD3D.cpp
	ADM_vidReverse.cpp        ADM_vidVlad.cpp
	ADM_vidBlend.cpp         ADM_vidDenoise.cpp          ADM_vidForcedPP.cpp         ADM_vidMPLD3Dlow.cpp
	ADM_vidRotate.cpp         ADM_vidWhirl.cpp
	ADM_vidBlendRemoval.cpp  ADM_vidDGbob.cpp            ADM_vidHardIvtcRemoval.cpp  ADM_vidMSharpen.cpp
	ADM_vidSeparateField.cpp  ADM_vidYadif.cpp
	ADM_vidBSmear.cpp        ADM_vidDropOut.cpp          ADM_vidKernelDeint.cpp      ADM_vidMsmooth.cpp
	ADM_vidSoften.cpp
	ADM_vidChroma.cpp        ADM_vidFade.cpp             ADM_vidLargeMedian.cpp      ADM_vidPalShift.cpp
	ADM_vidStabilize.cpp
	ADM_vidDecDec.cpp        ADM_vidFastConvolution.cpp  ADM_vidLuma.cpp             ADM_vidPalSmart.cpp
	ADM_vidSwapFields.cpp
	ADM_vidYadif_asm.c
	ADM_guiResize.cpp  ADM_resizebis.cpp  ADM_resizeter.cpp  ADM_vidResize.cpp
	ADM_vidMPLResize.cpp
	ADM_vidResize25.cpp
	ADM_vidTdeint.cpp
	ADM_vidChromaShift.cpp
	ADM_vidAnimated.cpp
	ADM_vidDvbSub.cpp
	ADM_vidLogo.cpp
	ADM_vidHue.cpp
	ADM_vidSRT.cpp
	ADM_vidSRTload.cpp
	ADM_vidSRTRender.cpp
	ADM_vidCrop.cpp
	ADM_vidEq2.cpp
	ADM_vidContrast.cpp
	ADM_vidDelta.cpp
	ADM_vidUnblend.cpp
	ADM_vidAsharp.cpp
	ADM_vidVobsub.cpp
	ADM_vidVobSubRender.cpp
	ADM_vobsubinfo.cpp
	ADM_vidComputeAverage.cpp
	ADM_vidThreshold.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_videoFilter ${ADM_videoFilter_SRCS})

IF (APPLE AND ADM_DEBUG)
	SET_SOURCE_FILES_PROPERTIES(ADM_vidYadif_asm.c PROPERTIES COMPILE_FLAGS "-O2")
ENDIF (APPLE AND ADM_DEBUG)

IF (USE_FREETYPE)
	ADD_SOURCE_CFLAGS(ADM_vidSRT.cpp "${FREETYPE2_CFLAGS}")
	ADD_SOURCE_CFLAGS(ADM_vidSRTload.cpp "${FREETYPE2_CFLAGS}" "-I${LIBICONV_INCLUDE_DIR}")
	ADD_SOURCE_CFLAGS(ADM_vidSRTRender.cpp "${FREETYPE2_CFLAGS}")
ENDIF (USE_FREETYPE)

IF (ADM_CPU_X86)
	ADD_SOURCE_CFLAGS(ADM_vidDecDec.cpp "-DUSE_SSE")
ENDIF (ADM_CPU_X86)