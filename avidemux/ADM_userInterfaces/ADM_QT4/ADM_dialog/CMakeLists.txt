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
srt
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
DIA_srt.cpp
DIA_x264.cpp
DIA_xvid4.cpp
OCR_none.cpp
)


################################
include(adm_qt4)
################################
ADD_definitions(-DADM_SUBVERSION=0 )
include_directories(.. ../../../ADM_osSupport ../../../ADM_inputs)
include_directories(${QT_INCLUDES})


if(FT_FOUND)
add_definitions(${FT_CFLAGS})
endif(FT_FOUND)
