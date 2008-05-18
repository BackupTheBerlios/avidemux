SET(ADM_LIB ADM_dialogQt4)

SET(uiFiles
	about.ui  asharp.ui  chromashift.ui  contrast.ui  eq2.ui  crop.ui  encoding.ui  hue.ui  license.ui
	jobs.ui  props.ui    vobsub.ui  working.ui)

SET(headers
	Q_about.h  Q_asharp.h  Q_chromashift.h  Q_contrast.h  Q_crop.h  Q_encoding.h  Q_eq2.h  Q_hue.h
	Q_license.h  Q_jobs.h  Q_props.h    Q_vobsub.h  Q_working.h  T_index_pg.h)

IF (USE_FREETYPE)
	SET(uiFiles ${uiFiles} srt.ui)
	SET(headers ${headers} Q_srt.h)
	SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} Q_srt.cpp)
ENDIF (USE_FREETYPE)

QT4_WRAP_UI(${ADM_LIB}_headers ${uiFiles})
QT4_WRAP_CPP(${ADM_LIB}_source ${headers})
QT4_ADD_RESOURCES(${ADM_LIB}_resource  about.qrc)

SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS}
	${${ADM_LIB}_headers}  ${${ADM_LIB}_source}  ${${ADM_LIB}_resource}  Q_about.cpp  Q_asharp.cpp
	Q_chromashift.cpp  Q_contrast.cpp  Q_crop.cpp  Q_encoding.cpp  Q_eq2.cpp  Q_hue.cpp  Q_license.cpp
	Q_jobs.cpp  Q_props.cpp   Q_vobsub.cpp  Q_working.cpp  T_flyDialog.cpp  T_index_pg.cpp
	T_QCanvas.cpp  alert_qt4.cpp  DIA_busy.cpp  DIA_none.cpp  DIA_xvid4.cpp  OCR_none.cpp)

ADD_DEFINITIONS(-DADM_SUBVERSION=${ADM_SUBVERSION})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
ADD_ADM_LIB_QT4_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})
