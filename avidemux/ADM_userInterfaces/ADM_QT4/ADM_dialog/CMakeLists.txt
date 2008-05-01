SET(ADM_LIB ADM_dialogQt4)

SET(uiFiles
	asharp.ui  chromashift.ui  contrast.ui  eq2.ui  crop.ui  encoding.ui  hue.ui  jobs.ui  props.ui
	resizing.ui  vobsub.ui  working.ui  x264_matrix.ui  x264_zones.ui  x264.ui)

SET(headers
	Q_asharp.h  Q_chromashift.h  Q_contrast.h  Q_crop.h  Q_encoding.h  Q_eq2.h  Q_hue.h  Q_jobs.h
	Q_props.h  Q_resizing.h  Q_vobsub.h  Q_working.h  Q_x264.h  Q_x264_matrix.h  Q_x264_zones.h
	T_index_pg.h)

IF (USE_FREETYPE)
	SET(uiFiles ${uiFiles} srt.ui)
	SET(headers ${headers} Q_srt.h)
	SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS} Q_srt.cpp)
ENDIF (USE_FREETYPE)

QT4_WRAP_UI(${ADM_LIB}_headers ${uiFiles})
QT4_WRAP_CPP(${ADM_LIB}_source ${headers})

SET(${ADM_LIB}_SRCS ${${ADM_LIB}_SRCS}
	${${ADM_LIB}_headers}  ${${ADM_LIB}_source}  Q_asharp.cpp  Q_chromashift.cpp  Q_contrast.cpp
	Q_crop.cpp  Q_encoding.cpp  Q_eq2.cpp  Q_hue.cpp  Q_jobs.cpp  Q_props.cpp  Q_resizing.cpp
	Q_vobsub.cpp  Q_working.cpp  Q_x264.cpp  Q_x264_matrix.cpp  Q_x264_zones.cpp  T_flyDialog.cpp
	T_index_pg.cpp  T_QCanvas.cpp  alert_qt4.cpp  DIA_busy.cpp  DIA_none.cpp  DIA_xvid4.cpp
	OCR_none.cpp)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDE_DIR})
ADD_ADM_LIB_QT4_TARGET(${ADM_LIB} ${${ADM_LIB}_SRCS})