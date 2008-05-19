SET(ADM_videoFilter_SRCS 
	ADM_vidASS.cpp               
	ADM_vidRotate.cpp         
	ADM_vidTdeint.cpp
	ADM_vidAnimated.cpp
	ADM_vidDvbSub.cpp
	ADM_vidHue.cpp
	ADM_vidSRT.cpp
	ADM_vidSRTload.cpp
	ADM_vidSRTRender.cpp
	ADM_vidCrop.cpp
	ADM_vidEq2.cpp
	ADM_vidContrast.cpp
	ADM_vidAsharp.cpp
	ADM_vidVobsub.cpp
	ADM_vidVobSubRender.cpp
	ADM_vobsubinfo.cpp
	ADM_vidComputeAverage.cpp
	ADM_vidThreshold.cpp)

ADD_ADM_LIB_ALL_TARGETS(ADM_videoFilter ${ADM_videoFilter_SRCS})


IF (USE_FREETYPE)
	ADD_SOURCE_CFLAGS(ADM_vidSRT.cpp "${FREETYPE2_CFLAGS}")
	ADD_SOURCE_CFLAGS(ADM_vidSRTload.cpp "${FREETYPE2_CFLAGS}" "-I${LIBICONV_INCLUDE_DIR}")
	ADD_SOURCE_CFLAGS(ADM_vidSRTRender.cpp "${FREETYPE2_CFLAGS}")
ENDIF (USE_FREETYPE)
