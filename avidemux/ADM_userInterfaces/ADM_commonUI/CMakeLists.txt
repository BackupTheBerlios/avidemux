SET(ADM_LIB ADM_commonUI)

SET(${ADM_LIB}_SRCS
	DIA_audioconfig.cpp   DIA_DVDff.cpp           DIA_flyContrast.cpp  DIA_flySrtPos.cpp   DIA_mjpeg.cpp     DIA_resizeWiz.cpp  DIA_v2v.cpp
	DIA_audioFilter.cpp   DIA_enter.cpp           DIA_flyCrop.cpp      DIA_gototime.cpp    DIA_postproc.cpp  DIA_srt.cpp        GUI_render.cpp
	DIA_bitrateHisto.cpp  DIA_flyAsharp.cpp       DIA_flyDialog.cpp    DIA_lavcodec.cpp    DIA_prefs.cpp     DIA_SVCD.cpp       GUI_sdlRender.cpp
	DIA_builtin.cpp       DIA_flyChromaShift.cpp  DIA_flyHue.cpp       DIA_lavDecoder.cpp  DIA_requant.cpp   DIA_tdeint.cpp     GUI_xvRender.cpp
	DIA_animated.cpp      DIA_ocr.cpp             DIA_tsSub.cpp        DIA_flyEq2.cpp      DIA_flv1.cpp      DIA_flyThreshold.cpp 
	DIA_flyParticle.cpp   DIA_flySwissArmyKnife.cpp)

ADD_ADM_LIB_ALL_TARGETS(${ADM_LIB} ${${ADM_LIB}_SRCS})

SDLify(GUI_sdlRender.cpp)