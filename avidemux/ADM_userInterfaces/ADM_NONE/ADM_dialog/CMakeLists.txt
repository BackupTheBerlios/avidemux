################################
#
################################
include(AdmLib)
SET(ADM_LIB ADM_dialogNone)
#
SET(${ADM_LIB}_SRCS 
alert_none.cpp  DIA_busy.cpp  DIA_encoding.cpp  DIA_flyDialog.cpp  DIA_indexing.cpp  DIA_none.cpp  DIA_working.cpp  OCR_none.cpp
)
ADD_LIBRARY(${ADM_LIB} STATIC ${${ADM_LIB}_SRCS})
ADD_ADM_LIB(${ADM_LIB} ADM_libraries)


################################
include_directories(../../../ADM_inputs)
