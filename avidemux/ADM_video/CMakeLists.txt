SET(ADM_video_SRCS 
ADM_cache.cpp            ADM_resizebis.cpp       ADM_vidCNR2.cpp       ADM_vidFieldUnblend.cpp  ADM_vidPartial.cpp    ADM_vidTdeint.cpp
ADM_confCouples.cpp      ADM_resizeter.cpp       ADM_vidColorYuv.cpp   ADM_vidFieldUtil.cpp     ADM_vidRaw.cpp        ADM_vidUnblend.cpp
ADM_genvideo.cpp         ADM_vidAnimated.cpp     ADM_vidContrast.cpp   ADM_vidFont.cpp          ADM_vidResize25.cpp   ADM_vidVobsub.cpp
ADM_guiResize.cpp        ADM_vidAsharp.cpp       ADM_vidCrop.cpp       ADM_vidHue.cpp           ADM_vidResize.cpp     ADM_vidVobSubRender.cpp
ADM_interlaced.cpp       ADM_vidCached.cpp       ADM_vidDelta.cpp      ADM_vidMPdelogo.cpp      ADM_vidSRT.cpp        ADM_vobsubinfo.cpp
ADM_mpdetc.cpp           ADM_vidChromaShift.cpp  ADM_vidEq2.cpp        ADM_vidMPLResize.cpp     ADM_vidSRTload.cpp
ADM_vidClean.cpp        ADM_vidEqualizer.cpp  ADM_vidNull.cpp          ADM_vidSRTRender.cpp
)
ADD_LIBRARY(ADM_video STATIC ${ADM_video_SRCS})
if(FT_FOUND)
add_definitions(${FT_CFLAGS})
endif(FT_FOUND)
include_directories(../ADM_inputs)