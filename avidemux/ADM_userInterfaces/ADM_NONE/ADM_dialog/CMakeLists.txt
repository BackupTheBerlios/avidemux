include_directories(../../ADM_commonUI)
include_directories(../../../ADM_inputs)

SET(ADM_dialogNone_SRCS 
alert_none.cpp  DIA_busy.cpp  DIA_encoding.cpp  DIA_flyDialog.cpp  DIA_indexing.cpp  DIA_none.cpp  DIA_working.cpp  OCR_none.cpp
)
ADD_LIBRARY(ADM_dialogNone STATIC ${ADM_dialogNone_SRCS})
