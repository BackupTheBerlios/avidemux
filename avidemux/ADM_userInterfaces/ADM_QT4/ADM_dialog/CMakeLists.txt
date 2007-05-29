################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogQt4)
#
SET(${ADM_LIB}_SRCS
alert_qt4.cpp
AQ_asharp.cpp
AQ_chromashift.cpp
AQ_contrast.cpp
AQ_crop.cpp
AQ_encoding.cpp
AQ_hue.cpp
AQ_props.cpp
AQ_resizing.cpp
AQ_srt.cpp
AQ_working.cpp
AT_flyDialog.cpp
AT_index_pg.cpp
AT_QCanvas.cpp
DIA_busy.cpp
DIA_none.cpp
DIA_srt.cpp
DIA_x264.cpp
DIA_xvid4.cpp
OCR_none.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. ../../../ADM_osSupport ../../../ADM_inputs)
include_directories(${QT_INCLUDES})

if(FT_FOUND)
add_definitions(${FT_CFLAGS})
endif(FT_FOUND)
