
SET(ADM_commonUI_SRCS 
DIA_audioconfig.cpp   DIA_DVDff.cpp           DIA_flyContrast.cpp  DIA_flySrtPos.cpp   DIA_mjpeg.cpp     DIA_resizeWiz.cpp  DIA_v2v.cpp
DIA_audioFilter.cpp   DIA_enter.cpp           DIA_flyCrop.cpp      DIA_gototime.cpp    DIA_postproc.cpp  DIA_srt.cpp        GUI_render.cpp
DIA_bitrateHisto.cpp  DIA_flyAsharp.cpp       DIA_flyDialog.cpp    DIA_lavcodec.cpp    DIA_prefs.cpp     DIA_SVCD.cpp       GUI_sdlRender.cpp
DIA_builtin.cpp       DIA_flyChromaShift.cpp  DIA_flyHue.cpp       DIA_lavDecoder.cpp  DIA_requant.cpp   DIA_tdeint.cpp     GUI_xvRender.cpp
)

ADD_LIBRARY(ADM_commonUI STATIC ${ADM_commonUI_SRCS})
include_directories(./..)
