################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogQt4)
#
SET(${ADM_LIB}_Q
asharp
chromashift
contrast
crop
encoding
hue
props
resizing
jobs
working
)
#
SET(${ADM_LIB}_T
flyDialog
index_pg
QCanvas
)
#
SET(${ADM_LIB}_SRCS
alert_qt4.cpp
DIA_busy.cpp
DIA_none.cpp
#DIA_srt.cpp
DIA_x264.cpp
DIA_xvid4.cpp
OCR_none.cpp
)

IF(USE_FREETYPE)
SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} DIA_srt.cpp)
SET(${ADM_LIB}_Q ${${ADM_LIB}_Q} srt)
ENDIF(USE_FREETYPE)

################################
include(adm_qt4)
################################
include_directories(.. ../../../ADM_osSupport ../../../ADM_inputs)
include_directories(${QT_INCLUDES})


