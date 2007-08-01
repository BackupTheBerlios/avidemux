################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_ocrQt4)
#
SET(${ADM_LIB}_Q
ocr
)
#
SET(${ADM_LIB}_T
)
#
SET(${ADM_LIB}_SRCS
)


################################
include(adm_qt4)
################################
include_directories(.. ../../../ADM_osSupport ../../../ADM_inputs)
include_directories(${QT_INCLUDES})


