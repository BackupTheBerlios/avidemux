SET(ADM_LIB ADM_ocrQt4)

QT4_WRAP_UI(${ADM_LIB}_header  ocr.ui)
QT4_WRAP_CPP(${ADM_LIB}_source  Q_ocr.h)

SET(${ADM_LIB}_SRCS
	Q_ocr.cpp  ${${ADM_LIB}_header}  ${${ADM_LIB}_source})

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_ADM_LIB_QT4_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})