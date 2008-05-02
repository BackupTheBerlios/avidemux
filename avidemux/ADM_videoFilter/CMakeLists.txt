SET(ADM_videoFilter_SRCS 
	ADM_lavpp_deint.cpp      ADM_vidFields.cpp           
	ADM_vidPulldown.cpp       ADM_vidSwapSmart.cpp
	ADM_vidDeintASM.cpp         ADM_vidMosaic.cpp
	ADM_vidASS.cpp           ADM_vidDeinterlace.cpp      ADM_vidFlux.cpp             
	ADM_vidDenoise.cpp          
	ADM_vidRotate.cpp         ADM_vidWhirl.cpp
	ADM_vidMSharpen.cpp
	ADM_vidSeparateField.cpp  
	ADM_vidBSmear.cpp        ADM_vidDropOut.cpp          ADM_vidKernelDeint.cpp      ADM_vidMsmooth.cpp
	ADM_vidSoften.cpp
	ADM_vidPalShift.cpp
	ADM_vidStabilize.cpp
	ADM_vidPalSmart.cpp
	ADM_vidSwapFields.cpp
	ADM_guiResize.cpp  ADM_resizebis.cpp  ADM_resizeter.cpp  ADM_vidResize.cpp
	ADM_vidMPLResize.cpp
	ADM_vidResize25.cpp
	ADM_vidTdeint.cpp
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
	ADM_vidAsharp.cpp
	ADM_vidVobsub.cpp
	ADM_vidVobSubRender.cpp
	ADM_vobsubinfo.cpp
	ADM_vidComputeAverage.cpp
	ADM_vidChromaShift.cpp
	ADM_vidBlend.cpp
	ADM_vidThreshold.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_videoFilter ${ADM_videoFilter_SRCS})


IF (USE_FREETYPE)
	ADD_SOURCE_CFLAGS(ADM_vidSRT.cpp "${FREETYPE2_CFLAGS}")
	ADD_SOURCE_CFLAGS(ADM_vidSRTload.cpp "${FREETYPE2_CFLAGS}" "-I${LIBICONV_INCLUDE_DIR}")
	ADD_SOURCE_CFLAGS(ADM_vidSRTRender.cpp "${FREETYPE2_CFLAGS}")
ENDIF (USE_FREETYPE)