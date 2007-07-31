
SET(ADM_ocr_SRCS 
adm_glyph.cpp  adm_leftturn.cpp  ADM_vob2vobsub.cpp
ADM_ocr.cpp
ADM_bitmapSource.cpp
ADM_ocrEngine.cpp
)
ADD_LIBRARY(ADM_ocr STATIC ${ADM_ocr_SRCS})
include_directories(../ADM_inputs)
