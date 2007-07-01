################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_commonUI)
#
SET(${ADM_LIB}_SRCS
DIA_audioconfig.cpp   DIA_DVDff.cpp           DIA_flyContrast.cpp  DIA_flySrtPos.cpp   DIA_mjpeg.cpp     DIA_resizeWiz.cpp  DIA_v2v.cpp
DIA_audioFilter.cpp   DIA_enter.cpp           DIA_flyCrop.cpp      DIA_gototime.cpp    DIA_postproc.cpp  DIA_srt.cpp        GUI_render.cpp
DIA_bitrateHisto.cpp  DIA_flyAsharp.cpp       DIA_flyDialog.cpp    DIA_lavcodec.cpp    DIA_prefs.cpp     DIA_SVCD.cpp       GUI_sdlRender.cpp
DIA_builtin.cpp       DIA_flyChromaShift.cpp  DIA_flyHue.cpp       DIA_lavDecoder.cpp  DIA_requant.cpp   DIA_tdeint.cpp     GUI_xvRender.cpp
DIA_animated.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_userInterfaces)

####################################"
include_directories(./..)
SDLify(GUI_sdlRender.cpp)
