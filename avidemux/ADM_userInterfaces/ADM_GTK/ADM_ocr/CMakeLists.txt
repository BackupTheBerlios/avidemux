################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_ocrGtk)
#
SET(${ADM_LIB}_SRCS
adm_ocr.cpp
adm_editglyph.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
ADD_definitions(${GTK_CFLAGS})
include_directories(.. ../../../ADM_ocr)
include_directories(../../../ADM_inputs)
